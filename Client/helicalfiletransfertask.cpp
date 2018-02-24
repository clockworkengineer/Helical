/*
 * File:   helicalfiletransfertask.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

//
// Class: HelicalFileTransferTask
//
// Description:
//

// =============
// INCLUDE FILES
// =============

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
        m_sftp.reset(new QtSFTP(*m_session));
        m_sftp->open();
        connect(m_sftp.data(), &QtSFTP::uploadFinished, this,  &HelicalFileTransferTask::uploadFinished);
        connect(m_sftp.data(), &QtSFTP::downloadFinished, this,  &HelicalFileTransferTask::downloadFinished);
        connect(m_sftp.data(), &QtSFTP::listedRemoteFileName, this,  &HelicalFileTransferTask::listedRemoteFileName);
    }
}

void HelicalFileTransferTask::closeSession()
{
    qDebug() << "CLOSEDOWN";
    if (m_sftp) {
        m_sftp->close();
        m_sftp.reset();
    }
    if (m_session) {
        m_session->disconnectFromServer();
        m_session.reset();
    }
    deleteLater();
  //  m_terminate=false;

}

void HelicalFileTransferTask::uploadFile(const QString &sourceFile, const QString &destinationFile)
{
    qDebug() << "UPLOAD " << sourceFile;
    if (m_sftp) {
        m_sftp->putLocalFile(sourceFile, destinationFile);
    }
//    if (m_terminate) {
//        closeSession();
//    }
}

void HelicalFileTransferTask::downloadFile(const QString &sourceFile, const QString &destinationFile)
{
    qDebug() << "DOWNLOAD " << sourceFile;
    if (m_sftp) {
        m_sftp->getRemoteFile(sourceFile, destinationFile);
    }
//    if (m_terminate) {
//        closeSession();
//    }
}

void HelicalFileTransferTask::listRemoteDirectoryRecursive(const QString &directoryPath)
{

    if (m_sftp) {
        m_sftp->listRemoteDirectoryRecursive(directoryPath);
        emit startDownloading();
    }
//    if (m_terminate) {
//        closeSession();
//    }
}
