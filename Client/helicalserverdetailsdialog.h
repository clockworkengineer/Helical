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
// Description:
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

    explicit HelicalServerDetailsDialog(const QString& connectionName, QWidget *parent = 0);
    ~HelicalServerDetailsDialog();

    QString connectionName() const;

protected:

    void showEvent( QShowEvent* event );

private slots:

    void on_cancelButton_clicked();
    void on_saveButton_clicked();

private:

    Ui::HelicalServerDetailsDialog *ui;

    QString m_connectionName;
    QString m_serverName;
    QString m_serverPort;
    QString m_userName;
    QString m_userPassword;
    QString m_command;

};

#endif // HELICALSERVERDETAILSDIALOG_H
