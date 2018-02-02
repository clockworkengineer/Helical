#include "helicalmainwindow.h"
#include "ui_heilcalmainwindow.h"

HelicalMainWindow::HelicalMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HeilcalMainWindow)

{
    ui->setupUi(this);

    QCoreApplication::setOrganizationName("ClockWorkEngineer");
    QCoreApplication::setApplicationName("Helical");

    terminateSession();

}

HelicalMainWindow::~HelicalMainWindow()
{

    terminateSession();

    delete ui;
}

void HelicalMainWindow::sessionFullyConnected()
{
    qDebug() << "Connected.";

    if (!m_command.isEmpty()){
        ui->executeCommandButton->setEnabled(true);
    }
    ui->terminalButton->setEnabled(true);
    ui->disconnectServerButton->setEnabled(true);

    ui->currentStatusLabel->setText("Connected.");

    ui->serverSessionLog->insertPlainText(m_session->getBanner());

}

void HelicalMainWindow::terminateSession()
{

    if(m_connectionWindow) {
        m_connectionWindow->close();
        m_connectionWindow.reset();
    }

    if (m_session) {
        m_session->disconnectFromServer();
        m_session.reset();
    }

    ui->currentStatusLabel->setText("Disconnected.");
    ui->serverNameLabel->setText("");
    ui->terminalButton->setEnabled(false);
    ui->disconnectServerButton->setEnabled(false);
    ui->executeCommandButton->setEnabled(false);
    ui->serverSessionLog->clear();

}

void HelicalMainWindow::connectToServer(const QString &connectionName)
{

    QSettings helicalSettings;
    helicalSettings.beginGroup(connectionName);
    m_serverName = helicalSettings.value("server").toString();
    m_serverPort = helicalSettings.value("port").toString();
    m_userName = helicalSettings.value("user").toString();
    m_userPassword = helicalSettings.value("password").toString();
    m_command = helicalSettings.value("command").toString();
    helicalSettings.endGroup();

    m_session.reset(new QtSSH);

    connect(m_session.data(), &QtSSH::error, this, &HelicalMainWindow::error);
    connect(m_session.data(), &QtSSH::connectedToServer, this, &HelicalMainWindow::connectedToServer);
    connect(m_session.data(), &QtSSH::serverVerified, this, &HelicalMainWindow::serverVerified);
    connect(m_session.data(), &QtSSH::userAuthorized, this, &HelicalMainWindow::userAuthorized);

    m_session->setSessionDetails(m_serverName,m_serverPort, m_userName, m_userPassword);
    m_session->connectToServer();
    m_serverConnections->hide();
    ui->serverNameLabel->setText(m_serverName);

}

void HelicalMainWindow::error(const QString &errorMessage, int errorCode)
{

    qDebug() << errorMessage << " " << errorCode;

}

void HelicalMainWindow::serverVerified()
{
    QtSSH *session { qobject_cast<QtSSH*>(sender()) };

    qDebug() << "Server verified...";

    ui->currentStatusLabel->setText("Server verified....");
    if (session) {
        session->authorizeUser();
    }

}

void HelicalMainWindow::userAuthorized()
{
    qDebug() << "User authorized ...";
    ui->currentStatusLabel->setText("User authorized ...");
    sessionFullyConnected();
}

void HelicalMainWindow::connectedToServer()
{
    QtSSH *session { qobject_cast<QtSSH*>(sender()) };

    qDebug() << "Connected to server ...";
    ui->currentStatusLabel->setText("Connected to server ...");
    if (session) {
        session->verifyServer();
    }

}

void HelicalMainWindow::commandOutput(const QString &text)
{
    ui->serverSessionLog->insertPlainText(text);
    ui->serverSessionLog->moveCursor (QTextCursor::End);

}

void HelicalMainWindow::on_disconnectServerButton_clicked()
{
    terminateSession();
}

void HelicalMainWindow::on_terminalButton_clicked()
{

    m_connectionWindow.reset(new HelicalTerminalDialog(*m_session.data()));
    m_connectionWindow->runShell(80,24);
    m_connectionWindow->show();

}

void HelicalMainWindow::on_actionConnections_triggered()
{
    if (!m_serverConnections) {
        m_serverConnections.reset(new HelicalServerConnectionsDialog(this));
    }

    m_serverConnections->exec();
}

void HelicalMainWindow::on_executeCommandButton_clicked()
{

    m_connectionChannel.reset(new QtSSHChannel(*m_session));
    if (m_connectionChannel) {
        connect(m_connectionChannel.data(), &QtSSHChannel::writeStdOut, this, &HelicalMainWindow::commandOutput);
        connect(m_connectionChannel.data(), &QtSSHChannel::writeStdErr, this, &HelicalMainWindow::commandOutput);
        m_connectionChannel->open();
        m_connectionChannel->executeRemoteCommand(m_command);
        m_connectionChannel->close();
        m_connectionChannel.reset();
    }
}
