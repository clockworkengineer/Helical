#include "helicalmainwindow.h"
#include "ui_heilcalmainwindow.h"
#include <QDebug>

HelicalMainWindow::HelicalMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HeilcalMainWindow)

{
    ui->setupUi(this);

    QCoreApplication::setOrganizationName("ClockWorkEngineer");
    QCoreApplication::setApplicationName("Helical");

    QSettings helicalSettings;
    helicalSettings.beginGroup("SSH");
    m_serverName = helicalSettings.value("server").toString();
    m_serverPort = helicalSettings.value("port").toString();
    m_userName = helicalSettings.value("user").toString();
    m_userPassword = helicalSettings.value("password").toString();
    helicalSettings.endGroup();

    connect(&m_session, &QtSSH::error, this, &HelicalMainWindow::error);
    connect(&m_session, &QtSSH::connectedToServer, this, &HelicalMainWindow::connectedToServer);
    connect(&m_session, &QtSSH::serverVerified, this, &HelicalMainWindow::serverVerified);
    connect(&m_session, &QtSSH::userAuthorized, this, &HelicalMainWindow::userAuthorized);

    setWindowTitle(m_serverName);

    // Set session details

    m_session.setSessionDetails(m_serverName,m_serverPort, m_userName, m_userPassword);

    // Connect to server

    m_session.connectToServer();

}

HelicalMainWindow::~HelicalMainWindow()
{
    delete ui;
}

void HelicalMainWindow::error(const QString &errorMessage, int errorCode)
{

    qDebug() << errorMessage << " " << errorCode;

}

void HelicalMainWindow::serverVerified()
{
    QtSSH *session { qobject_cast<QtSSH*>(sender()) };

    qDebug() << "Server verified...";
    if (session) {
        session->authorizeUser();
    }

}

void HelicalMainWindow::userAuthorized()
{
    qDebug() << "User Authorized ...";

    m_connectionWindow.reset(new HelicalConnectionDialog(m_session));
    m_connectionWindow->show();
    m_connectionWindow->activateWindow();
    m_connectionWindow->runShell(80, 24);

}

void HelicalMainWindow::connectedToServer()
{
    QtSSH *session { qobject_cast<QtSSH*>(sender()) };

    qDebug() << "Connetected to server ...";
    if (session) {
        session->verifyServer();
    }

}
