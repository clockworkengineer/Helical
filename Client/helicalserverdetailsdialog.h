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

#ifndef HELICALSERVERDETAILSDIALOG_H
#define HELICALSERVERDETAILSDIALOG_H

//
// Class: HelicalServerDetailsDialog
//
// Description: Class to display/edit server connection details dialog.
//

// =============
// INCLUDE FILES
// =============

#include <QDialog>
#include <QSettings>

// =================
// CLASS DECLARATION
// =================

namespace Ui {
class HelicalServerDetailsDialog;
}

class HelicalServerDetailsDialog : public QDialog
{
    Q_OBJECT

public:

    // Constructor / Destructor

    explicit HelicalServerDetailsDialog(const QString& connectionName, QWidget *parent = 0);
    ~HelicalServerDetailsDialog();

    // Get connection name

    QString connectionName() const;

protected:

    // Override shot event

    void showEvent( QShowEvent* event ) override;

private slots:

    // Dialog controls

    void on_cancelButton_clicked();     // Cancel
    void on_saveButton_clicked();       // Save connection details

private:

    Ui::HelicalServerDetailsDialog *ui;     // Qt dialog data

    QString m_connectionName;   // SSH connection name
    QString m_serverName;       // Server name
    QString m_serverPort;       // Server port
    QString m_userName;         // User name
    QString m_userPassword;     // User password
    QString m_command;          // Remote command to execute on demand.

};

#endif // HELICALSERVERDETAILSDIALOG_H
