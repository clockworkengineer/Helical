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
public:
    explicit HelicalFileTransferController(QObject *parent = nullptr);

    void createFileTransferTask(QtSSH &session);
    void destroyFileTransferTask();

signals:
        void openSession(const QString &serverName, const QString serverPort, const QString &userName, const QString &userPassword);
        void closeSession();
        void uploadFile(const QString &sourceFile, const QString &destinationFile);
        void downloadFile(const QString &sourceFile, const QString &destinationFile);
        void deleteFile(const QString &directoryPath);
        void downloadDirectory(const HelicalFileTransferTask::FileMappingPair &fileMappinegPair, const QString &directoryPath);
        void uploadDirectory(const HelicalFileTransferTask::FileMappingPair &fileMappinegPair, const QString &directoryPath);
        void deleteDirectory(const QString &directoryPath);
public slots:

private:
    QScopedPointer<HelicalFileTransferTask> m_fileTransferTask;

};

#endif // HELICALFILETRANSFERCONTROLLER_H
