/*
 * File:   helicalfiletransfercontroller.h
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

#ifndef HELICALFILETRANSFERCONTROLLER_H
#define HELICALFILETRANSFERCONTROLLER_H

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

#include <QObject>
#include <QMap>

#include "helicalfiletransfertask.h"

class HelicalFileTransferController : public QObject
{
    Q_OBJECT

public:

    // Main consstructor

    explicit HelicalFileTransferController(QObject *parent = nullptr);

    // Create/destroy file transfer task

    void createFileTransferTask(QtSSH &session);
    void destroyFileTransferTask();

signals:

    // Open/close session

    void openSession(const QString &serverName, const QString serverPort, const QString &userName, const QString &userPassword);
    void closeSession();

    // File/directory procesing

    void processFile(const FileTransferAction &fileTransaction);
    void processDirectory(const FileTransferAction &fileTransaction);

    // UI feedback

    void statusMessage(const QString &message);
    void finishedTransactionMessage(const QString &message);
    void errorTransactionMessage(const QString &message);
    void updateRemoteFileList();

public slots:

    // File transaction processing

    void fileFinished(quint64 transactionID);
    void queueFileForProcessing(const FileTransferAction &fileTransaction);
    void processNextFile();

    // Error message feedback

    void error(const QString &errorTransactionMessage, int errorCode, quint64 transactionID);

private:
    QScopedPointer<HelicalFileTransferTask> m_fileTransferTask;     // File transaction task

    // File transaction queues (maps indexed by ID at present)

    std::uint64_t m_nextID {0};
    QMap<std::uint64_t, FileTransferAction> m_queuedFileTransactions;
    QMap<std::uint64_t, FileTransferAction> m_beingProcessedFileTransactions;
    QMap<std::uint64_t, FileTransferAction> m_fileTransactionsInError;

};

#endif // HELICALFILETRANSFERCONTROLLER_H
