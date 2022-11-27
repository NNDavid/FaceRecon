#include "MainWindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    MainWindow mainWindow;
    mainWindow.setWindowState(Qt::WindowFullScreen);
    mainWindow.show();
    return app.exec();
}
