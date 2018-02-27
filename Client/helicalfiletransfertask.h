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

    typedef std::pair<QString, QString> FileMappingPair;

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
    void deleteFileFinised(const QString &fileName);
    void queueFileForDownload(const QString &sourceFile, const QString &destinationFile);
    void queueFileForUpload(const QString &sourceFile, const QString &destinationFile);
    void queueFileForDelete(const QString &fileName);
    void startDownloading();
    void startUploading();
    void startDeleting();
    void error(const QString &errorMessage, int errorCode);

public slots:
    void openSession(const QString &serverName, const QString serverPort, const QString &userName, const QString &userPassword);
    void closeSession();
    void uploadFile(const QString &sourceFile, const QString &destinationFile);
    void deleteFile(const QString &fileName);
    void downloadFile(const QString &sourceFile, const QString &destinationFile);
    void downloadDirectory(const FileMappingPair &fileMappingPair, const QString &directoryPath);
    void uploadDirectory(const FileMappingPair &fileMappingPair, const QString &directoryPath);
    void deleteDirectory(const QString &directoryPath);

private:
    QThread *m_fileTaskThread;
    QScopedPointer<QtSSH> m_session;
    QScopedPointer<QtSFTP> m_sftp;
    std::atomic_bool m_busy {false};

};

Q_DECLARE_METATYPE(HelicalFileTransferTask::FileMappingPair);

#endif // HELICALFILETRANSFERTASK_H
