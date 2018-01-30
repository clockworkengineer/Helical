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

CSSHSession& QtSSH::getSession()
{
    return m_session;
}
