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

//
// Overidden Qt server verification methods.
//

/**
 * @brief QtSSH::QtServerVerificationContext::serverKnown
 */
void QtSSH::QtServerVerificationContext::serverKnown()
{
    if (m_contextData) {
        QtSSH *sshSession = static_cast<QtSSH *> (m_contextData);
        emit sshSession->serverKnown();
    }
}

/**
 * @brief QtSSH::QtServerVerificationContext::serverKnownChanged
 * @param keyHash
 * @return
 */
bool QtSSH::QtServerVerificationContext::serverKnownChanged(std::vector<unsigned char> &keyHash)
{
    if (m_contextData) {
        QtSSH *sshSession = static_cast<QtSSH *> (m_contextData);
        emit sshSession->serverKnownChanged(keyHash);
    }
    return (false);
}

/**
 * @brief QtSSH::QtServerVerificationContext::serverFoundOther
 * @return
 */
bool QtSSH::QtServerVerificationContext::serverFoundOther()
{
    if (m_contextData) {
        QtSSH *sshSession = static_cast<QtSSH *> (m_contextData);
        emit sshSession->serverFoundOther();
    }
    return (false);
}

/**
 * @brief QtSSH::QtServerVerificationContext::serverFileNotFound
 * @param keyHash
 * @return
 */
bool QtSSH::QtServerVerificationContext::serverFileNotFound(std::vector<unsigned char> &keyHash)
{
    if (m_contextData) {
        QtSSH *sshSession = static_cast<QtSSH *> (m_contextData);
        emit sshSession->serverFileNotFound(keyHash);
    }
    return(false);
}

/**
 * @brief QtSSH::QtServerVerificationContext::serverNotKnown
 * @param keyHash
 * @return
 */
bool QtSSH::QtServerVerificationContext::serverNotKnown(std::vector<unsigned char> &keyHash)
{
    if (m_contextData) {
        QtSSH *sshSession = static_cast<QtSSH *> (m_contextData);
        emit sshSession->serverNotKnown(keyHash);
    }
    return(false);
}

/**
 * @brief QtSSH::QtServerVerificationContext::serverError
 * @return
 */
bool QtSSH::QtServerVerificationContext::serverError()
{
    if (m_contextData) {
        QtSSH *sshSession = static_cast<QtSSH *> (m_contextData);
        emit sshSession->serverError();
    }
    return(false);
}

/**
 * @brief QtSSH::QtSSH
 * @param parent
 */
QtSSH::QtSSH(QObject *parent) : QObject(parent)
{

}

/**
 * @brief QtSSH::setSessionDetails
 * @param serverName
 * @param serverPort
 * @param userName
 * @param userPassword
 */
void QtSSH::setSessionDetails(const QString &serverName, const QString &serverPort, const QString &userName, const QString &userPassword)
{

    // Set session details

    m_session.setServer(serverName.toStdString());
    m_session.setPort(std::stoi(serverPort.toStdString()));
    m_session.setUser(userName.toStdString());
    m_session.setUserPassword(userPassword.toStdString());

}

/**
 * @brief QtSSH::connectToServer
 */
void QtSSH::connectToServer()
{
    try {
        m_session.connect();
        emit connectedToServer();
    }catch(const CSSHSession::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

/**
 * @brief QtSSH::disconnectFromServer
 */
void QtSSH::disconnectFromServer()
{
    m_session.disconnect();
    emit disconnectedFromServer();
}

void QtSSH::verifyServer()
{
    QtServerVerificationContext verificationContext {this };

    try {
        if (verifyKnownServer(m_session, verificationContext)) {
            emit serverVerified();
        }
    }catch(const CSSHSession::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

/**
 * @brief QtSSH::authorizeUser
 */
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

/**
 * @brief QtSSH::getBanner
 * @return
 */
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

quint32 QtSSH::getAuthorizarionType()
{
    return(m_session.getAuthorizarionType());
}

/**
 * @brief QtSSH::isConnected
 * @return
 */
bool QtSSH::isConnected()
{
    return(m_session.isConnected());
}

/**
 * @brief QtSSH::isAuthorized
 * @return
 */
bool QtSSH::isAuthorized()
{
    return(m_session.isAuthorized());
}

/**
 * @brief QtSSH::getSession
 * @return
 */
CSSHSession& QtSSH::getSession()
{
    return m_session;
}
