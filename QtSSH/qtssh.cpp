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
// Description: Class for client SSH to server connections. Its uses the Antik::CSSH C++
// wrapper classes for third party library libssh. Its translates to/from Qt to standard
// C++ data structures as and when needed to keep the whole interface Qt orientated.
//

// =============
// INCLUDE FILES
// =============

#include "qtssh.h"

//
// Overidden Qt SSH server verification methods.
//

/**
 * @brief QtSSH::QtServerVerificationContext::serverKnown
 *
 * Server known.
 *
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
 *
 * Server known but its key has changed.
 *
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
 *
 * Key not found for server but others exist.
 *
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
 *
 * Key not found for server in known hosts file.
 *
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
 *
 * Server not known.
 *
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
 *
 * Verification error.
 *
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
 *
 * Create SSH session.
 *
 * @param parent
 */
QtSSH::QtSSH(QObject *parent) : QObject(parent)
{

}

/**
 * @brief QtSSH::setSessionDetails
 *
 * Set SSH session details.
 *
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
 *
 * Connect to SSH server (open session).
 *
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
 *
 * Disconnect SSH session.
 *
 */
void QtSSH::disconnectFromServer()
{
    m_session.disconnect();
    emit disconnectedFromServer();
}


/**
 * @brief QtSSH::verifyServer
 *
 * Verify SSH known  server.
 *
 */
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
 *
 * Perform user authorization.
 *
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
 *
 * Retrieve server banner.
 *
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

/**
 * @brief QtSSH::getAuthorizarionType
 *
 * Return user authorization type used for session.
 *
 * @return
 */
quint32 QtSSH::getAuthorizarionType()
{
    return(m_session.getAuthorizarionType());
}

/**
 * @brief QtSSH::isConnected
 *
 * @return == true of session is connected.
 */
bool QtSSH::isConnected()
{
    return(m_session.isConnected());
}

/**
 * @brief QtSSH::isAuthorized
 *
 * @return == true if session has been authorized.
 */
bool QtSSH::isAuthorized()
{
    return(m_session.isAuthorized());
}

/**
 * @brief QtSSH::getSession
 * @return Reference to internal session object.
 */
CSSHSession& QtSSH::getSession()
{
    return m_session;
}
