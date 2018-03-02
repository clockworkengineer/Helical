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

#include "helicalfiletransfertask.h"

class HelicalFileTransferController : public QObject
{
    Q_OBJECT

    typedef std::pair<QString, QString> FileTransferPair;

public:

    explicit HelicalFileTransferController(QObject *parent = nullptr);

    void createFileTransferTask(QtSSH &session);
    void destroyFileTransferTask();

signals:
    void openSession(const QString &serverName, const QString serverPort, const QString &userName, const QString &userPassword);
    void closeSession();
    void uploadFile(const QString &sourceFile, const QString &destinationFile);
    void downloadFile(const QString &sourceFile, const QString &destinationFile);
    void deleteFile(const QString &directoryPath, const QString &unused="");
    void downloadDirectory(const QString &directoryPath, const HelicalFileTransferTask::FileMappingPair &fileMappinegPair);
    void uploadDirectory(const QString &directoryPath, const HelicalFileTransferTask::FileMappingPair &fileMappinegPair);
    void deleteDirectory(const QString &directoryPath);
    void statusMessage(const QString &message);
    void updateRemoteFileList();
    void error(const QString &errorMessage, int errorCode);

public slots:
    void uploadFinished(const QString &sourceFile, const QString &destinationFile);
    void downloadFinished(const QString &sourceFile, const QString &destinationFile);
    void deleteFileFinised(const QString &filePath);
    void queueFileForProcessing(HelicalFileTransferTask::FileAction action, const QString &sourceFile, const QString &destinationFile);
    void processNextFile(HelicalFileTransferTask::FileAction action);


private:
    QScopedPointer<HelicalFileTransferTask> m_fileTransferTask;

    QList<FileTransferPair> m_downloadQueue;
    QList<FileTransferPair> m_uploadQueue;
    QList<QString> m_deleteQueue;
};

#endif // HELICALFILETRANSFERCONTROLLER_H
