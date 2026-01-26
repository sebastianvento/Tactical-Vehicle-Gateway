#include "MainWindow.h"

#include <QApplication>
#include <QObject>


int main(int argc, char **argv) {
    QApplication app (argc, argv);
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
