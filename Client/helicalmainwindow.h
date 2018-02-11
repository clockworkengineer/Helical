/*
 * File:   helicalmainwindow.h
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

#ifndef HEILCALMAINWINDOW_H
#define HEILCALMAINWINDOW_H

//
// Class: HelicalMainWindow
//
// Description: Class to display the main client window and to initiate
// any SSH sessions.
//

// =============
// INCLUDE FILES
// =============

#include <QMainWindow>
#include <QKeyEvent>
#include <QDebug>
#include <QSettings>
#include <QMessageBox>

#include "helicalserverconnectionsdialog.h"
#include "helicalterminaldialog.h"
#include "QtSSH/qtssh.h"

// =================
// CLASS DECLARATION
// =================

namespace Ui {
class HeilcalMainWindow;
}

class HelicalMainWindow : public QMainWindow
{
    Q_OBJECT

public:

    // Constructor / Destructor

    explicit HelicalMainWindow(QWidget *parent = 0);
    ~HelicalMainWindow();

public slots:

    // General SSH session

    void connectToServer(const QString &connectionName);        // Connect to server
    void error(const QString &errorMessage, int errorCode);     // SSH error message
    void serverVerified();                                      // Server has been verified
    void userAuthorized();                                      // User has been authurized
    void connectedToServer();                                   // Conneced to server
    void commandOutput(const QString &text);                    // Display command output (stdout/stderr)

    // Verify server feedback

    void serverKnownChanged(std::vector<unsigned char> &keyHash);
    void serverFoundOther();
    void serverFileNotFound(std::vector<unsigned char> &keyHash);
    void serverNotKnown(std::vector<unsigned char> &keyHash);
    void serverError();

private slots:

    // Window controls

    void on_disconnectServerButton_clicked();   // Disconnect SSH session
    void on_terminalButton_clicked();           // Execute remote shell
    void on_actionConnections_triggered();      // Connect to remote server
    void on_executeCommandButton_clicked();     // Execute remote command

private:

    // Session connected processing

    void sessionFullyConnected();

    // Terminate session processing

    void terminateSession();

    Ui::HeilcalMainWindow *ui;  // Qt window data

    QScopedPointer<QtSSH> m_session {nullptr};                      // Pointer to session
    QScopedPointer<QtSSHChannel> m_connectionChannel {nullptr};     // Pointer to channel

    QString m_connectionName;   // SSH connection name
    QString m_serverName;       // Server name
    QString m_serverPort;       // Server port
    QString m_userName;         // User name
    QString m_userPassword;     // User password
    QString m_command;          // Remote command to execute on demand.

    QScopedPointer<HelicalServerConnectionsDialog> m_serverConnections;     // Pointer to server connection details dialog
    QScopedPointer<HelicalTerminalDialog> m_connectionWindow;               // Pointer to shell connection window

};

#endif // HEILCALMAINWINDOW_H
