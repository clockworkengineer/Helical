/*
 * File:   helicalserverconnectionsettings.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

//
// Class: HelicalServerConnectionSettings
//
// Description: Class to load/save Helical SSH server connection details.
//

// =============
// INCLUDE FILES
// =============

#include "helicalserverconnectionsettings.h"

/**
 * @brief HelicalServerConnectionSettings::HelicalServerConnectionSettings
 */
HelicalServerConnectionSettings::HelicalServerConnectionSettings()
{

}

/**
 * @brief HelicalServerConnectionSettings::load
 * @param connectionName
 */
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

/**
 * @brief HelicalServerConnectionSettings::save
 */
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

/**
 * @brief HelicalServerConnectionSettings::connectionName
 * @return
 */
QString HelicalServerConnectionSettings::connectionName() const
{
    return m_connectionName;
}

/**
 * @brief HelicalServerConnectionSettings::setConnectionName
 * @param connectionName
 */
void HelicalServerConnectionSettings::setConnectionName(const QString &connectionName)
{
    m_connectionName = connectionName;
}

/**
 * @brief HelicalServerConnectionSettings::serverName
 * @return
 */
QString HelicalServerConnectionSettings::serverName() const
{
    return m_serverName;
}

/**
 * @brief HelicalServerConnectionSettings::setServerName
 * @param serverName
 */
void HelicalServerConnectionSettings::setServerName(const QString &serverName)
{
    m_serverName = serverName;
}

/**
 * @brief HelicalServerConnectionSettings::serverPort
 * @return
 */
QString HelicalServerConnectionSettings::serverPort() const
{
    return m_serverPort;
}

/**
 * @brief HelicalServerConnectionSettings::setServerPort
 * @param serverPort
 */
void HelicalServerConnectionSettings::setServerPort(const QString &serverPort)
{
    m_serverPort = serverPort;
}

/**
 * @brief HelicalServerConnectionSettings::userName
 * @return
 */
QString HelicalServerConnectionSettings::userName() const
{
    return m_userName;
}

/**
 * @brief HelicalServerConnectionSettings::setUserName
 * @param userName
 */
void HelicalServerConnectionSettings::setUserName(const QString &userName)
{
    m_userName = userName;
}

/**
 * @brief HelicalServerConnectionSettings::userPassword
 * @return
 */
QString HelicalServerConnectionSettings::userPassword() const
{
    return m_userPassword;
}

/**
 * @brief HelicalServerConnectionSettings::setUserPassword
 * @param userPassword
 */
void HelicalServerConnectionSettings::setUserPassword(const QString &userPassword)
{
    m_userPassword = userPassword;
}

/**
 * @brief HelicalServerConnectionSettings::command
 * @return
 */
QString HelicalServerConnectionSettings::command() const
{
    return m_command;
}

/**
 * @brief HelicalServerConnectionSettings::setCommand
 * @param command
 */
void HelicalServerConnectionSettings::setCommand(const QString &command)
{
    m_command = command;
}

/**
 * @brief HelicalServerConnectionSettings::userHome
 * @return
 */
QString HelicalServerConnectionSettings::userHome() const
{
    return m_userHome;
}

/**
 * @brief HelicalServerConnectionSettings::setUserHome
 * @param userHome
 */
void HelicalServerConnectionSettings::setUserHome(const QString &userHome)
{
    m_userHome = userHome;
}
