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
            [=]( const QString &sourceFile, const QString &destinationFile, quint64 transactionID ) { this->fileFinished(transactionID); });

    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::downloadFinished,
            [=]( const QString &sourceFile, const QString &destinationFile, quint64 transactionID ) { this->fileFinished(transactionID); });

    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::deleteFileFinised,
            [=]( const QString &fileName, quint64 transactionID) { this->fileFinished(transactionID); });

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

    m_queuedFileTransactions.clear();
    m_queuedFileTransactions.clear();

    emit closeSession();
    m_fileTransferTask.take();

}

/**
 * @brief HelicalFileTransferController::fileFinished
 * @param sourceFile
 * @param destinationFile
 */
void HelicalFileTransferController::fileFinished(quint64 transactionID)
{

    if (!m_beingProcessedFileTransactions.empty()) {

        FileTransferAction nextTransaction =  m_beingProcessedFileTransactions.first();
        m_beingProcessedFileTransactions.remove(nextTransaction.m_fileTransferID);

        switch (nextTransaction.m_action) {

        case DOWNLOAD:
            emit finishedMessage(QString("Downloaded File \"%1\" to \"%2\".\n").arg(nextTransaction.m_sourceFile).arg(nextTransaction.m_destinationFile));
            break;

        case UPLOAD:
            emit finishedMessage(QString("Uploaded File \"%1\" to \"%2\".\n").arg(nextTransaction.m_sourceFile).arg(nextTransaction.m_destinationFile));
            break;

        case DELETE:
            emit finishedMessage(QString("Deleted File \"%1\".\n").arg(nextTransaction.m_sourceFile));
             break;

        }

        processNextFile();

    }

}

/**
 * @brief HelicalFileTransferController::queueFileForProcessing
 * @param fileName
 */
void HelicalFileTransferController::queueFileForProcessing(const FileTransferAction &fileTransaction)
{

    m_queuedFileTransactions[m_nextID] = fileTransaction;
    m_queuedFileTransactions[m_nextID].m_fileTransferID = m_nextID;
    m_nextID++;

    switch (fileTransaction.m_action) {

    case DOWNLOAD:
        emit statusMessage(QString("File \"%1\" queued for download.\n").arg(fileTransaction.m_sourceFile));
        break;

    case UPLOAD:
        emit statusMessage(QString("File \"%1\" queued for upload.\n").arg(fileTransaction.m_sourceFile));

        break;

    case DELETE:
        emit statusMessage(QString("File \"%1\" queued for delete.\n").arg(fileTransaction.m_sourceFile));
        break;

    }
}

/**
 * @brief HelicalFileTransferController::processNextFile
 */
void HelicalFileTransferController::processNextFile()
{

    if (!m_queuedFileTransactions.empty()) {
        FileTransferAction nextTransaction =  m_queuedFileTransactions.first();
        m_queuedFileTransactions.remove(nextTransaction.m_fileTransferID);
        m_beingProcessedFileTransactions[nextTransaction.m_fileTransferID] = nextTransaction;
        emit processFile(nextTransaction);
    } else {
        emit statusMessage("Transfer queue clear.\n");
        emit updateRemoteFileList();
    }
}

/**
 * @brief HelicalFileTransferController::error
 * @param errorMessage
 * @param errorCode
 */
void HelicalFileTransferController::error(const QString &errorMsg, int errorCode, quint64 transactionID)
{
    Q_UNUSED(errorCode);

    emit errorMessage(errorMsg+"\n");

    if (!m_beingProcessedFileTransactions.empty()) {
        FileTransferAction nextTransaction =  m_beingProcessedFileTransactions.first();
        m_beingProcessedFileTransactions.remove(nextTransaction.m_fileTransferID);
        m_fileTransactionsInError[nextTransaction.m_fileTransferID] = nextTransaction;
    }

    emit processNextFile();

}

