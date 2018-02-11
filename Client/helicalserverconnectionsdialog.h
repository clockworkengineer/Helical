/*
 * File:   helicalserverconnectionsdialog.h
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

#ifndef HELICALSERVERCONNECTIONSDIALOG_H
#define HELICALSERVERCONNECTIONSDIALOG_H

//
// Class: HelicalServerConnectionsDialog
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
class HelicalServerConnectionsDialog;
}

class HelicalServerConnectionsDialog : public QDialog
{
    Q_OBJECT

public:

    explicit HelicalServerConnectionsDialog(QWidget *parent = 0);
    ~HelicalServerConnectionsDialog();

signals:

    void connectToServer(const QString &connectionName);

private slots:

    void on_newServerButton_clicked();
    void on_editServerButton_clicked();
    void on_removeServerButton_clicked();
    void on_connectServerButton_clicked();

private:

    void loadConnectionsList();
    void populateConnectionList();
    void saveConnectionList();

    Ui::HelicalServerConnectionsDialog *ui;

    QStringList m_connectionList;

};

#endif // HELICALSERVERCONNECTIONSDIALOG_H
