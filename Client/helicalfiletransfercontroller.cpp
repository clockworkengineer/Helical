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

    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::downloadFinished, this, &HelicalFileTransferController::downloadFinished);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::uploadFinished, this, &HelicalFileTransferController::uploadFinished);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::deleteFileFinised, this, &HelicalFileTransferController::deleteFileFinised);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::queueFileForDownload, this, &HelicalFileTransferController::queueFileForDownload);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::queueFileForUpload, this, &HelicalFileTransferController::queueFileForUpload);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::queueFileForDelete, this, &HelicalFileTransferController::queueFileForDelete);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::startDownloading, this, &HelicalFileTransferController::downloadNextFile);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::startUploading, this, &HelicalFileTransferController::uploadNextFile);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::startDeleting, this, &HelicalFileTransferController::deleteNextFile);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::error, this, &HelicalFileTransferController::error);

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

    m_downloadQueue.clear();
    m_uploadQueue.clear();

    emit closeSession();
    m_fileTransferTask.take();

}

/**
 * @brief HelicalFileTransferController::uploadFinished
 * @param sourceFile
 * @param destinationFile
 */
void HelicalFileTransferController::uploadFinished(const QString &sourceFile, const QString &destinationFile)
{

    emit statusMessage(QString("Uploaded File \"%1\" to \"%2\".\n").arg(sourceFile).arg(destinationFile));

    uploadNextFile();

}

/**
 * @brief HelicalFileTransferController::downloadFinished
 * @param sourceFile
 * @param destinationFile
 */
void HelicalFileTransferController::downloadFinished(const QString &sourceFile, const QString &destinationFile)
{
    emit statusMessage(QString("Downloaded File \"%1\" to \"%2\".\n").arg(sourceFile).arg(destinationFile));

    downloadNextFile();

}

/**
 * @brief HelicalFileTransferController::fileDeleted
 * @param filePath
 */
void HelicalFileTransferController::deleteFileFinised(const QString &filePath)
{
    emit statusMessage(QString("Deleted File \"%1\".\n").arg(filePath));

    deleteNextFile();

}

/**
 * @brief HelicalFileTransferController::queueFileForDownload
 * @param fileName
 */
void HelicalFileTransferController::queueFileForDownload(const QString &sourceFile, const QString &destinationFile)
{
    emit statusMessage(QString("File \"%1\" queued for download.\n").arg(sourceFile));
    m_downloadQueue.push_back({sourceFile, destinationFile});
}

/**
 * @brief HelicalFileTransferController::queueFileForUpload
 * @param fileName
 */
void HelicalFileTransferController::queueFileForUpload(const QString &sourceFile, const QString &destinationFile)
{
    emit statusMessage(QString("File \"%1\" queued for upload.\n").arg(sourceFile));
    m_uploadQueue.push_back({sourceFile, destinationFile});
}

/**
 * @brief HelicalFileTransferController::queueFileForDelete
 * @param fileName
 */
void HelicalFileTransferController::queueFileForDelete(const QString &fileName)
{
    emit statusMessage(QString("File \"%1\" queued for delete.\n").arg(fileName));
    m_deleteQueue.push_back(fileName);
}

/**
 * @brief HelicalFileTransferController::downloadNextFile
 */
void HelicalFileTransferController::downloadNextFile()
{
    if (!m_downloadQueue.isEmpty()) {
        emit downloadFile(m_downloadQueue.front().first, m_downloadQueue.front().second);
        m_downloadQueue.pop_front();
    } else {
        emit statusMessage("Download queue cleared.\n");
        emit updateRemoteFileList();
    }
}

/**
 * @brief HelicalFileTransferController::uploadNextFile
 */
void HelicalFileTransferController::uploadNextFile()
{
    if (!m_uploadQueue.isEmpty()) {
        emit uploadFile(m_uploadQueue.front().first, m_uploadQueue.front().second);
        m_uploadQueue.pop_front();
    } else {
        emit statusMessage("Upload queue cleared.\n");
        emit updateRemoteFileList();
    }
}

/**
 * @brief HelicalFileTransferController::deleteNextFile
 */
void HelicalFileTransferController::deleteNextFile()
{
    if (!m_deleteQueue.isEmpty()) {
        emit deleteFile(m_deleteQueue.front());
        m_deleteQueue.pop_front();
    } else {
        emit statusMessage("Delete queue cleared.\n");
        emit updateRemoteFileList();
    }
}
