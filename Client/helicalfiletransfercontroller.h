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

    explicit HelicalFileTransferController(QObject *parent = nullptr);

    void createFileTransferTask(QtSSH &session);
    void destroyFileTransferTask();

signals:
    void openSession(const QString &serverName, const QString serverPort, const QString &userName, const QString &userPassword);
    void closeSession();
    void processFile(const FileTransferAction &fileTransaction);
    void processDirectory(const FileTransferAction &fileTransaction);

    void statusMessage(const QString &message);
    void finishedTransactionMessage(const QString &message);
    void errorTransactionMessage(const QString &message);
    void updateRemoteFileList();

public slots:
    void fileFinished(quint64 transactionID);
    void queueFileForProcessing(const FileTransferAction &fileTransaction);
    void processNextFile();
    void error(const QString &errorTransactionMessage, int errorCode, quint64 transactionID);

private:
    QScopedPointer<HelicalFileTransferTask> m_fileTransferTask;

    std::uint64_t m_nextID {0};
    QMap<std::uint64_t, FileTransferAction> m_queuedFileTransactions;
    QMap<std::uint64_t, FileTransferAction> m_beingProcessedFileTransactions;
    QMap<std::uint64_t, FileTransferAction> m_fileTransactionsInError;

};

#endif // HELICALFILETRANSFERCONTROLLER_H
