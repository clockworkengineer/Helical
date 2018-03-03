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

//    typedef std::pair<QString, QString> FileTransferPair;
//    typedef HelicalFileTransferTask::FileAction FileAction;
//    typedef HelicalFileTransferTask::FileMappingPair FileMappingPair;

public:

    explicit HelicalFileTransferController(QObject *parent = nullptr);

    void createFileTransferTask(QtSSH &session);
    void destroyFileTransferTask();

signals:
    void openSession(const QString &serverName, const QString serverPort, const QString &userName, const QString &userPassword);
    void closeSession();
    void processFile(FileAction action, const QString &sourceFile, const QString &destinationFile="");
    void processDirectory(FileAction action,const QString &directoryPath, const FileMappingPair &FileMappingPair=FileMappingPair());

    void statusMessage(const QString &message);
    void updateRemoteFileList();
    void error(const QString &errorMessage, int errorCode);

public slots:
    void fileFinished(FileAction action, const QString &sourceFile, const QString &destinationFile);
    void queueFileForProcessing(FileAction action, const QString &sourceFile, const QString &destinationFile);
    void processNextFile(FileAction action);

private:
    QScopedPointer<HelicalFileTransferTask> m_fileTransferTask;

    std::uint64_t m_nextID {0};
    QList<FileTransferAction> m_downloadQueue;
    QList<FileTransferAction> m_uploadQueue;
    QList<FileTransferAction> m_deleteQueue;
};

#endif // HELICALFILETRANSFERCONTROLLER_H
