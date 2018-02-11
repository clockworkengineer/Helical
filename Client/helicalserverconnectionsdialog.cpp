/*
 * File:   helicalserverconnectionsdialog.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

//
// Class: HelicalServerConnectionsDialog
//
// Description: Class to create and display server settings creation/editor dialog.
//

// =============
// INCLUDE FILES
// =============

#include "helicalserverconnectionsdialog.h"
#include "ui_helicalserverconnectionsdialog.h"

#include "helicalmainwindow.h"
#include "helicalserverdetailsdialog.h"

/**
 * @brief HelicalServerConnectionsDialog::HelicalServerConnectionsDialog
 *
 * Read connections list and display dialog.
 *
 * @param parent
 */
HelicalServerConnectionsDialog::HelicalServerConnectionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelicalServerConnectionsDialog)
{
    ui->setupUi(this);

    loadConnectionsList();
    populateConnectionList();

    connect(this, &HelicalServerConnectionsDialog::connectToServer, static_cast<HelicalMainWindow*>(parent), &HelicalMainWindow::connectToServer);

}

/**
 * @brief HelicalServerConnectionsDialog::~HelicalServerConnectionsDialog
 *
 * Destroy dialog.
 * s
 */
HelicalServerConnectionsDialog::~HelicalServerConnectionsDialog()
{
    delete ui;
}

/**
 * @brief HelicalServerConnectionsDialog::loadConnectionsList
 *
 * Load connections list.
 *
 */
void HelicalServerConnectionsDialog::loadConnectionsList()
{

    QSettings helicalSettings;
    helicalSettings.beginGroup("ConnectionList");
    m_connectionList = helicalSettings.value("connections").toStringList();
    helicalSettings.endGroup();

}

/**
 * @brief HelicalServerConnectionsDialog::populateConnectionList
 *
 * Add current connections list to dialog and save away to settings.
 *
 */
void HelicalServerConnectionsDialog::populateConnectionList()
{

    ui->connectionList->clear();
    ui->connectionList->addItems(m_connectionList);
    saveConnectionList();

}

/**
 * @brief HelicalServerConnectionsDialog::saveConnectionList
 *
 * Save connections list to settings.
 *
 */
void HelicalServerConnectionsDialog::saveConnectionList()
{

    QSettings helicalSettings;
    helicalSettings.beginGroup("ConnectionList");
    helicalSettings.setValue("connections",m_connectionList);
    helicalSettings.endGroup();
}

/**
 * @brief HelicalServerConnectionsDialog::on_newServerButton_clicked
 *
 * Display new connection details dialog.
 *
 */
void HelicalServerConnectionsDialog::on_newServerButton_clicked()
{

    HelicalServerDetailsDialog serverDetails("");
    serverDetails.exec();

    QString connectionName = serverDetails.connectionName();
    if (!connectionName.isEmpty()) {
        m_connectionList.append(connectionName);
        populateConnectionList();
    }

}

/**
 * @brief HelicalServerConnectionsDialog::on_editServerButton_clicked
 *
 * Display edit connection details dialog.
 *
 */
void HelicalServerConnectionsDialog::on_editServerButton_clicked()
{

    QListWidgetItem *connectionToEdit= ui->connectionList->currentItem();

    if(connectionToEdit != nullptr) {
        HelicalServerDetailsDialog serverDetails(connectionToEdit->text());
        serverDetails.exec();
    }
}

/**
 * @brief HelicalServerConnectionsDialog::on_removeServerButton_clicked
 *
 * Remove connection from connection list.
 *
 */
void HelicalServerConnectionsDialog::on_removeServerButton_clicked()
{
    QListWidgetItem *connectionToDelete = ui->connectionList->currentItem();

    if(connectionToDelete != nullptr) {
        QString connectionName = connectionToDelete->text();
        int connectionIndex = m_connectionList.indexOf(connectionName);
        if (connectionIndex != -1) {
            m_connectionList.removeAt(connectionIndex);
            populateConnectionList();
        }
        QSettings hedlicalSettings;
        hedlicalSettings.beginGroup(connectionName);
        hedlicalSettings.remove("");
        hedlicalSettings.endGroup();

    }

}

/**
 * @brief HelicalServerConnectionsDialog::on_connectServerButton_clicked
 *
 * Connect to given SSH server on connection list.
 */
void HelicalServerConnectionsDialog::on_connectServerButton_clicked()
{

    QListWidgetItem *connectionToRun= ui->connectionList->currentItem();

    if(connectionToRun != nullptr) {
        emit connectToServer(connectionToRun->text());
    }
}
