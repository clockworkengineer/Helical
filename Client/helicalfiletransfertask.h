/*
 * File:   helicalfiletransfertask.h
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

#ifndef HELICALFILETRANSFERTASK_H
#define HELICALFILETRANSFERTASK_H

//
// Class: HelicalFileTransferTask
//
// Description: Class to implement SFTP file transfer task that talks to an SFTP server and
// executes SFTP commands (upload/download/delete files). The task runs on a separate thread
// and commuicates with the main client using signal/slots.
//

// =============
// INCLUDE FILES
// =============

#include <QObject>
#include <QThread>
#include <QScopedPointer>

#include "helical.h"
#include "QtSSH/qtssh.h"
#include "QtSSH/qtsftp.h"

// =================
// CLASS DECLARATION
// =================

class HelicalFileTransferTask : public QObject
{
    Q_OBJECT

public:

    // Class exception

    struct Exception : public std::runtime_error {

        Exception(const QString & messageStr)
            : std::runtime_error(static_cast<QString>("HelicalFileTransferTask Failure: " + messageStr).toStdString()) {
        }

    };

    // Main constructor

    explicit HelicalFileTransferTask(QObject *parent = nullptr);

    // Set/get main thread

    QThread *fileTaskThread() const;
    void setFileTaskThread(QThread *fileTaskThread);

signals:

    // Task controller siganals

    void uploadFinished(const QString &sourceFile, const QString &destinationFile, quint64 transactionID);
    void downloadFinished(const QString &sourceFile, const QString &destinationFile, quint64 transactionID);
    void deleteFileFinised(const QString &fileName, quint64 transactionID);
    void listRecursiveFinished(const QString &fileName, quint64 transactionID);
    void queueFileForProcessing(const FileTransferAction &fileTransaction);
    void error(const QString &errorMessage, int errorCode,  quint64 transactionID);

public slots:

    // Open/close session

    void openSession(const QString &serverName, const QString serverPort, const QString &userName, const QString &userPassword);
    void closeSession();

     // File transaction processing

    void processFile(const FileTransferAction &fileTransaction);

private:
    QThread *m_fileTransferTaskThread;  // File transfer task thread
    QScopedPointer<QtSSH> m_session;    // File transfer task SSH Session
    QScopedPointer<QtSFTP> m_sftp;      // File transfer task SFTP Session

};

#endif // HELICALFILETRANSFERTASK_H
