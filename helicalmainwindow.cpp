#include "heilcalmainwindow.h"
#include "ui_heilcalmainwindow.h"

HeilcalMainWindow::HeilcalMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HeilcalMainWindow)
{
    ui->setupUi(this);
}

HeilcalMainWindow::~HeilcalMainWindow()
{
    delete ui;
}
