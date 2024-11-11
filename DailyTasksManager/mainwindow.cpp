#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QCalendarWidget>
#include <QDebug>
#include <QPainter>
#include <QSpinBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    QString file_name = QFileDialog::getOpenFileName(this, "Open Image", "",
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

    qDebug() << "Selected date: " << ui->calendar->selectedDate();

    //
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

    QString file_name = QFileDialog::getSaveFileName(this, "Save Image", "",
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

