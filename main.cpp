#include "helicalmainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication helicalApplication(argc, argv);

    HelicalMainWindow helicalMain;

    helicalMain.show();

    return helicalApplication.exec();

}
