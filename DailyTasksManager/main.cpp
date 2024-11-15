#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.setStyleSheet(app_style);
    window.setWindowTitle("Daily tasks manager");
    window.show();
    return app.exec();
}
