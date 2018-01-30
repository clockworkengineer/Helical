#include "helicalconnectiondialog.h"
#include "ui_helicalconnectiondialog.h"

HelicalConnectionDialog::HelicalConnectionDialog(QtSSH &session, QWidget *parent) :
     QDialog(parent),
     ui(new Ui::HelicalConnectionDialog)

{
    ui->setupUi(this);

    m_connectionChannel.reset(new QtSSHChannel(session));
    m_connectionChannel->open();

    setupTerminalTextArea();

}

HelicalConnectionDialog::~HelicalConnectionDialog()
{
    m_connectionChannel->close();
    m_commandThread->join();

    delete ui;
}

void HelicalConnectionDialog::keyRecv(const QByteArray &keyAscii)
{
    m_connectionChannel->write(keyAscii);

}

void HelicalConnectionDialog::remoteShellClosed()
{
    this->close();
}

void HelicalConnectionDialog::setupTerminalTextArea()
{

    m_terminalTextArea = new QtTerminalText(ui->terminalText);

    m_textAreaLayout.reset(new QHBoxLayout);
    m_textAreaLayout->addWidget(m_terminalTextArea);
    ui->terminalText->setLayout(m_textAreaLayout.data());

    connect(m_terminalTextArea, &QtTerminalText::keySend, this, &HelicalConnectionDialog::keyRecv);
    connect(m_connectionChannel.data(), &QtSSHChannel::remoteShellClosed, this, &HelicalConnectionDialog::remoteShellClosed);
    connect(m_connectionChannel.data(), &QtSSHChannel::writeStdOut, m_terminalTextArea, &QtTerminalText::terminalOutput);
    connect(m_connectionChannel.data(), &QtSSHChannel::writeStdErr, m_terminalTextArea, &QtTerminalText::terminalOutput);

    m_terminalTextArea->setFocus();

}

void HelicalConnectionDialog::runCommand(const QString &command)
{

    m_connectionChannel->executeRemoteCommand(command);

}

void HelicalConnectionDialog::runShell(int columns, int rows)
{

    m_commandThread.reset(new std::thread(&QtSSHChannel::remoteShell,m_connectionChannel.data(), columns, rows));

}

