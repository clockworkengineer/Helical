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
// Description: Class to display/edit server connection details dialog.
//

// =============
// INCLUDE FILES
// =============

#include "helicalserverdetailsdialog.h"
#include "ui_helicalserverdetailsdialog.h"

/**
 * @brief HelicalServerDetailsDialog::HelicalServerDetailsDialog
 *
 * Create/Display connection details dialog. If connection name is not empty then
 * the dialog is open for editing of an existing connection otherwise creation of a new
 * session.
 *
 * @param connectionName
 * @param parent
 */
HelicalServerDetailsDialog::HelicalServerDetailsDialog(const QString& connectionName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelicalServerDetailsDialog)

{
    ui->setupUi(this);

    if (!connectionName.isEmpty()) {
        m_serverConnectionSettings.load(connectionName);
    }

}

/**
 * @brief HelicalServerDetailsDialog::~HelicalServerDetailsDialog
 *
 * Destroy server connectiosn details dialog.
 *
 */
HelicalServerDetailsDialog::~HelicalServerDetailsDialog()
{
    delete ui;
}

/**
 * @brief HelicalServerDetailsDialog::on_cancelButton_clicked
 *
 * Cancel edit connection dettails.
 *
 */
void HelicalServerDetailsDialog::on_cancelButton_clicked()
{

   close();

}

/**
 * @brief HelicalServerDetailsDialog::on_saveButton_clicked
 *
 * Save connection details away to settings.
 *
 */
void HelicalServerDetailsDialog::on_saveButton_clicked()
{

    m_serverConnectionSettings.setConnectionName(ui->connectionNameLineEdit->text());
    m_serverConnectionSettings.setServerName(ui->serverNameLineEdit->text());
    m_serverConnectionSettings.setServerPort(ui->serverPortLineEdit->text());
    m_serverConnectionSettings.setUserName(ui->userNameLineEdit->text());
    m_serverConnectionSettings.setUserPassword(ui->userPasswordLineEdit->text());
    m_serverConnectionSettings.setUserHome(ui->userHomeLineEdit->text());
    m_serverConnectionSettings.setCommand(ui->commandLineEdit->text());
    m_serverConnectionSettings.setTerminalSize(static_cast<HelicalServerConnectionSettings::TerminalSize>((ui->terminalSizeMenu->currentIndex())));
    m_serverConnectionSettings.save();

    close();

}

/**
 * @brief HelicalServerDetailsDialog::connectionName
 *
 * Get connection name.
 *
 * @return
 */
QString HelicalServerDetailsDialog::connectionName() const
{
    return m_serverConnectionSettings.connectionName();
}

/**
 * @brief HelicalServerDetailsDialog::showEvent
 *
 * Intialise dialog from settings for show event.
 *
 * @param event
 */
void HelicalServerDetailsDialog::showEvent(QShowEvent *event)
{

        QWidget::showEvent( event );

        if (!m_serverConnectionSettings.connectionName().isEmpty()) {
            ui->connectionNameLineEdit->setEnabled(false);
        }

        ui->connectionNameLineEdit->setText(m_serverConnectionSettings.connectionName());
        ui->serverNameLineEdit->setText(m_serverConnectionSettings.serverName());
        ui->serverPortLineEdit->setText(m_serverConnectionSettings.serverPort());
        ui->userNameLineEdit->setText(m_serverConnectionSettings.userName());
        ui->userPasswordLineEdit->setText(m_serverConnectionSettings.userPassword());
        ui->userHomeLineEdit->setText(m_serverConnectionSettings.userHome());
        ui->commandLineEdit->setText(m_serverConnectionSettings.command());
        ui->terminalSizeMenu->setCurrentIndex(static_cast<int>(m_serverConnectionSettings.terminalSize()));

}

