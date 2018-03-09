/*
 * File:   helicalserverdetailsdialog.h
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

#ifndef HELICALSERVERCONNECTIONSETTINGS_H
#define HELICALSERVERCONNECTIONSETTINGS_H

//
// Class: HelicalServerConnectionSettings
//
// Description: Class to load/save Helical SSH server connection details.
//

// =============
// INCLUDE FILES
// =============

#include <QString>
#include <QSettings>

class HelicalServerConnectionSettings
{
public:

    // Class exception

    struct Exception : public std::runtime_error {

        Exception(const QString & messageStr)
            : std::runtime_error(static_cast<QString>("HelicalServerConnectionSettings Failure: " + messageStr).toStdString()) {
        }

    };

    // Load/Save settings

    void load(const QString& connectionName);
    void save();

    // Settings getter/setters

    QString connectionName() const;
    void setConnectionName(const QString &connectionName);
    QString serverName() const;
    void setServerName(const QString &serverName);
    QString serverPort() const;
    void setServerPort(const QString &serverPort);
    QString userName() const;
    void setUserName(const QString &userName);
    QString userPassword() const;
    void setUserPassword(const QString &userPassword);
    QString command() const;
    void setCommand(const QString &command);
    QString userHome() const;
    void setUserHome(const QString &userHome);

private:
    QString m_connectionName;   // SSH connection name
    QString m_serverName;       // Server name
    QString m_serverPort;       // Server port
    QString m_userName;         // User name
    QString m_userPassword;     // User password
    QString m_userHome;         // User Home
    QString m_command;          // Remote command to execute on demand.
};

#endif // HELICALSERVERCONNECTIONSETTINGS_H
