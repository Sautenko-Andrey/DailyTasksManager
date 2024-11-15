#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QStandardPaths>
#include <QSqlDatabase>
#include "databasemanager.h"

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

    void on_nextTaskBtn_clicked();

    void on_previousTaskBtn_clicked();

private:
    Ui::MainWindow *ui;

    DatabaseManager &database_manager = DatabaseManager::getInstance();

    bool drawing{false};

    QPoint last_point;

    // copy of loaded image for drawing
    QImage modified_image;

    QColor pen_color{Qt::black};

    int pen_width{5};

    // get user's home path
    const QString home_path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    QStringList selected_day_tasks;

    int tasks_counter{0};

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

};

static const QString app_style = R"(
    QMainWindow {
        background-color: #2E2E2E; /* Dark background */
        color: #ffb600;           /* Gold text color */
    }

    QMenuBar {
        background-color: #1E1E1E; /* Slightly darker background for the menu bar */
        color: #FFD700;
    }

    QMenuBar::item {
        background: transparent;
        color: #FFD700;
    }

    QMenuBar::item:selected {
        background: #555555; /* Highlighted menu item */
        color: #FFD700;
    }

    QMenu {
        background-color: #1E1E1E; /* Menu dropdown background */
        color: #FFD700;
        border: 1px solid #FFD700; /* Gold border for menus */
    }

    QMenu::item:selected {
        background-color: #555555; /* Highlighted menu item */
        color: #FFD700;
    }

    QStatusBar {
        background-color: #2E2E2E;
        color: #FFD700;
    }

    QToolBar {
        background-color: #2E2E2E;
        border: 1px solid #FFD700;
        color: #FFD700;
    }

    QPushButton {
        background-color: #3E3E3E; /* Dark button background */
        color: #FFD700;
        border: 1px solid #FFD700;
        padding: 5px;
        border-radius: 5px;
    }

    QPushButton:hover {
        background-color: #555555; /* Slightly lighter background on hover */
    }

    QPushButton:pressed {
        background-color: #FFD700; /* Gold background on press */
        color: #2E2E2E;            /* Dark text on press */
    }

    QLineEdit {
        background-color: #3E3E3E;
        color: #FFD700;
        border: 1px solid #FFD700;
        border-radius: 5px;
        padding: 3px;
    }

    QComboBox {
        background-color: #3E3E3E;
        color: #FFD700;
        border: 1px solid #FFD700;
        border-radius: 5px;
        padding: 3px;
    }

    QComboBox QAbstractItemView {
        background-color: #2E2E2E;
        color: #FFD700;
        selection-background-color: #555555; /* Selected item background */
        selection-color: #FFD700;
    }

QScrollBar:vertical {
        background: #2E2E2E; /* Background color */
        border: 1px solid #FFD700; /* Gold border */
        width: 15px; /* Set width for vertical scrollbar */
    }

    QScrollBar:horizontal {
        background: #2E2E2E; /* Background color */
        border: 1px solid #FFD700; /* Gold border */
        height: 15px; /* Set height for horizontal scrollbar */
    }

    QScrollBar::handle:vertical {
        background: #555555; /* Handle color */
        border: 1px solid #FFD700;
        border-radius: 7px;
        min-height: 20px; /* Minimum handle height */
    }

    QScrollBar::handle:horizontal {
        background: #555555; /* Handle color */
        border: 1px solid #FFD700;
        border-radius: 7px;
        min-width: 20px; /* Minimum handle width */
    }

    QScrollBar::add-line, QScrollBar::sub-line {
        background: #3E3E3E;
        border: 1px solid #FFD700;
    }

    QScrollBar::add-page, QScrollBar::sub-page {
        background: none;
    }

    QSpinBox {
        background-color: #3E3E3E; /* Dark background for spinbox */
        color: #FFD700;           /* Gold text */
        border: 1px solid #FFD700;
        border-radius: 5px;
        padding: 3px;
    }

    QSpinBox::up-button {
        subcontrol-origin: border;
        subcontrol-position: top right;
        width: 16px;
        background-color: #555555; /* Button background */
        border: 1px solid #FFD700;
        border-top-right-radius: 3px;
    }

    QSpinBox::down-button {
        subcontrol-origin: border;
        subcontrol-position: bottom right;
        width: 16px;
        background-color: #555555; /* Button background */
        border: 1px solid #FFD700;
        border-bottom-right-radius: 3px;
    }

    QSpinBox::up-button:hover, QSpinBox::down-button:hover {
        background-color: #FFD700; /* Highlight buttons on hover */
    }

    QSpinBox::up-button:pressed, QSpinBox::down-button:pressed {
        background-color: #FFD700;
        color: #2E2E2E; /* Dark text on button press */
    }

    QSpinBox::up-arrow, QSpinBox::down-arrow {
        width: 8px;
        height: 8px;
        color: #FFD700; /* Arrow color */
    }

    QCalendarWidget {
        background-color: #2E2E2E; /* Calendar background */
        color: #FFD700;            /* Default text color */
        border: 1px solid #FFD700; /* Gold border around the calendar */
        border-radius: 10px;
    }

    QCalendarWidget QToolButton {
        background-color: #3E3E3E; /* Buttons for navigation */
        color: #FFD700;
        border: 1px solid #FFD700;
        border-radius: 5px;
        padding: 5px;
    }

    QCalendarWidget QToolButton:hover {
        background-color: #555555; /* Slightly lighter background on hover */
    }

    QCalendarWidget QToolButton:pressed {
        background-color: #FFD700; /* Gold background on press */
        color: #2E2E2E;            /* Dark text on press */
    }

    QCalendarWidget QHeaderView::section {
        background-color: #1E1E1E; /* Header background */
        color: #FFD700;            /* Header text */
        padding: 5px;
        border: none;
    }

    QCalendarWidget QWidget {
        background-color: #2E2E2E; /* Default cell background */
        color: #FFD700;
        border: none;
    }

    QCalendarWidget QAbstractItemView:selected {
        background-color: #FFD700; /* Gold background for selected day */
        color: #2E2E2E;            /* Dark text for selected day */
    }

    QCalendarWidget QAbstractItemView::item:hover {
        background-color: #555555; /* Highlight background on hover */
        color: #FFD700;
    }

    QCalendarWidget QAbstractItemView::item:disabled {
        background-color: #1E1E1E; /* Darker background for disabled cells */
        color: #555555;            /* Dimmed text for disabled cells */
    }
)";


#endif // MAINWINDOW_H
