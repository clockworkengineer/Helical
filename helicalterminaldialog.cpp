#include "helicalterminaldialog.h"
#include "ui_helicalterminaldialog.h"

HelicalTerminalDialog::HelicalTerminalDialog(QtSSH &session, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelicalConnectionDialog)

{
    ui->setupUi(this);

    setWindowTitle("Terminal");

    m_connectionChannel.reset(new QtSSHChannel(session));
    m_connectionChannel->open();

    setupTerminalTextArea();

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

void HelicalTerminalDialog::setupTerminalTextArea()
{

    m_terminalTextArea = new QtTerminalText(ui->terminalText);

    m_textAreaLayout.reset(new QHBoxLayout);
    m_textAreaLayout->addWidget(m_terminalTextArea);
    ui->terminalText->setLayout(m_textAreaLayout.data());

    connect(m_terminalTextArea, &QtTerminalText::keySend, this, &HelicalTerminalDialog::keyRecv);
    connect(m_connectionChannel.data(), &QtSSHChannel::remoteShellClosed, this, &HelicalTerminalDialog::remoteShellClosed);
    connect(m_connectionChannel.data(), &QtSSHChannel::writeStdOut, m_terminalTextArea, &QtTerminalText::terminalOutput);
    connect(m_connectionChannel.data(), &QtSSHChannel::writeStdErr, m_terminalTextArea, &QtTerminalText::terminalOutput);

    m_terminalTextArea->setFocus();

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

void HelicalTerminalDialog::runShell(int columns, int rows)
{

    m_remoteShellThread.reset(new std::thread(&QtSSHChannel::remoteShell,m_connectionChannel.data(), columns, rows));

}



