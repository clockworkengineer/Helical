#include "helicalfiletransfertask.h"
#include <QDebug>
HelicalFileTransferTask::HelicalFileTransferTask(QObject *parent) : QObject(parent)
{

}

QThread *HelicalFileTransferTask::fileTaskThread() const
{
    return m_fileTaskThread;
}

void HelicalFileTransferTask::setFileTaskThread(QThread *fileTaskThread)
{
    m_fileTaskThread = fileTaskThread;
}

void HelicalFileTransferTask::openSession(const QString &serverName, const QString serverPort, const QString &userName, const QString &userPassword)
{
    m_session.reset(new QtSSH());

    m_session->setSessionDetails(serverName, serverPort, userName, userPassword);

    m_session->connectToServer();
    m_session->verifyServer();
    m_session->authorizeUser();

    if (m_session->isConnected() && m_session->isAuthorized()) {
        qDebug() << "CONNECTED";
    }

    m_sftp.reset(new QtSFTP(*m_session));

    m_sftp->open();

}

void HelicalFileTransferTask::closeSession()
{

    m_sftp->close();
    m_sftp.reset();
    m_session->disconnectFromServer();
    m_session.reset();
}

void HelicalFileTransferTask::uploadFile(const QString &sourceFile, const QString &destinationFile)
{
    qDebug() << "UPLOAD " << sourceFile;
    m_sftp->putLocalFile(sourceFile, destinationFile);
    emit uploadFinished(sourceFile, destinationFile);
}

void HelicalFileTransferTask::downloadFile(const QString &sourceFile, const QString &destinationFile)
{
    qDebug() << "DOWNLOAD " << sourceFile;
    m_sftp->getRemoteFile(sourceFile, destinationFile);
    emit downloadFinished(sourceFile, destinationFile);
}
