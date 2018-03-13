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
// the main client UI and the file trasnfer task. It maintains the list of queued requests and passes
// them onto the file trasnfer task as and when needed. It also queues any transfer requests generated as
// a result of direct commands to the task such is list a local/remote directory recursively.
//

// =============
// INCLUDE FILES
// =============

#include "helicalfiletransfercontroller.h"

// File transaction queues.

QMutex HelicalFileTransferController::m_queueMutex;
std::uint64_t HelicalFileTransferController::m_nextID {0};
QMap<std::uint64_t, FileTransferAction> HelicalFileTransferController::m_queuedFileTransactions;
QMap<std::uint64_t, FileTransferAction> HelicalFileTransferController::m_beingProcessedFileTransactions;
QMap<std::uint64_t, FileTransferAction> HelicalFileTransferController::m_fileTransactionsInError;

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
    connect(this,&HelicalFileTransferController::processFileTransaction, m_fileTransferTask.data(), &HelicalFileTransferTask::processFileTransaction);

    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::uploadFinished,
            [=]( const QString &/*sourceFile*/, const QString &/*destinationFile*/, quint64 transactionID ) { this->fileTransactionFinished(transactionID); });

    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::downloadFinished,
            [=]( const QString &/*sourceFile*/, const QString &/*destinationFile*/, quint64 transactionID ) { this->fileTransactionFinished(transactionID); });

    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::deleteFileFinised,
            [=]( const QString &/*fileName*/, quint64 transactionID) { this->fileTransactionFinished(transactionID); });

    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::listRecursiveFinished,
            [=]( const QString &/*fileName*/, quint64 transactionID) { this->fileTransactionFinished(transactionID); });


    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::queueFileTransaction, this, &HelicalFileTransferController::queueFileTransaction);

    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::error, this, &HelicalFileTransferController::error);

    // Delete thread when it is finished

    connect(m_fileTransferTask->fileTaskThread(),&QThread::finished,m_fileTransferTask->fileTaskThread(), &QThread::deleteLater );
    emit openSession(session.getServerName(), session.getServerPort(), session.getUserName(), session.getUserPassword());

    // Check file transation queue every second

    startTimer(1000);

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
 * @brief HelicalFileTransferController::addFileTrasnsactionToQueue
 * @param fileTransaction
 */
void HelicalFileTransferController::addFileTrasnsactionToQueue(const FileTransferAction &fileTransaction)
{
    QMutexLocker locker(&m_queueMutex);
    m_queuedFileTransactions[m_nextID] = fileTransaction;
    m_queuedFileTransactions[m_nextID].m_fileTransferID = m_nextID;
    m_nextID++;
}

/**
 * @brief HelicalFileTransferController::nextFileTrasnsaction
 * @return
 */
FileTransferAction HelicalFileTransferController::nextFileTrasnsaction()
{
    QMutexLocker locker(&m_queueMutex);
    FileTransferAction nextTransaction =  m_queuedFileTransactions.first();
    m_queuedFileTransactions.remove(nextTransaction.m_fileTransferID);
    m_beingProcessedFileTransactions[nextTransaction.m_fileTransferID] = nextTransaction;
    return(nextTransaction);
}

/**
 * @brief HelicalFileTransferController::fileTrasnsactionError
 * @param transactionID
 */
void HelicalFileTransferController::fileTrasnsactionError(quint64 transactionID)
{
    QMutexLocker locker(&m_queueMutex);
    FileTransferAction nextTransaction;
    if (m_beingProcessedFileTransactions.find(transactionID)!=m_beingProcessedFileTransactions.end()) {
        FileTransferAction nextTransaction =  m_beingProcessedFileTransactions[transactionID];
        m_beingProcessedFileTransactions.remove(transactionID);
        m_fileTransactionsInError[transactionID] = nextTransaction;
    }
}

/**
 * @brief HelicalFileTransferController::removeFinishedFileTrasnsaction
 * @param transactionID
 * @param fileTransaction
 * @return
 */
bool HelicalFileTransferController::removeFinishedFileTrasnsaction(quint64 transactionID, FileTransferAction &fileTransaction)
{
    QMutexLocker locker(&m_queueMutex);
    if (m_beingProcessedFileTransactions.find(transactionID)!=m_beingProcessedFileTransactions.end()) {
        fileTransaction =  m_beingProcessedFileTransactions[transactionID];
        m_beingProcessedFileTransactions.remove(transactionID);
        return(true);
    }

    return(false);
}

/**
 * @brief HelicalFileTransferController::fileFinished
 *
 * File trasnaction finished slot. Remove transaction from being processed queue and get next.
 *
 * @param transactionID
 */
void HelicalFileTransferController::fileTransactionFinished(quint64 transactionID)
{

    FileTransferAction fileTransaction;

    if (removeFinishedFileTrasnsaction(transactionID, fileTransaction) && !fileTransaction.m_directory) {

        switch (fileTransaction.m_action) {

        case DOWNLOAD:
            emit finishedTransactionMessage(QString("Downloaded File \"%1\" to \"%2\".\n").arg(fileTransaction.m_sourceFile).arg(fileTransaction.m_destinationFile));
            break;

        case UPLOAD:
            emit finishedTransactionMessage(QString("Uploaded File \"%1\" to \"%2\".\n").arg(fileTransaction.m_sourceFile).arg(fileTransaction.m_destinationFile));
            break;

        case DELETE:
            emit finishedTransactionMessage(QString("Deleted File \"%1\".\n").arg(fileTransaction.m_sourceFile));
            break;

        }

        emit updateTransactionTotals(m_queuedFileTransactions.size(), m_beingProcessedFileTransactions.size(), m_fileTransactionsInError.size());

    }

    setBusy(false);

    processNextTransaction();

}

/**
 * @brief HelicalFileTransferController::queueFileForProcessing
 *
 * Queue file transaction for processing.
 *
 * @param fileName
 */
void HelicalFileTransferController::queueFileTransaction(const FileTransferAction &fileTransaction)
{

    addFileTrasnsactionToQueue(fileTransaction);

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

    emit updateTransactionTotals(m_queuedFileTransactions.size(), m_beingProcessedFileTransactions.size(), m_fileTransactionsInError.size());

}

/**
 * @brief HelicalFileTransferController::processNextFile
 *
 * Process next file transaction if queue not empty.
 *
 */
void HelicalFileTransferController::processNextTransaction()
{

    if (!m_queuedFileTransactions.empty()) {
        if (m_queuedFileTransactions.first().m_action & supportedTransactions()){
            FileTransferAction nextTransaction =  nextFileTrasnsaction();
            setBusy(true);
            emit processFileTransaction(nextTransaction);
        }
    } else {
        emit updateTransactionTotals(m_queuedFileTransactions.size(), m_beingProcessedFileTransactions.size(), m_fileTransactionsInError.size());
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

    fileTrasnsactionError(transactionID);

}

/**
 * @brief HelicalFileTransferController::timerEvent
 */
void HelicalFileTransferController::timerEvent(QTimerEvent */*event*/)
{
    if (!m_queuedFileTransactions.empty() && !busy()) {
        processNextTransaction();
    }

}

/**
 * @brief HelicalFileTransferController::busy
 * @return
 */
bool HelicalFileTransferController::busy() const
{
    return m_busy;
}

/**
 * @brief HelicalFileTransferController::setBusy
 * @param busy
 */
void HelicalFileTransferController::setBusy(bool busy)
{
    m_busy = busy;
}

/**
 * @brief HelicalFileTransferController::supportedTransactions
 * @return
 */
std::uint32_t HelicalFileTransferController::supportedTransactions() const
{
    return m_supportedTransactions;
}

/**
 * @brief HelicalFileTransferController::setSupportedTransactions
 * @param supportedTransactions
 */
void HelicalFileTransferController::setSupportedTransactions(const std::uint32_t supportedTransactions)
{
    m_supportedTransactions = supportedTransactions;
}

