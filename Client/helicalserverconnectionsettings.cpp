#include "helicalserverconnectionsettings.h"

#include <QSettings>

HelicalServerConnectionSettings::HelicalServerConnectionSettings()
{

}

void HelicalServerConnectionSettings::load(const QString &connectionName)
{
    QSettings helicalSettings;
    m_connectionName = connectionName;
    helicalSettings.beginGroup(m_connectionName);
    m_serverName= helicalSettings.value("server").toString();
    m_serverPort = helicalSettings.value("port").toString();
    m_userName = helicalSettings.value("user").toString();
    m_userPassword =  helicalSettings.value("password").toString();
    m_userHome =  helicalSettings.value("home").toString();
    m_command =  helicalSettings.value("command").toString();
    helicalSettings.endGroup();
}

void HelicalServerConnectionSettings::save()
{
    QSettings helicalSettings;
    helicalSettings.beginGroup(m_connectionName);
    helicalSettings.setValue("server",m_serverName);
    helicalSettings.setValue("port",m_serverPort);
    helicalSettings.setValue("user",m_userName);
    helicalSettings.setValue("password",m_userPassword);
    helicalSettings.setValue("home",m_userHome);
    helicalSettings.setValue("command",m_command);
    helicalSettings.endGroup();
}

QString HelicalServerConnectionSettings::connectionName() const
{
    return m_connectionName;
}

void HelicalServerConnectionSettings::setConnectionName(const QString &connectionName)
{
    m_connectionName = connectionName;
}

QString HelicalServerConnectionSettings::serverName() const
{
    return m_serverName;
}

void HelicalServerConnectionSettings::setServerName(const QString &serverName)
{
    m_serverName = serverName;
}

QString HelicalServerConnectionSettings::serverPort() const
{
    return m_serverPort;
}

void HelicalServerConnectionSettings::setServerPort(const QString &serverPort)
{
    m_serverPort = serverPort;
}

QString HelicalServerConnectionSettings::userName() const
{
    return m_userName;
}

void HelicalServerConnectionSettings::setUserName(const QString &userName)
{
    m_userName = userName;
}

QString HelicalServerConnectionSettings::userPassword() const
{
    return m_userPassword;
}

void HelicalServerConnectionSettings::setUserPassword(const QString &userPassword)
{
    m_userPassword = userPassword;
}

QString HelicalServerConnectionSettings::command() const
{
    return m_command;
}

void HelicalServerConnectionSettings::setCommand(const QString &command)
{
    m_command = command;
}

QString HelicalServerConnectionSettings::userHome() const
{
    return m_userHome;
}

void HelicalServerConnectionSettings::setUserHome(const QString &userHome)
{
    m_userHome = userHome;
}
