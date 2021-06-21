#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlTableModel>

bool init_db_connect();

void set_db_model_header_data(QSqlTableModel *model);

#endif // DATABASE_H
