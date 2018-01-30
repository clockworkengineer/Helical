#include "helicalmainwindow.h"
#include <QApplication>
#include "QtSSH/qtssh.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication helicalApplication(argc, argv);

    HelicalMainWindow helicalMain;

    helicalMain.show();

    return helicalApplication.exec();
}
