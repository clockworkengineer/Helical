/*
 * File:   helicalmainwindow.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

//
// Class: HelicalMainWindow
//
// Description:
//

// =============
// INCLUDE FILES
// =============

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

    if (m_session) {

        connect(m_session.data(), &QtSSH::error, this, &HelicalMainWindow::error);
        connect(m_session.data(), &QtSSH::connectedToServer, this, &HelicalMainWindow::connectedToServer);
        connect(m_session.data(), &QtSSH::serverVerified, this, &HelicalMainWindow::serverVerified);
        connect(m_session.data(), &QtSSH::userAuthorized, this, &HelicalMainWindow::userAuthorized);

        connect(m_session.data(), &QtSSH::serverKnownChanged, this, &HelicalMainWindow::serverKnownChanged);
        connect(m_session.data(), &QtSSH::serverFoundOther, this, &HelicalMainWindow::serverFoundOther);
        connect(m_session.data(), &QtSSH::serverFileNotFound, this, &HelicalMainWindow::serverFileNotFound);
        connect(m_session.data(), &QtSSH::serverNotKnown, this, &HelicalMainWindow::serverNotKnown);
        connect(m_session.data(), &QtSSH::serverError, this, &HelicalMainWindow::serverError);

        m_serverConnections->hide();
        ui->statusBar->showMessage("Connecting to server ...");

        m_session->setSessionDetails(m_serverName,m_serverPort, m_userName, m_userPassword);
        m_session->connectToServer();
        if (m_session->isConnected() && m_session->isAuthorized()) {
            ui->serverNameLabel->setText(m_serverName);
        }

    }

}

void HelicalMainWindow::error(const QString &errorMessage, int errorCode)
{

    Q_UNUSED(errorCode);

    ui->statusBar->showMessage(errorMessage);

}

void HelicalMainWindow::serverVerified()
{
    QtSSH *session { qobject_cast<QtSSH*>(sender()) };

    ui->currentStatusLabel->setText("Server verified....");
    if (session) {
        session->authorizeUser();
    }

}

void HelicalMainWindow::userAuthorized()
{
    ui->currentStatusLabel->setText("User authorized ...");
    sessionFullyConnected();
}

void HelicalMainWindow::connectedToServer()
{
    QtSSH *session { qobject_cast<QtSSH*>(sender()) };

    ui->statusBar->clearMessage();
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

void HelicalMainWindow::serverKnownChanged(std::vector<unsigned char> &keyHash)
{

    QString message { "Host key for server changed: it is now:"+
                      QString::fromStdString(this->m_session->getSession().convertKeyHashToHex(keyHash))+
                              " For security reasons, connection will be stopped."};

    QMessageBox::warning(this, "Helical", message);

    ui->currentStatusLabel->clear();

}

void HelicalMainWindow::serverFoundOther()
{
    QString error { "The host key for this server was not found but an other type of key exists."
                    "An attacker might change the default server key to confuse your client into "
                    "thinking the key does not exist" };

    QMessageBox::warning(this, "Helical", error);

    ui->currentStatusLabel->clear();
}

void HelicalMainWindow::serverFileNotFound(std::vector<unsigned char> &keyHash)
{
    QString error { "Could not find known host file.\n"
                    "If you accept the host key here, the file will be automatically created."};

    QMessageBox::information(this, "Helical", error);

    serverNotKnown(keyHash);

}

void HelicalMainWindow::serverNotKnown(std::vector<unsigned char> &keyHash)
{

    QString message { "The server is unknown. Do you trust the host key? Public key hash: "+
                      QString::fromStdString(m_session->getSession().convertKeyHashToHex(keyHash))};

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Test", message, QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        m_session->getSession().writeKnownHost();
        emit serverVerified();
    } else {
        ui->currentStatusLabel->clear();
    }

}

void HelicalMainWindow::serverError()
{

}

void HelicalMainWindow::on_disconnectServerButton_clicked()
{
    terminateSession();
}

void HelicalMainWindow::on_terminalButton_clicked()
{

    m_connectionWindow.reset(new HelicalTerminalDialog(*m_session.data(), 80, 24));
    m_connectionWindow->runShell();
    m_connectionWindow->show();

}

void HelicalMainWindow::on_actionConnections_triggered()
{
    if (!m_serverConnections) {
        m_serverConnections.reset(new HelicalServerConnectionsDialog(this));
    }
    ui->statusBar->clearMessage();
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
