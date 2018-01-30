#include "heilcalmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HeilcalMainWindow w;
    w.show();

    return a.exec();
}
