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
#include <QTimerEvent>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>

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

    // Error message feedback

    void error(const QString &errorTransactionMessage, int errorCode, quint64 transactionID);

protected:
    void timerEvent(QTimerEvent *event);

private:

    // Process next queued file

    void processNextFile();

    // Queue hancdling code

    void queueFileTrasnsaction(const FileTransferAction &fileTransaction);
    FileTransferAction nextFileTrasnsaction();
    void fileTrasnsactionError(quint64 transactionID);
    bool removeFinishedFileTrasnsaction(quint64 transactionID, FileTransferAction &fileTransaction);

    QScopedPointer<HelicalFileTransferTask> m_fileTransferTask;     // File transaction task

    std::atomic_bool m_busy {false};    // File transfer busy flag

    // File transaction queues (maps indexed by ID at present)

    static QMutex m_queueMutex;
    static std::uint64_t m_nextID;
    static QMap<std::uint64_t, FileTransferAction> m_queuedFileTransactions;
    static QMap<std::uint64_t, FileTransferAction> m_beingProcessedFileTransactions;
    static QMap<std::uint64_t, FileTransferAction> m_fileTransactionsInError;

};

#endif // HELICALFILETRANSFERCONTROLLER_H
