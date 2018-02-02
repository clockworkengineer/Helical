#ifndef QTSSH_H
#define QTSSH_H

#include <QObject>
#include "SSHSessionUtil.hpp"

using namespace Antik::SSH;

class QtSSH : public QObject
{
    Q_OBJECT

    class SSHSession : public CSSHSession
    {
//        virtual int userAuthorizationWithPassword();
//        virtual int userAuthorizationWithPublicKey();
//        virtual int userAuthorizationWithKeyboardInteractive();
    };

public:

    explicit QtSSH(QObject *parent = nullptr);

    void setSessionDetails(const QString &serverName, const QString &serverPort, const QString &userName, const QString &userPassword);

    void connectToServer();
    void disconnectFromServer();
    void verifyServer();
    void authorizeUser();
    QString getBanner();
    bool isConnected();
    bool isAuthorized();

signals:
    void error(const QString &errorMessage, int errorCode);
    void connectedToServer();
    void serverVerified();
    void userAuthorized();
    void disconnectedFromServer();

public slots:

public:

    CSSHSession& getSession();

private:
    SSHSession m_session;

};

#endif // QTSSH_H
