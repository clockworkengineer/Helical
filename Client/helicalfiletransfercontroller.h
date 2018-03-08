#ifndef HELICALFILETRANSFERCONTROLLER_H
#define HELICALFILETRANSFERCONTROLLER_H

/*
 * File:   helicalfiletransfercontroller.h
 *
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
