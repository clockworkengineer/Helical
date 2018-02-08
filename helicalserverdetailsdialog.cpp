/*
 * File:   helicalserverdetailsdialog.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

//
// Class: HelicalServerDetailsDialog
//
// Description:
//

// =============
// INCLUDE FILES
// =============

#include "helicalserverdetailsdialog.h"
#include "ui_helicalserverdetailsdialog.h"

HelicalServerDetailsDialog::HelicalServerDetailsDialog(const QString& connectionName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelicalServerDetailsDialog),
    m_connectionName{connectionName}
{
    ui->setupUi(this);

    if (!connectionName.isEmpty()) {
        QSettings helicalSettings;
        helicalSettings.beginGroup(m_connectionName);
        m_serverName= helicalSettings.value("server").toString();
        m_serverPort = helicalSettings.value("port").toString();
        m_userName = helicalSettings.value("user").toString();
        m_userPassword =  helicalSettings.value("password").toString();
        m_command =  helicalSettings.value("command").toString();
        helicalSettings.endGroup();
    }
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
    m_command = ui->commandLineEdit->text();

    QSettings helicalSettings;
    helicalSettings.beginGroup(m_connectionName);
    helicalSettings.setValue("server",m_serverName);
    helicalSettings.setValue("port",m_serverPort);
    helicalSettings.setValue("user",m_userName);
    helicalSettings.setValue("password",m_userPassword);
    helicalSettings.setValue("command",m_command);
    helicalSettings.endGroup();

    close();

}

QString HelicalServerDetailsDialog::connectionName() const
{
    return m_connectionName;
}

void HelicalServerDetailsDialog::showEvent(QShowEvent *event)
{

        QWidget::showEvent( event );

        if (!m_connectionName.isEmpty()) {
            ui->connectionNameLineEdit->setEnabled(false);
        }

        ui->connectionNameLineEdit->setText(m_connectionName);
        ui->serverNameLineEdit->setText(m_serverName);
        ui->serverPortLineEdit->setText(m_serverPort);
        ui->userNameLineEdit->setText(m_userName);
        ui->userPasswordLineEdit->setText(m_userPassword);
        ui->commandLineEdit->setText(this->m_command);


}

