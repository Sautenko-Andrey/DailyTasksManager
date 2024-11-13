#include "database_manager.h"
#include <QMessageBox>

void DatabaseManager::prepareManager(QWidget *widget){
    m_db = QSqlDatabase::addDatabase("QSQLITE","Connection");
    m_db.setDatabaseName(path_to_db);

    if(!m_db.open()){
        QMessageBox::warning(widget, "Error",
                             "Couldn't load the database. Reopen the app");
        exit(1);
    }
}

DatabaseManager::~DatabaseManager() {
    m_db.close();
}


QSqlDatabase& DatabaseManager::getDatabase() {
    return m_db;
}
