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
    connect(this,&HelicalFileTransferController::processFile, m_fileTransferTask.data(), &HelicalFileTransferTask::processFile);
    connect(this,&HelicalFileTransferController::processDirectory, m_fileTransferTask.data(), &HelicalFileTransferTask::processDirectory);

    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::uploadFinished,
            [=]( const QString &sourceFile, const QString &destinationFile ) { this->fileFinished(UPLOAD, sourceFile, destinationFile); });

    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::downloadFinished,
            [=]( const QString &sourceFile, const QString &destinationFile ) { this->fileFinished(DOWNLOAD, sourceFile, destinationFile); });

    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::deleteFileFinised,
            [=]( const QString &fileName) { this->fileFinished(DELETE, fileName, ""); });

    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::queueFileForProcessing, this, &HelicalFileTransferController::queueFileForProcessing);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::startFileProcessing, this, &HelicalFileTransferController::processNextFile);

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
void HelicalFileTransferController::fileFinished(FileAction action, const QString &sourceFile, const QString &destinationFile)
{

    switch (action) {

    case DOWNLOAD:
        emit statusMessage(QString("Downloaded File \"%1\" to \"%2\".\n").arg(sourceFile).arg(destinationFile));
        processNextFile(action);
        break;

    case UPLOAD:
        emit statusMessage(QString("Uploaded File \"%1\" to \"%2\".\n").arg(sourceFile).arg(destinationFile));
        processNextFile(action);
        break;

    case DELETE:
        emit statusMessage(QString("Deleted File \"%1\".\n").arg(sourceFile));
        processNextFile(action);
        break;

    }
}

/**
 * @brief HelicalFileTransferController::queueFileForProcessing
 * @param fileName
 */
void HelicalFileTransferController::queueFileForProcessing(const FileTransferAction &fileTransaction)
{
    switch (fileTransaction.m_action) {

    case DOWNLOAD:
        emit statusMessage(QString("File \"%1\" queued for download.\n").arg(fileTransaction.m_sourceFile));
        m_downloadQueue.push_back(fileTransaction);
        break;

    case UPLOAD:
        emit statusMessage(QString("File \"%1\" queued for upload.\n").arg(fileTransaction.m_sourceFile));
        m_uploadQueue.push_back(fileTransaction);
        break;

    case DELETE:
        emit statusMessage(QString("File \"%1\" queued for delete.\n").arg(fileTransaction.m_sourceFile));
        m_deleteQueue.push_back(fileTransaction);
        break;

    }
}

/**
 * @brief HelicalFileTransferController::processNextFile
 */
void HelicalFileTransferController::processNextFile(FileAction action)
{

    bool updateFileList {false};

    switch(action) {

    case DOWNLOAD:
        if (!m_downloadQueue.isEmpty()) {
            FileTransferAction fileTransaction { m_downloadQueue.front()  };
            m_downloadQueue.pop_front();
            emit processFile(fileTransaction);
        } else {
            emit statusMessage("Download queue clear.\n");
            updateFileList=true;
        }
        break;
    case UPLOAD:
        if (!m_uploadQueue.isEmpty()) {
            FileTransferAction fileTransaction { m_uploadQueue.front()  };
            m_uploadQueue.pop_front();
            emit processFile(fileTransaction);
        } else {
            emit statusMessage("Upload queue clear.\n");
            updateFileList=true;
        }
        break;
    case DELETE:
        if (!m_deleteQueue.isEmpty()) {
            FileTransferAction fileTransaction { m_deleteQueue.front()  };
            m_deleteQueue.pop_front();
            emit processFile(fileTransaction);
        } else {
            emit statusMessage("Delete queue clear\n");
            updateFileList=true;
        }
        break;
    }

    if (updateFileList) {
        emit updateRemoteFileList();
    }
}

