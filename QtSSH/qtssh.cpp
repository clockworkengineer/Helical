/*
 * File:   qtssh.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

//
// Class: QtSSH
//
// Description:
//

// =============
// INCLUDE FILES
// =============

#include "qtssh.h"

QtSSH::QtSSH(QObject *parent) : QObject(parent)
{

}

void QtSSH::setSessionDetails(const QString &serverName, const QString &serverPort, const QString &userName, const QString &userPassword)
{

    // Set session details

    m_session.setServer(serverName.toStdString());
    m_session.setPort(std::stoi(serverPort.toStdString()));
    m_session.setUser(userName.toStdString());
    m_session.setUserPassword(userPassword.toStdString());

}

void QtSSH::connectToServer()
{
    try {
        m_session.connect();
        emit connectedToServer();
    }catch(const CSSHSession::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

void QtSSH::disconnectFromServer()
{
    m_session.disconnect();
    emit disconnectedFromServer();
}

void QtSSH::verifyServer()
{

    try {
        if (verifyKnownServer(m_session)) {
            emit serverVerified();
        }
    }catch(const CSSHSession::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

void QtSSH::authorizeUser()
{

    try {
        if (userAuthorize(m_session)) {
            emit userAuthorized();
        }
    }catch(const CSSHSession::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

QString QtSSH::getBanner()
{

    QString serverBanner;

    try {
        std::string banner = m_session.getBanner();
        serverBanner = serverBanner.fromStdString(banner);
    }catch(const CSSHSession::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

    return(serverBanner);

}

bool QtSSH::isConnected()
{
    return(m_session.isConnected());
}

bool QtSSH::isAuthorized()
{
    return(m_session.isAuthorized());
}

CSSHSession& QtSSH::getSession()
{
    return m_session;
}
