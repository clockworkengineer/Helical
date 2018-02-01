#ifndef HEILCALMAINWINDOW_H
#define HEILCALMAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QDebug>
#include <QSettings>

#include "helicalserverconnectionsdialog.h"
#include "helicalterminaldialog.h"
#include "QtSSH/qtssh.h"

namespace Ui {
class HeilcalMainWindow;
}

class HelicalMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HelicalMainWindow(QWidget *parent = 0);
    ~HelicalMainWindow();

    void sessionFullyConnected();
    void terminateSession();

public slots:
    void connectToServer(const QString &connectionName);
    void error(const QString &errorMessage, int errorCode);
    void serverVerified();
    void userAuthorized();
    void connectedToServer();

private slots:
    void on_actionEdit_triggered();

    void on_disconnectServerButton_clicked();

    void on_terminalButton_clicked();

private:
    Ui::HeilcalMainWindow *ui;

    QScopedPointer<QtSSH> m_session;

    QString m_connectionName;
    QString m_serverName;
    QString m_serverPort;
    QString m_userName;
    QString m_userPassword;

    QScopedPointer<HelicalServerConnectionsDialog> m_serverConnections;
    QScopedPointer<HelicalTerminalDialog> m_connectionWindow;

};

#endif // HEILCALMAINWINDOW_H
