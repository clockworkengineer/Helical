#ifndef HEILCALMAINWINDOW_H
#define HEILCALMAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QDebug>
#include <QSettings>

#include "helicalconnectiondialog.h"
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

public slots:
    void error(const QString &errorMessage, int errorCode);
    void serverVerified();
    void userAuthorized();
    void connectedToServer();

private:
    Ui::HeilcalMainWindow *ui;

    QtSSH m_session;
    QString m_serverName;
    QString m_serverPort;
    QString m_userName;
    QString m_userPassword;

    QScopedPointer<HelicalConnectionDialog> m_connectionWindow;

};

#endif // HEILCALMAINWINDOW_H
