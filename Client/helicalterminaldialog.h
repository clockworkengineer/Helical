/*
 * File:   helicalterminaldialog.h
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

#ifndef HELICALCONNECTIONDIALOG_H
#define HELICALCONNECTIONDIALOG_H

//
// Class: HelicalTerminalDialog
//
// Description: Class to create a terminal shell connection dialog to accept all shell
// input and display all output from it. It utilises class cterminal which is a basic
// vt100 terminal emulation that uses a character array as a virtual display (this is
// flushed to the dialog window at decreet points).
//

// =============
// INCLUDE FILES
// =============

#include <QDialog>
#include <QHBoxLayout>

// =================
// CLASS DECLARATION
// =================

#include "QtTerminal/qtterminal.h"
#include "QtSSH/qtsshchannel.h"

namespace Ui {
class HelicalConnectionDialog;
}

class HelicalTerminalDialog : public QDialog
{
    Q_OBJECT

public:

    // Constructor / Destructor

    explicit HelicalTerminalDialog(QtSSH &session, int columns, int rows, QWidget *parent = 0);
    ~HelicalTerminalDialog();

    // Run remote shell command
    void runCommand(const QString &command);

    // Run remote shell in window

    void runShell();

public slots:

    void keyRecv(const QByteArray &keyAscii);   // Key input recieved
    void remoteShellClosed();                   // Remote shell closed.

protected:

    // Override for dialof close event

    void closeEvent(QCloseEvent *event) override;

private:

    // Terminate shell processing

    void terminateShell();

    Ui::HelicalConnectionDialog *ui;    // Qt dialog data

    QScopedPointer<QtTerminal> m_terminalTextArea {nullptr};    // Pointer to terminal text area (screen)
    QScopedPointer<QtSSHChannel> m_connectionChannel {nullptr}; // Pointer to shell channel
    QScopedPointer<QHBoxLayout> m_textAreaLayout {nullptr};     // Pointer to layout for terminal text area.
    QScopedPointer<std::thread> m_remoteShellThread {nullptr};  // Pointer to remote shell thread

    int m_columns {0};  // Shell columns
    int m_rows {0};     // Shell rows

};

#endif // HELICALCONNECTIONDIALOG_H
