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

    // prepare database manager
    database_manager.prepareManager(this);

    // Create a database from scratch if it doesn't exist in the user's home path
    create_db();

    // Query exampel
    //QSqlQuery players_query(database_manager.getDatabase());

    // Iamge label settings
    ui->image_label->setAlignment(Qt::AlignCenter);

    // Pen width spin box settings
    ui->penWidthSpinBox->setMinimum(1);
    ui->penWidthSpinBox->setMaximum(50);
    ui->penWidthSpinBox->setValue(pen_width);

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

    // First of all clear image label
    ui->image_label->clear();

    // Get current time in format hours:minutes:seconds
    QTime current_time = QTime::currentTime();

    qDebug() << ui->calendar->selectedDate().toString() + " " + current_time.toString();
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

void MainWindow::create_db()   // this function doesn't work!!!!!
{
    /*
        Function creates a databse in the user's home path
    */

    // Create the databse if not exists
    const QString db_path = home_path + "/data.db";
    if (!QFile::exists(db_path)){
        QFile file(db_path);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, "Database error", "Databse creator failed");
            exit(1);
        }
        else{
            file.close();
        }

        // Create a table
        QSqlQuery query(database_manager.getDatabase());

        QString create_table_query = "CREATE TABLE IF NOT EXISTS tasks ("
                                     "id INTEGER PRIMARY KEY, "
                                     "date VARCHAR(20) NOT NULL, "
                                     "file_path VARCHAR(100) NOT NULL);";
        if(!query.exec(create_table_query)){
            QMessageBox::warning(this, "Database error",
                                 "Couldn't create a table tasks");
            exit(1);
        }
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

