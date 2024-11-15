#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QCalendarWidget>
#include <QDebug>
#include <QPainter>
#include <QSpinBox>
#include <QTime>
#include <QDir>
#include <QFile>
#include <QSqlQuery>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create a database from scratch if it doesn't exist in the user's home path
    // Check if the directory exists; if not, create it
    const QString db_path = home_path + "/daily_tasks_app/data.db";

    QDir dir = QFileInfo(db_path).absoluteDir();

    if(!dir.exists()){
        if(!dir.mkpath(".")){
            QMessageBox::warning(this, "Database error",
                                 "Failed while creating databse directory");
            exit(1);
        }
    }

    // Check if the database file exists
    if(!QFile::exists(db_path)){
        // Setup the SQLite database connection
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

        db.setDatabaseName(db_path);

        // Open the database connection
        if(!db.open()){
            QMessageBox::warning(this, "Databse error",
                                 "Failed while openeing the database");
            exit(1);
        }

        // Create the "tasks" table
        QSqlQuery query;

        qDebug() << "Preparing to create a table from scratch";

        const QString create_table_query = "CREATE TABLE IF NOT EXISTS tasks ("
                                          "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                          "date VARCHAR(20) NOT NULL, "
                                          "file_path VARCHAR(50) UNIQUE NOT NULL);";

        if(!query.exec(create_table_query)){
            QMessageBox::warning(this, "Database error",
                                 "Failed while creating the table 'tasks'");
            exit(1);
        }

        db.close();
    }

    // prepare database manager
    database_manager.prepareManager(this);

    // Iamge label settings
    ui->image_label->setAlignment(Qt::AlignCenter);

    // Pen width spin box settings
    ui->penWidthSpinBox->setMinimum(1);
    ui->penWidthSpinBox->setMaximum(50);
    ui->penWidthSpinBox->setValue(pen_width);

    // Make next and previous task buttons disabled
    ui->previousTaskBtn->setDisabled(true);
    ui->nextTaskBtn->setDisabled(true);

    // Make save task button disabled
    ui->saveTaskButton->setDisabled(true);

    // Calendar settings
    ui->calendar->setLocale(QLocale::English);
    ui->calendar->setGridVisible(true);
    ui->calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    ui->calendar->setFirstDayOfWeek(Qt::Monday);

    // Signals and slots
    //
    // When user wants to add a new task (click ENTER on calendar date)
    connect(ui->calendar, &QCalendarWidget::activated, this, &MainWindow::addTask);

    // When user wants to watch tasks for a particular date.
    connect(ui->calendar, &QCalendarWidget::clicked, this, &MainWindow::selectDay);

    //When user wants to change pen width
    connect(ui->penWidthSpinBox, &QSpinBox::valueChanged, this, &MainWindow::changePenWidth);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addTask()
{
    /*
        This private slots allows user to add a new task to the database.
        When user click ENTER on selected date it starts a dialog for
        loading a new image with a schedule.
    */

    ui->saveTaskButton->setDisabled(false);

    const QString file_name = QFileDialog::getOpenFileName(this, "Open Image", home_path,
                                               "Images (*.png *.xpm *.jpg *.jpeg *.bmp)");

    if (file_name.isEmpty()){
        // empty file. simply return nothing.
        return;
    }

    // Loading an image
    QImage image(file_name);

    if (image.isNull()){
        QMessageBox::warning(this, "Image loading error", "Failed to load image.");
        return;
    }

    // Converting QImage to QPixmap for displaying it in QLabel
    ui->image_label->setPixmap(QPixmap::fromImage(image));
    ui->image_label->adjustSize();

    // copy original for the next purposes (drawing)
    modified_image = image;
}

void MainWindow::selectDay()
{
    /*
        Function allows user to watch tasks for the desired date.
        User just has to click by mouse on a particular date.
        Function goes to the databse and loads a file with tasks.
    */

    ui->previousTaskBtn->setDisabled(false);
    ui->nextTaskBtn->setDisabled(false);
    ui->saveTaskButton->setDisabled(false);

    // Clear selected day tasks container
    selected_day_tasks.clear();

    // Drop tasks counter
    tasks_counter = 0;


    // First of all clear image label
    ui->image_label->clear();

    // open it and load tasks image/images into the label
    QSqlQuery query(database_manager.getDatabase());

    query.prepare("SELECT file_path FROM tasks WHERE date(date) = :selected_date");

    // Get data using selected date on the calendar
    query.bindValue(":selected_date", ui->calendar->selectedDate().toString("yyyy-MM-dd"));

    if(!query.exec()){
        QMessageBox::warning(this, "Database error",
                             "Couldn't open the task/tasks. Reload app");

        return;
    }
    else{

        // QStringList selected_day_tasks;
        while(query.next()){
            // Push image/images in the container (path/pathes)
            selected_day_tasks.push_back(query.value(0).toString());
        }

        // Check if there is any task in the container
        if(selected_day_tasks.isEmpty()){
            ui->image_label->setText("No tasks on this day.");
            return;
        }

        // Check how many tasks on selected days and disabled
        if(tasks_counter == 0 && selected_day_tasks.size() == 1){
            ui->previousTaskBtn->setDisabled(true);
            ui->nextTaskBtn->setDisabled(true);
        }
        else{
            ui->previousTaskBtn->setDisabled(false);
            ui->nextTaskBtn->setDisabled(false);
        }

        // Check and load the first task from container
        QImage image(selected_day_tasks.first());
        if (image.isNull()){
            QMessageBox::warning(this, "Image loading error", "Failed to load image.");
            return;
        }

        // Converting QImage to QPixmap for displaying it in QLabel
        ui->image_label->setPixmap(QPixmap::fromImage(image));
        ui->image_label->adjustSize();

        // copy original for the next purposes (drawing)
        modified_image = image;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    /*
        Overriding function mousePressEvent for drawing.
        Event with left-mouse click
    */

    // Check if event happens in the image label
    if (event->button() == Qt::LeftButton &&
        ui->image_label->rect().contains(event->pos() - ui->image_label->pos())) {
            drawing = true;
            // Convert into coordinats QLabel
            last_point = event->pos() - ui->image_label->pos();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    /*
        Overriding function mouseMoveEvent for drawing.
        The mouse moving event.
    */

    if (drawing && (event->buttons() & Qt::LeftButton)) {
        QPoint current_point = event->pos() - ui->image_label->pos();

        // Drawing a line on the image
        QPainter painter(&modified_image);

        QPen pen(pen_color, pen_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        painter.drawLine(last_point, current_point);

        last_point = current_point;

        // update QLabel
        ui->image_label->setPixmap(QPixmap::fromImage(modified_image));
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    /*
        Overriding function mouseReleaseEvent for drawing.
        When user releases left mouse button.
    */

    if (event->button() == Qt::LeftButton && drawing) {
        drawing = false;
    }
}


void MainWindow::on_saveTaskButton_clicked()
{
    /*
        Function saves all paints on the image what users has made.
    */

    // Create a directory where app will save all tasks images (processed or not)
    QDir work_dir;

    // Path to workdir
    const QString work_dir_path = home_path + "/my_tasks/";

    // Create work directory if it doesn't exist.
    if(!work_dir.exists(work_dir_path)){
        work_dir.mkpath(work_dir_path);
    }

    // Get current time in format hours:minutes:seconds
    QTime current_time = QTime::currentTime();

    QString file = ui->calendar->selectedDate().toString() + "_" + current_time.toString();

    file.replace(" ", "_");

    QString file_name = QFileDialog::getSaveFileName(this, "Save Image",
                                                     work_dir_path + file + ".jpg",
                                      "PNG (*.png);;JPEG (*.jpg *.jpeg);;BMP (*.bmp)");
    if (!file_name.isEmpty()) {
        if (!modified_image.save(file_name)) {
            QMessageBox::warning(this, "Error",
                                 "Failed to save image. "
                                 "Maybe you have forgotten to specify file's extension");
        }
    }

    QSqlQuery query(database_manager.getDatabase());
    query.prepare("INSERT INTO tasks(date, file_path) VALUES (:sel_date, :cur_file_path)");

    query.bindValue(":sel_date", ui->calendar->selectedDate().toString("yyyy-MM-dd"));
    query.bindValue(":cur_file_path", file_name.replace(" ", "_"));

    if(!query.exec()){
        QMessageBox::warning(this, "Database error",
                             "Couldn't save a new task. Reload app");
        return;
    }
    else{
        QMessageBox::information(this, "Report",
                                 "A new task has been added!");
    }
}


void MainWindow::on_chooseColorButton_clicked()
{
    /*
        Function allows user to choose a color of paint brush.
    */

    QColor new_pen_color = QColorDialog::getColor(pen_color, this, "Select pen color");

    if(new_pen_color.isValid()){
        pen_color = new_pen_color;
    }
}

void MainWindow::changePenWidth()
{
    /*
        Function changes pen's width.
    */

    pen_width = ui->penWidthSpinBox->value();
}


void MainWindow::on_nextTaskBtn_clicked()
{
    /*
        Function allows user to watch a next task
    */

    ui->previousTaskBtn->setDisabled(false);

    // Check is there one more task
    if(selected_day_tasks.size() > tasks_counter + 1){
        QImage image(selected_day_tasks[++tasks_counter]);
        if (image.isNull()){
            QMessageBox::warning(this, "Image loading error", "Failed to load image.");
            return;
        }

        // Converting QImage to QPixmap for displaying it in QLabel
        ui->image_label->setPixmap(QPixmap::fromImage(image));
        ui->image_label->adjustSize();

        // copy original for the next purposes (drawing)
        modified_image = image;

        if((tasks_counter + 1) == selected_day_tasks.size()){
            ui->nextTaskBtn->setDisabled(true);
        }
    }
    else{
        return;
    }

}


void MainWindow::on_previousTaskBtn_clicked()
{
    /*
        Function allows user to watch a previous task
    */

    ui->nextTaskBtn->setDisabled(false);

    --tasks_counter;

    if(tasks_counter >= 0){
        QImage image(selected_day_tasks[tasks_counter]);

        if (image.isNull()){
            QMessageBox::warning(this, "Image loading error", "Failed to load image.");
            return;
        }

        // Converting QImage to QPixmap for displaying it in QLabel
        ui->image_label->setPixmap(QPixmap::fromImage(image));
        ui->image_label->adjustSize();

        // copy original for the next purposes (drawing)
        modified_image = image;

        if(tasks_counter == 0){
            ui->previousTaskBtn->setDisabled(true);
        }
    }
    else{
        return;
    }
}

