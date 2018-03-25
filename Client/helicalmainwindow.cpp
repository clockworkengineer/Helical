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
// Description: Class to display the main client window and to initiate
// any SSH sessions.
//

// =============
// INCLUDE FILES
// =============

#include "helicalmainwindow.h"
#include "ui_heilcalmainwindow.h"

/**
 * @brief HelicalMainWindow::HelicalMainWindow
 *
 * Create and display main client window.
 *
 * @param parent
 */
HelicalMainWindow::HelicalMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HeilcalMainWindow)

{

    // Setup Qt Designed UI

    ui->setupUi(this);

    // Set Company/Application name

    QCoreApplication::setOrganizationName("ClockWorkEngineer");
    QCoreApplication::setApplicationName("Helical");

    // Window to initial disconnected state.

    setWindowTitle(QCoreApplication::applicationName());

    ui->serverSessionLog->setReadOnly(true);

    terminateSession();

}

/**
 * @brief HelicalMainWindow::~HelicalMainWindow
 *
 * Closedown main window (including closing any open session).
 *
 */
HelicalMainWindow::~HelicalMainWindow()
{

    terminateSession();

    delete ui;

}

/**
 * @brief HelicalMainWindow::sessionFullyConnected
 *
 * Session fully connected update main window status.
 * s
 */
void HelicalMainWindow::sessionFullyConnected()
{

    if (!m_serverConnectionSettings.command().isEmpty()){
        ui->executeCommandButton->setEnabled(true);
    }

    ui->terminalButton->setEnabled(true);
    ui->disconnectServerButton->setEnabled(true);
    ui->sftpButton->setEnabled(true);


    ui->currentStatusLabel->setText("Connected.");
    ui->userNameLabel->setText(m_serverConnectionSettings.userName());

    quint32 authorizationType=m_session->getAuthorizarionType();
    switch (authorizationType) {
    case UserAuthorizationType::None:
        ui->authorizationTypeLabel->setText("None");
        break;
    case UserAuthorizationType::Password:
        ui->authorizationTypeLabel->setText("Password");
        break;
    case UserAuthorizationType::PublicKey:
        ui->authorizationTypeLabel->setText("Public Key");
        break;
    case UserAuthorizationType::Interactice:
        ui->authorizationTypeLabel->setText("Interactive");
        break;
    }

    ui->serverSessionLog->insertPlainText(m_session->getBanner());

    setUserHome();

}

/**
 * @brief HelicalMainWindow::terminateSession
 *
 * Closedown any SSH session and update main window status accordingly.
 *
 */
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
    ui->serverNameLabel->clear();
    ui->userNameLabel->clear();
    ui->authorizationTypeLabel->clear();
    ui->terminalButton->setEnabled(false);
    ui->disconnectServerButton->setEnabled(false);
    ui->sftpButton->setEnabled(false);
    ui->executeCommandButton->setEnabled(false);
    ui->sftpButton->setEnabled(false);
    ui->serverSessionLog->clear();

    ui->statusBar->setStyleSheet("QStatusBar {color: default}");

}

/**
 * @brief HelicalMainWindow::connectToServer
 *
 * Get connection name parameters and initiate SSH connection.
 *
 * @param connectionName
 */
void HelicalMainWindow::connectToServer(const QString &connectionName)
{

    m_serverConnectionSettings.load(connectionName);

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

        m_session->setSessionDetails(m_serverConnectionSettings.serverName(),m_serverConnectionSettings.serverPort(),
                                     m_serverConnectionSettings.userName(), m_serverConnectionSettings.userPassword());
        m_session->connectToServer();
        if (m_session->isConnected() && m_session->isAuthorized()) {
            ui->serverNameLabel->setText(m_serverConnectionSettings.serverName());
        }

    }

}

/**
 * @brief HelicalMainWindow::error
 *
 * Display SSH error messages in main window status bar.
 *
 * @param errorMessage
 * @param errorCode
 */
void HelicalMainWindow::error(const QString &errorMessage, int errorCode)
{

    Q_UNUSED(errorCode);

    ui->statusBar->setStyleSheet("QStatusBar {color: red}");
    ui->statusBar->showMessage(errorMessage);

}

/**
 * @brief HelicalMainWindow::serverVerified
 *
 * Server has been verified slot method. Authorize user.
 *
 */
void HelicalMainWindow::serverVerified()
{
    QtSSH *session { qobject_cast<QtSSH*>(sender()) };

    ui->currentStatusLabel->setText("Server verified....");
    if (session) {
        session->authorizeUser();
    }

}

/**
 * @brief HelicalMainWindow::userAuthorized
 *
 * User athorized slot method. Update session status to connected.
 *
 */

void HelicalMainWindow::userAuthorized()
{
    ui->currentStatusLabel->setText("User authorized ...");
    sessionFullyConnected();
}

/**
 * @brief HelicalMainWindow::connectedToServer
 *
 * Connected to server slot method. Now verify server.
 */
void HelicalMainWindow::connectedToServer()
{
    QtSSH *session { qobject_cast<QtSSH*>(sender()) };

    ui->statusBar->clearMessage();
    ui->currentStatusLabel->setText("Connected to server ...");
    if (session) {
        session->verifyServer();
    }

}

/**
 * @brief HelicalMainWindow::commandOutput
 *
 * Single command output slot method (write to main window session logging).
 * @param text
 */
void HelicalMainWindow::commandOutput(const QString &text)
{
    ui->serverSessionLog->insertPlainText(text);
    ui->serverSessionLog->moveCursor (QTextCursor::End);

}

/**
 * @brief HelicalMainWindow::saveCommandOutput
 *
 * Save command output to local buffer for processing.
 *
 * @param text
 */
void HelicalMainWindow::saveCommandOutput(const QString &text)
{

   m_savedCommandOutput.push_back(text);

}

/**
 * @brief HelicalMainWindow::setUserHome
 *
 * Use channel execute command to get PWD value. Set to empty if there is an error.
 *
 */
void HelicalMainWindow::setUserHome()
{
    if (m_serverConnectionSettings.userHome().isEmpty()) {
        m_connectionChannel.reset(new QtSSHChannel(*m_session));
        if (m_connectionChannel) {
            connect(m_connectionChannel.data(), &QtSSHChannel::writeStdOutput, this, &HelicalMainWindow::saveCommandOutput);
            m_connectionChannel->open();
            m_connectionChannel->executeRemoteCommand("pwd");
            m_connectionChannel->close();
            m_connectionChannel.reset();
            if (!m_savedCommandOutput.isEmpty()) {
                m_userHome = m_savedCommandOutput[0];
                m_userHome.chop(1);
                m_savedCommandOutput.clear();
            }
        }
    } else {
        m_userHome = m_serverConnectionSettings.userHome();
    }

}

/**
 * @brief HelicalMainWindow::on_disconnectServerButton_clicked
 *
 * Disconnect session button.
 *
 */
void HelicalMainWindow::on_disconnectServerButton_clicked()
{
    terminateSession();
}

/**
 * @brief HelicalMainWindow::on_terminalButton_clicked
 *
 * Run remote shell in window (currently hardencoded size).
 *
 */
void HelicalMainWindow::on_terminalButton_clicked()
{

    int columns {80};
    int rows {24};

    switch(m_serverConnectionSettings.terminalSize()) {

    case HelicalServerConnectionSettings::term80x24:
        columns=80;
        rows=24;
        break;
    case HelicalServerConnectionSettings::term80x43:
        columns=80;
        rows=43;
        break;
    case HelicalServerConnectionSettings::term132x24:
        columns=132;
        rows=24;
        break;
    case HelicalServerConnectionSettings::term132x43:
        columns=132;
        rows=43;
        break;

    }

    m_connectionWindow.reset(new HelicalTerminalDialog(*m_session.data(), columns, rows));
    m_connectionWindow->runShell();
    m_connectionWindow->show();

}

/**
 * @brief HelicalMainWindow::on_actionConnections_triggered
 *
 * Display connection settings creation/editor dialog.
 *
 */
void HelicalMainWindow::on_actionConnections_triggered()
{
    if (!m_serverConnections) {
        m_serverConnections.reset(new HelicalServerConnectionsDialog(this));
    }
    ui->statusBar->setStyleSheet("QStatusBar {color: default}");
    ui->statusBar->clearMessage();
    m_serverConnections->exec();
}

/**
 * @brief HelicalMainWindow::on_executeCommandButton_clicked
 *
 * Excute remote shell command (output sent to session logging).
 *
 */
void HelicalMainWindow::on_executeCommandButton_clicked()
{

    m_connectionChannel.reset(new QtSSHChannel(*m_session));
    if (m_connectionChannel) {
        connect(m_connectionChannel.data(), &QtSSHChannel::writeStdOutput, this, &HelicalMainWindow::commandOutput);
        connect(m_connectionChannel.data(), &QtSSHChannel::writeStdError, this, &HelicalMainWindow::commandOutput);
        m_connectionChannel->open();
        m_connectionChannel->executeRemoteCommand(m_serverConnectionSettings.command());
        m_connectionChannel->close();
        m_connectionChannel.reset();
    }

}

void HelicalMainWindow::on_sftpButton_clicked()
{
    if (!m_sftpWindow) {
        m_sftpWindow.reset(new HelicalSFTPDialog(*m_session, m_userHome, QDir::homePath(), this));
    }
    ui->statusBar->setStyleSheet("QStatusBar {color: default}");
    ui->statusBar->clearMessage();
    m_sftpWindow->exec();
    m_sftpWindow.reset();

}


// ===================================
// SERVER VERFICATION FEEDBACK METHODS
// ===================================

/**
 * @brief HelicalMainWindow::serverKnownChanged
 * @param keyHash
 */
void HelicalMainWindow::serverKnownChanged(std::vector<unsigned char> &keyHash)
{

    QString message { "Host key for server changed: it is now:"+
                      QString::fromStdString(this->m_session->getSession().convertKeyHashToHex(keyHash))+
                              " For security reasons, connection will be stopped."};

    QMessageBox::warning(this, QCoreApplication::applicationName(), message);

    ui->currentStatusLabel->setText("Disconnected.");

}

/**
 * @brief HelicalMainWindow::serverFoundOther
 */
void HelicalMainWindow::serverFoundOther()
{

    QString message { "The host key for this server was not found but an other type of key exists."
                      "An attacker might change the default server key to confuse your client into "
                      "thinking the key does not exist" };

    QMessageBox::warning(this, QCoreApplication::applicationName(), message);

    ui->currentStatusLabel->setText("Disconnected.");

}

/**
 * @brief HelicalMainWindow::serverFileNotFound
 * @param keyHash
 */
void HelicalMainWindow::serverFileNotFound(std::vector<unsigned char> &keyHash)
{
    QString message { "Could not find known host file.\n"
                      "If you accept the host key here, the file will be automatically created."};

    QMessageBox::information(this, QCoreApplication::applicationName(), message);

    serverNotKnown(keyHash);

}
/**
 * @brief HelicalMainWindow::serverNotKnown
 * @param keyHash
 */
void HelicalMainWindow::serverNotKnown(std::vector<unsigned char> &keyHash)
{

    QString message { "The server is unknown. Do you trust the host key? Public key hash: "+
                      QString::fromStdString(m_session->getSession().convertKeyHashToHex(keyHash))};

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, QCoreApplication::applicationName(), message, QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        m_session->getSession().writeKnownHost();
        emit serverVerified();
    } else {
        ui->currentStatusLabel->setText("Disconnected.");
    }

}
/**
 * @brief HelicalMainWindow::serverError
 */
void HelicalMainWindow::serverError()
{

}

