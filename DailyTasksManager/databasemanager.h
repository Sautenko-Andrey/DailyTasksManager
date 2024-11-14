#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QDir>

class DatabaseManager{

public:

    // prohibits copy constructor and copy assignment operator
    DatabaseManager(const DatabaseManager &db_manager) = delete;
    DatabaseManager& operator=(const DatabaseManager &db_manager) = delete;

    // get access to the single object
    static DatabaseManager& getInstance(){
        static DatabaseManager manager;
        return manager;
    }

    void prepareManager(QWidget *widget);

    ~DatabaseManager();

    QSqlDatabase& getDatabase();


private:
    DatabaseManager(){}    // private constructor

    QWidget *widget{nullptr};

    QSqlDatabase m_db;

    inline static const QString path_to_db = QDir::homePath() +
                                             "/daily_tasks_app/data.db";
};

#endif // DATABASEMANAGER_H
