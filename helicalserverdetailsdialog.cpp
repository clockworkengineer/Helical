#include "helicalserverdetailsdialog.h"
#include "ui_helicalserverdetailsdialog.h"

HelicalServerDetailsDialog::HelicalServerDetailsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelicalServerDetailsDialog)
{
    ui->setupUi(this);
}

HelicalServerDetailsDialog::~HelicalServerDetailsDialog()
{
    delete ui;
}

void HelicalServerDetailsDialog::on_cancelButton_clicked()
{

   close();

}

void HelicalServerDetailsDialog::on_saveButton_clicked()
{

    m_connectionName = ui->connectionNameLineEdit->text();
    m_serverName = ui->serverNameLineEdit->text();
    m_serverPort = ui->serverPortLineEdit->text();
    m_userName   = ui->userNameLineEdit->text();
    m_userPassword = ui->userPasswordLineEdit->text();

    QSettings helicalSettings;
    helicalSettings.beginGroup(m_connectionName);
    helicalSettings.setValue("server",m_serverName);
    helicalSettings.setValue("port",m_serverPort);
    helicalSettings.setValue("user",m_userName);
    helicalSettings.setValue("password",m_userPassword);
    helicalSettings.endGroup();

    close();

}

QString HelicalServerDetailsDialog::connectionName() const
{
    return m_connectionName;
}

