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
// Class: HeilcalMainWindow
//
// Description:
//

// =============
// INCLUDE FILES
// =============

#include <QMainWindow>
#include <QKeyEvent>
#include <QDebug>
#include <QSettings>

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

    explicit HelicalMainWindow(QWidget *parent = 0);
    ~HelicalMainWindow();

public slots:

    void connectToServer(const QString &connectionName);
    void error(const QString &errorMessage, int errorCode);
    void serverVerified();
    void userAuthorized();
    void connectedToServer();
    void commandOutput(const QString &text);

private slots:

    void on_disconnectServerButton_clicked();
    void on_terminalButton_clicked();
    void on_actionConnections_triggered();
    void on_executeCommandButton_clicked();

private:

    void sessionFullyConnected();
    void terminateSession();

    Ui::HeilcalMainWindow *ui;

    QScopedPointer<QtSSH> m_session {nullptr};
    QScopedPointer<QtSSHChannel> m_connectionChannel {nullptr};

    QString m_connectionName;
    QString m_serverName;
    QString m_serverPort;
    QString m_userName;
    QString m_userPassword;
    QString m_command;

    QScopedPointer<HelicalServerConnectionsDialog> m_serverConnections;
    QScopedPointer<HelicalTerminalDialog> m_connectionWindow;

};

#endif // HEILCALMAINWINDOW_H
