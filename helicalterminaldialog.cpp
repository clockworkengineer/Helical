#include "helicalterminaldialog.h"
#include "ui_helicalterminaldialog.h"

#include <QDebug>

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

    m_terminalTextArea.reset(new QtTerminal(m_columns, m_rows, ui->terminalText));
    m_textAreaLayout.reset(new QHBoxLayout);
    m_textAreaLayout->addWidget(m_terminalTextArea.data());
    ui->terminalText->setLayout(m_textAreaLayout.data());

    connect(m_terminalTextArea.data(), &QtTerminal::keySend, this, &HelicalTerminalDialog::keyRecv);
    connect(m_connectionChannel.data(), &QtSSHChannel::remoteShellClosed, this, &HelicalTerminalDialog::remoteShellClosed);
    connect(m_connectionChannel.data(), &QtSSHChannel::writeStdOut, m_terminalTextArea.data(), &QtTerminal::terminalOutput);
    connect(m_connectionChannel.data(), &QtSSHChannel::writeStdErr, m_terminalTextArea.data(), &QtTerminal::terminalOutput);

    adjustSize();

}

HelicalTerminalDialog::~HelicalTerminalDialog()
{

    terminateShell();
    delete ui;
}

void HelicalTerminalDialog::keyRecv(const QByteArray &keyAscii)
{
    m_connectionChannel->write(keyAscii);

}

void HelicalTerminalDialog::remoteShellClosed()
{
    close();
}

void HelicalTerminalDialog::closeEvent(QCloseEvent *event)
{

    terminateShell();
    QDialog::closeEvent(event);

}

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

void HelicalTerminalDialog::runCommand(const QString &command)
{

    m_connectionChannel->executeRemoteCommand(command);

}

void HelicalTerminalDialog::runShell()
{

    m_remoteShellThread.reset(new std::thread(&QtSSHChannel::remoteShell,m_connectionChannel.data(), m_columns, m_rows));

}



