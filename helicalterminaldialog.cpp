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

    setupTerminalTextArea();

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

void HelicalTerminalDialog::setupTerminalTextArea()
{

    m_terminalTextArea = new QtTerminalText(ui->terminalText);
    m_textAreaLayout.reset(new QHBoxLayout);
    m_textAreaLayout->addWidget(m_terminalTextArea);
    ui->terminalText->setLayout(m_textAreaLayout.data());

    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    m_terminalTextArea->setFont(font);
    QFontMetrics fm(m_terminalTextArea->property("font").value<QFont>());
    m_terminalTextArea->setFixedSize(fm.maxWidth()*(m_columns)+2, fm.height()*(m_rows+1)+2);
    m_terminalTextArea->setupTerminalText(m_columns, m_rows);

    connect(m_terminalTextArea, &QtTerminalText::keySend, this, &HelicalTerminalDialog::keyRecv);
    connect(m_connectionChannel.data(), &QtSSHChannel::remoteShellClosed, this, &HelicalTerminalDialog::remoteShellClosed);
    connect(m_connectionChannel.data(), &QtSSHChannel::writeStdOut, m_terminalTextArea, &QtTerminalText::terminalOutput);
    connect(m_connectionChannel.data(), &QtSSHChannel::writeStdErr, m_terminalTextArea, &QtTerminalText::terminalOutput);

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



