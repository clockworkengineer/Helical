/*
 * File:   qtssh.h
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

#ifndef QTSSH_H
#define QTSSH_H

//
// Class: QtSSH
//
// Description: Class for client SSH to server connections. Its uses the Antik::CSSH C++
// wrapper classes for third party library libssh. Its translates to/from Qt to standard
// C++ data structures as and when needed to keep the whole interface Qt orientated.
//

// =============
// INCLUDE FILES
// =============

#include <QObject>

#include "SSHSessionUtil.hpp"

// =================
// CLASS DECLARATION
// =================

using namespace Antik::SSH;

class QtSSH : public QObject
{
    Q_OBJECT

    //
    // Context for server verification feedback
    //

    class QtServerVerificationContext : public ServerVerificationContext {
    public:
        explicit QtServerVerificationContext(void *context=nullptr) : ServerVerificationContext{context} {}

        virtual void serverKnown() final;
        virtual bool serverKnownChanged(std::vector<unsigned char> &keyHash) final;
        virtual bool serverFoundOther() final;
        virtual bool serverFileNotFound(std::vector<unsigned char> &keyHash) final;
        virtual bool serverNotKnown(std::vector<unsigned char> &keyHash) final;
        virtual bool serverError() final;

    };

public:

    // Constructor

    explicit QtSSH(QObject *parent = nullptr);

    // Set session details

    void setSessionDetails(const QString &serverName, const QString &serverPort, const QString &userName, const QString &userPassword);

    // Connect/disconnext from server

    void connectToServer();
    void disconnectFromServer();

    // Verify server/ authorize user

    void verifyServer();
    void authorizeUser();

    // Get server banner

    QString getBanner();

    // Get autorizationtyp finally user.

    quint32 getAuthorizarionType();

    // Check if session connected/authorized

    bool isConnected();
    bool isAuthorized();

    // Return reference to session object

    CSSHSession& getSession();

    QString getServerName() const;
    void setServerName(const QString &serverName);
    QString getServerPort() const;
    void setServerPort(const QString &serverPort);
    QString getUserName() const;
    void setUserName(const QString &userName);
    QString getUserPassword() const;
    void setUserPassword(const QString &userPassword);

signals:

    // Session status

    void error(const QString &errorMessage, int errorCode); // Session error
    void connectedToServer();                               // Session conncted to server
    void serverVerified();                                  // Server has been verified
    void userAuthorized();                                  // User has been authorized
    void disconnectedFromServer();                          // Disconnected from server

    // Server verification feedback

    void serverKnown();
    void serverKnownChanged(std::vector<unsigned char> &keyHash);
    void serverFoundOther();
    void serverFileNotFound(std::vector<unsigned char> &keyHash);
    void serverNotKnown(std::vector<unsigned char> &keyHash);
    void serverError();

public slots:

private:

    // Session object

    CSSHSession m_session;

    QString m_serverName;       // Server name
    QString m_serverPort;       // Server port
    QString m_userName;         // User name
    QString m_userPassword;     // User password

};

#endif // QTSSH_H
