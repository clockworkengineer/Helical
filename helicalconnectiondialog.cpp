#include "helicalconnectiondialog.h"
#include "ui_helicalconnectiondialog.h"

HelicalConnectionDialog::HelicalConnectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelicalConnectionDialog)
{
    ui->setupUi(this);
}

HelicalConnectionDialog::~HelicalConnectionDialog()
{
    delete ui;
}
