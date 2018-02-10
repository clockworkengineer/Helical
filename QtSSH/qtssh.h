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
// Description:
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

    explicit QtSSH(QObject *parent = nullptr);

    void setSessionDetails(const QString &serverName, const QString &serverPort, const QString &userName, const QString &userPassword);

    void connectToServer();
    void disconnectFromServer();
    void verifyServer();
    void authorizeUser();
    QString getBanner();
    quint32 getAuthorizarionType();
    bool isConnected();
    bool isAuthorized();
    CSSHSession& getSession();

signals:

    void error(const QString &errorMessage, int errorCode);
    void connectedToServer();
    void serverVerified();
    void userAuthorized();
    void disconnectedFromServer();

    void serverKnown();
    void serverKnownChanged(std::vector<unsigned char> &keyHash);
    void serverFoundOther();
    void serverFileNotFound(std::vector<unsigned char> &keyHash);
    void serverNotKnown(std::vector<unsigned char> &keyHash);
    void serverError();

public slots:

private:

    CSSHSession m_session;

};

#endif // QTSSH_H
