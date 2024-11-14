#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QStandardPaths>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void addTask();

    void selectDay();

    void on_saveTaskButton_clicked();

    void on_chooseColorButton_clicked();

    void changePenWidth();

private:
    Ui::MainWindow *ui;

    bool drawing{false};

    QPoint last_point;

    // copy of loaded image for drawing
    QImage modified_image;

    QColor pen_color{Qt::black};

    int pen_width{5};

    // get user's home path
    const QString home_path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    QSqlDatabase *database{nullptr};

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

};
#endif // MAINWINDOW_H
