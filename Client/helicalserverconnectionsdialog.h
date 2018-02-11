/*
 * File:   helicalserverconnectionsdialog.h
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

#ifndef HELICALSERVERCONNECTIONSDIALOG_H
#define HELICALSERVERCONNECTIONSDIALOG_H

//
// Class: HelicalServerConnectionsDialog
//
// Description: Class to create and display server settings creation/editor dialog.
//

// =============
// INCLUDE FILES
// =============

#include <QDialog>
#include <QSettings>

// =================
// CLASS DECLARATION
// =================

namespace Ui {
class HelicalServerConnectionsDialog;
}

class HelicalServerConnectionsDialog : public QDialog
{
    Q_OBJECT

public:

    // Constructor / Destructor

    explicit HelicalServerConnectionsDialog(QWidget *parent = 0);
    ~HelicalServerConnectionsDialog();

signals:

    // Connect to remote server

    void connectToServer(const QString &connectionName);

private slots:

    // Window controls

    void on_newServerButton_clicked();      // New connection
    void on_editServerButton_clicked();     // Edit connection
    void on_removeServerButton_clicked();   // Remove connection
    void on_connectServerButton_clicked();  // Connect to server

private:

    // Load connection list from settings

    void loadConnectionsList();

    // Polulate connections list in dialog

    void populateConnectionList();

    // Save connections list away to settings

    void saveConnectionList();

    Ui::HelicalServerConnectionsDialog *ui;     // Qt dialog data

    QStringList m_connectionList;               // Connections list

};

#endif // HELICALSERVERCONNECTIONSDIALOG_H
