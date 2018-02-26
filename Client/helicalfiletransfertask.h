#ifndef HELICALFILETRANSFERTASK_H
#define HELICALFILETRANSFERTASK_H

#include <QObject>
#include <QThread>
#include <QScopedPointer>

#include "QtSSH/qtssh.h"
#include "QtSSH/qtsftp.h"
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

//
// Class: HelicalFileTransferTask
//
// Description:
//

// =============
// INCLUDE FILES
// =============

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

    explicit HelicalFileTransferTask(QObject *parent = nullptr);

    QThread *fileTaskThread() const;
    void setFileTaskThread(QThread *fileTaskThread);

signals:
    void uploadFinished(const QString &sourceFile, const QString &destinationFile);
    void downloadFinished(const QString &sourceFile, const QString &destinationFile);
    void listedRemoteFileName(const QString &fileName);
    void listedLocalFileName(const QString &fileName);
    void startDownloading();
    void startUploading();
    void error(const QString &errorMessage, int errorCode);

public slots:
    void openSession(const QString &serverName, const QString serverPort, const QString &userName, const QString &userPassword);
    void closeSession();
    void uploadFile(const QString &sourceFile, const QString &destinationFile);
    void downloadFile(const QString &sourceFile, const QString &destinationFile);
    void listRemoteDirectoryRecursive(const QString &directoryPath);
    void listLocalDirectoryRecursive(const QString &directoryPath);

private:
    QThread *m_fileTaskThread;
    QScopedPointer<QtSSH> m_session;
    QScopedPointer<QtSFTP> m_sftp;
    std::atomic_bool m_busy {false};

};

#endif // HELICALFILETRANSFERTASK_H
