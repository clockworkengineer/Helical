/*
 * File:   helicalterminaldialog.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

//
// Class: HelicalTerminalDialog
//
// Description: Class to create a terminal shell connection dialog to accept all shell
// input and display all output from it. It utilises class cterminal which is a basic
// vt100 termianl emulation that uses a character array as a virtual display (this is
// flushed to the dialog window at decreet points).
//

// =============
// INCLUDE FILES
// =============

#include "helicalterminaldialog.h"
#include "ui_helicalterminaldialog.h"

#include <QFontDatabase>

/**
 * @brief HelicalTerminalDialog::HelicalTerminalDialog
 *
 * Create shell sesion window (dialog).
 *
 * @param session
 * @param columns
 * @param rows
 * @param parent
 */

HelicalTerminalDialog::HelicalTerminalDialog(QtSSH &session, int columns, int rows, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelicalConnectionDialog),
    m_columns {columns},
    m_rows {rows}

{
    ui->setupUi(this);

    setWindowTitle("Terminal");

    m_connectionChannel.reset(new QtSSHChannel(session));
    m_connectionChannel->open();

    m_terminalTextArea.reset(new QtTerminal(m_columns, m_rows, this));

    layout()->addWidget(m_terminalTextArea.data());

    connect(m_terminalTextArea.data(), &QtTerminal::keySend, this, &HelicalTerminalDialog::keyRecv);
    connect(m_connectionChannel.data(), &QtSSHChannel::remoteShellClosed, this, &HelicalTerminalDialog::remoteShellClosed);
    connect(m_connectionChannel.data(), &QtSSHChannel::writeStdOutput, m_terminalTextArea.data(), &QtTerminal::terminalOutput);
    connect(m_connectionChannel.data(), &QtSSHChannel::writeStdError, m_terminalTextArea.data(), &QtTerminal::terminalOutput);

    layout()->setMargin(0);

    adjustSize();

}

/**
 * @brief HelicalTerminalDialog::~HelicalTerminalDialog
 *
 * Destory terminal shell window and terminate its session.
 *
 */

HelicalTerminalDialog::~HelicalTerminalDialog()
{

    terminateShell();
    delete ui;
}

/**
 * @brief HelicalTerminalDialog::keyRecv
 *
 * Slot method for sending local key events to shell channel (remote server).
 *
 * @param keyAscii
 */
void HelicalTerminalDialog::keyRecv(const QByteArray &keyAscii)
{
    m_connectionChannel->write(keyAscii);

}

/**
 * @brief HelicalTerminalDialog::remoteShellClosed
 *
 * Slow method for remote shell closed signal (just close window).
 *
 */
void HelicalTerminalDialog::remoteShellClosed()
{
    close();
}

/**
 * @brief HelicalTerminalDialog::closeEvent
 *
 * Override for window close event. Terminate session and resend event up chain.
 *
 * @param event
 */
void HelicalTerminalDialog::closeEvent(QCloseEvent *event)
{

    terminateShell();
    QDialog::closeEvent(event);

}

/**
 * @brief HelicalTerminalDialog::terminateShell
 *
 * Close shell channel and stop its associated thread.
 *
 */
void HelicalTerminalDialog::terminateShell()
{
    if (m_connectionChannel->isOpen()) {
        m_connectionChannel->close();
    }
    if (m_remoteShellThread) {
        m_remoteShellThread->join();
        m_remoteShellThread.reset();
    }
}

/**
 * @brief HelicalTerminalDialog::runCommand
 *
 * Run command on remote server.
 *
 * @param command
 */
void HelicalTerminalDialog::runCommand(const QString &command)
{

    m_connectionChannel->executeRemoteCommand(command);

}

/**
 * @brief HelicalTerminalDialog::runShell
 *
 * Startup remote shell and its correspding thread.
 *
 */
void HelicalTerminalDialog::runShell()
{

    m_remoteShellThread.reset(new std::thread(&QtSSHChannel::remoteShell,m_connectionChannel.data(), m_columns, m_rows));

}



