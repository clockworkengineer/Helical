/*
 * File:   helicalfiletransfercontroller.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

//
// Class: HelicalFileTransferController
//
// Description:
//

// =============
// INCLUDE FILES
// =============

#include "helicalsftpdialog.h"
#include "helicalfiletransfercontroller.h"

HelicalFileTransferController::HelicalFileTransferController(QObject *parent) : QObject(parent)
{

}


/**
 * @brief HelicalFileTransferController::createFileTransferTask
 *
 * Create SFTP file transfer task.
 *
 * @param session
 */

void HelicalFileTransferController::createFileTransferTask(QtSSH &session)
{
    QScopedPointer<QThread> fileTransferThread { new QThread() };
    HelicalSFTPDialog *clientDialog= dynamic_cast<HelicalSFTPDialog*>(this->parent());

    m_fileTransferTask.reset(new HelicalFileTransferTask());
    m_fileTransferTask->setFileTaskThread(fileTransferThread.take());
    m_fileTransferTask->moveToThread(m_fileTransferTask->fileTaskThread());
    m_fileTransferTask->fileTaskThread()->start();

    connect(this,&HelicalFileTransferController::openSession, m_fileTransferTask.data(), &HelicalFileTransferTask::openSession);
    connect(this,&HelicalFileTransferController::closeSession, m_fileTransferTask.data(), &HelicalFileTransferTask::closeSession);
    connect(this,&HelicalFileTransferController::uploadFile, m_fileTransferTask.data(), &HelicalFileTransferTask::uploadFile);
    connect(this,&HelicalFileTransferController::downloadFile, m_fileTransferTask.data(), &HelicalFileTransferTask::downloadFile);
    connect(this,&HelicalFileTransferController::deleteFile, m_fileTransferTask.data(), &HelicalFileTransferTask::deleteFile);
    connect(this,&HelicalFileTransferController::downloadDirectory, m_fileTransferTask.data(), &HelicalFileTransferTask::downloadDirectory);
    connect(this,&HelicalFileTransferController::uploadDirectory, m_fileTransferTask.data(), &HelicalFileTransferTask::uploadDirectory);
    connect(this,&HelicalFileTransferController::deleteDirectory, m_fileTransferTask.data(), &HelicalFileTransferTask::deleteDirectory);

    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::downloadFinished, clientDialog, &HelicalSFTPDialog::downloadFinished);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::uploadFinished, clientDialog, &HelicalSFTPDialog::uploadFinished);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::deleteFileFinised, clientDialog, &HelicalSFTPDialog::deleteFileFinised);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::queueFileForDownload, clientDialog, &HelicalSFTPDialog::queueFileForDownload);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::queueFileForUpload, clientDialog, &HelicalSFTPDialog::queueFileForUpload);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::queueFileForDelete, clientDialog, &HelicalSFTPDialog::queueFileForDelete);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::startDownloading, clientDialog, &HelicalSFTPDialog::downloadNextFile);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::startUploading, clientDialog, &HelicalSFTPDialog::uploadNextFile);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::startDeleting, clientDialog, &HelicalSFTPDialog::deleteNextFile);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::error, clientDialog, &HelicalSFTPDialog::error);

    // Delete thread when it is finished

    connect(m_fileTransferTask->fileTaskThread(),&QThread::finished,m_fileTransferTask->fileTaskThread(), &QThread::deleteLater );
    emit openSession(session.getServerName(), session.getServerPort(), session.getUserName(), session.getUserPassword());

}

/**
 * @brief HelicalFileTransferController::destroyFileTransferTask
 *
 * Close down and destroy file transfer task.
 *
 */
void HelicalFileTransferController::destroyFileTransferTask()
{

//    m_downloadQueue.clear();
//    m_uploadQueue.clear();

    emit closeSession();
    m_fileTransferTask.take();

}
