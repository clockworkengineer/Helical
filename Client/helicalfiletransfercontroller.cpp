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
// Description:  Class to implement file transfer task controller. This is the interface between
// the main client UI and the file trasnfer task. If maintains the list of queued requests and passes
// them onto the file trasnfer task as and when needed. It also queues any transfer requests generated as
// a result of direct commands to the task such is list a local/remote directory recursively.
//

// =============
// INCLUDE FILES
// =============

#include "helicalfiletransfercontroller.h"

/**
 * @brief HelicalFileTransferController::HelicalFileTransferController
 * @param parent
 */
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

    // Create task thread, start it and move task object to it

    QScopedPointer<QThread> fileTransferThread { new QThread() };

    m_fileTransferTask.reset(new HelicalFileTransferTask());
    m_fileTransferTask->setFileTaskThread(fileTransferThread.take());
    m_fileTransferTask->moveToThread(m_fileTransferTask->fileTaskThread());
    m_fileTransferTask->fileTaskThread()->start();

    // Signal/Slot interfce between controller/task

    connect(this,&HelicalFileTransferController::openSession, m_fileTransferTask.data(), &HelicalFileTransferTask::openSession);
    connect(this,&HelicalFileTransferController::closeSession, m_fileTransferTask.data(), &HelicalFileTransferTask::closeSession);
    connect(this,&HelicalFileTransferController::processFile, m_fileTransferTask.data(), &HelicalFileTransferTask::processFile);
    connect(this,&HelicalFileTransferController::processDirectory, m_fileTransferTask.data(), &HelicalFileTransferTask::processDirectory);

    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::uploadFinished,
            [=]( const QString &/*sourceFile*/, const QString &/*destinationFile*/, quint64 transactionID ) { this->fileFinished(transactionID); });

    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::downloadFinished,
            [=]( const QString &/*sourceFile*/, const QString &/*destinationFile*/, quint64 transactionID ) { this->fileFinished(transactionID); });

    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::deleteFileFinised,
            [=]( const QString &/*fileName*/, quint64 transactionID) { this->fileFinished(transactionID); });

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
    m_beingProcessedFileTransactions.clear();
    m_fileTransactionsInError.clear();

    emit closeSession();
    m_fileTransferTask.take();

}

/**
 * @brief HelicalFileTransferController::fileFinished
 *
 * File trasnaction finished slot. Remove transaction from being processed queue and get next.
 *
 * @param transactionID
 */
void HelicalFileTransferController::fileFinished(quint64 transactionID)
{

    if (m_beingProcessedFileTransactions.find(transactionID)!=m_beingProcessedFileTransactions.end()) {

        FileTransferAction nextTransaction =  m_beingProcessedFileTransactions[transactionID];
        m_beingProcessedFileTransactions.remove(transactionID);

        switch (nextTransaction.m_action) {

        case DOWNLOAD:
            emit finishedTransactionMessage(QString("Downloaded File \"%1\" to \"%2\".\n").arg(nextTransaction.m_sourceFile).arg(nextTransaction.m_destinationFile));
            break;

        case UPLOAD:
            emit finishedTransactionMessage(QString("Uploaded File \"%1\" to \"%2\".\n").arg(nextTransaction.m_sourceFile).arg(nextTransaction.m_destinationFile));
            break;

        case DELETE:
            emit finishedTransactionMessage(QString("Deleted File \"%1\".\n").arg(nextTransaction.m_sourceFile));
            break;

        }

    }

    processNextFile();

}

/**
 * @brief HelicalFileTransferController::queueFileForProcessing
 *
 * Queue file transaction for processing.
 *
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
 *
 * Process next file transaction if queue not empty.
 *
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
 *
 * A file transaction error has occurred.
 *
 * @param errorMsg      Error message
 * @param errorCode     Error code
 * @param transactionID Transaction code.
 */
void HelicalFileTransferController::error(const QString &errorMsg, int errorCode, quint64 transactionID)
{
    Q_UNUSED(errorCode);

    emit errorTransactionMessage(errorMsg+"\n");

    if (m_beingProcessedFileTransactions.find(transactionID)!=m_beingProcessedFileTransactions.end()) {
        FileTransferAction nextTransaction =  m_beingProcessedFileTransactions[transactionID];
        m_beingProcessedFileTransactions.remove(transactionID);
        m_fileTransactionsInError[transactionID] = nextTransaction;
    }

    emit processNextFile();

}

