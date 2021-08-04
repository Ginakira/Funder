#include "include/mainwindow.h"

#include <QApplication>

#include "include/settings.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle(QString("Funder v%1").arg(APP_VERSION));
    w.show();
    w.init();
    return a.exec();
}
