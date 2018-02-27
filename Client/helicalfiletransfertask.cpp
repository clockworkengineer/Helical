/*
 * File:   helicalfiletransfertask.cpp
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

#include "helicalfiletransfertask.h"
#include <QDebug>

/**
 * @brief HelicalFileTransferTask::HelicalFileTransferTask
 * @param parent
 */
HelicalFileTransferTask::HelicalFileTransferTask(QObject *parent) : QObject(parent)
{

}

/**
 * @brief HelicalFileTransferTask::fileTaskThread
 * @return
 */

QThread *HelicalFileTransferTask::fileTaskThread() const
{
    return m_fileTaskThread;
}

/**
 * @brief HelicalFileTransferTask::setFileTaskThread
 * @param fileTaskThread
 */
void HelicalFileTransferTask::setFileTaskThread(QThread *fileTaskThread)
{
    m_fileTaskThread = fileTaskThread;
}

/**
 * @brief HelicalFileTransferTask::openSession
 * @param serverName
 * @param serverPort
 * @param userName
 * @param userPassword
 */
void HelicalFileTransferTask::openSession(const QString &serverName, const QString serverPort, const QString &userName, const QString &userPassword)
{
    m_session.reset(new QtSSH());

    m_session->setSessionDetails(serverName, serverPort, userName, userPassword);

    m_session->connectToServer();
    m_session->verifyServer();
    m_session->authorizeUser();

    if (m_session->isConnected() && m_session->isAuthorized()) {
        qDebug() << "CONNECTED";
        m_sftp.reset(new QtSFTP(*m_session));
        m_sftp->open();
        connect(m_sftp.data(), &QtSFTP::uploadFinished, this,  &HelicalFileTransferTask::uploadFinished);
        connect(m_sftp.data(), &QtSFTP::downloadFinished, this,  &HelicalFileTransferTask::downloadFinished);
        connect(m_sftp.data(), &QtSFTP::removedLink, this,  &HelicalFileTransferTask::deleteFileFinised);
        connect(m_sftp.data(), &QtSFTP::removedDirectory, this,  &HelicalFileTransferTask::deleteFileFinised);
        connect(m_sftp.data(), &QtSFTP::error, this,  &HelicalFileTransferTask::error);
    }
}

/**
 * @brief HelicalFileTransferTask::closeSession
 */
void HelicalFileTransferTask::closeSession()
{
    qDebug() << "CLOSEDOWN FILE TRANSFER TASK";

    if (m_sftp) {
        m_sftp->close();
        m_sftp.reset();
    }
    if (m_session) {
        m_session->disconnectFromServer();
        m_session.reset();
    }
    deleteLater();

}

/**
 * @brief HelicalFileTransferTask::uploadFile
 * @param sourceFile
 * @param destinationFile
 */
void HelicalFileTransferTask::uploadFile(const QString &sourceFile, const QString &destinationFile)
{
    qDebug() << "UPLOAD FILE " << sourceFile;
    if (m_sftp) {
        m_sftp->putLocalFile(sourceFile, destinationFile);
    }
}

void HelicalFileTransferTask::deleteFile(const QString &fileName)
{
    qDebug() << "DELETE FILE " << fileName;
    if (m_sftp) {
        QtSFTP::FileAttributes filAttributes;
        m_sftp->getFileAttributes(fileName, filAttributes);
        if (m_sftp->isARegularFile(filAttributes)) {
            m_sftp->removeLink(fileName);
        } else if (m_sftp->isADirectory(filAttributes)) {
            m_sftp->removeDirectory(fileName);
        }
    }
}

/**
 * @brief HelicalFileTransferTask::downloadFile
 * @param sourceFile
 * @param destinationFile
 */
void HelicalFileTransferTask::downloadFile(const QString &sourceFile, const QString &destinationFile)
{
    qDebug() << "DOWNLOAD FILE" << sourceFile;
    if (m_sftp) {
        m_sftp->getRemoteFile(sourceFile, destinationFile);
    }
}

/**
 * @brief HelicalFileTransferTask::uploadDirectory
 * @param directoryPath
 */
void HelicalFileTransferTask::uploadDirectory(const FileMappingPair &fileMappingPair, const QString &directoryPath)
{

    QtSFTP::FileMapper fileMapper {fileMappingPair.first, fileMappingPair.second};

    Antik::FileList localFileList;
    Antik::FileFeedBackFn localFileFeedBackFn = [this, &fileMapper]
            (const std::string &fileName) {
        emit queueFileForUpload(QString::fromStdString(fileName),fileMapper.toRemote(QString::fromStdString(fileName)));
    };

    Antik::listLocalRecursive(directoryPath.toStdString(), localFileList, localFileFeedBackFn);

    emit startUploading();

}

/**
 * @brief HelicalFileTransferTask::deleteDirectory
 * @param directoryPath
 */
void HelicalFileTransferTask::deleteDirectory(const QString &directoryPath)
{

    if (m_sftp) {
        Antik::FileList localFileList;
        Antik::FileFeedBackFn localFileFeedBackFn = [this]
                (const std::string &fileName) { emit queueFileForDelete(QString::fromStdString(fileName));};
        m_sftp->listRemoteDirectoryRecursive(directoryPath, localFileFeedBackFn);
        emit queueFileForDelete(directoryPath);
        emit startDeleting();

    }

}

/**
 * @brief HelicalFileTransferTask::downloadDirectory
 * @param directoryPath
 */
void HelicalFileTransferTask::downloadDirectory(const FileMappingPair &fileMappingPair, const QString &directoryPath)
{
    QtSFTP::FileMapper fileMapper {fileMappingPair.first, fileMappingPair.second};

    if (m_sftp) {
        Antik::FileFeedBackFn remoteFileFeedBackFn = [this, &fileMapper]
                (const std::string &fileName) {
            emit queueFileForDownload(QString::fromStdString(fileName), fileMapper.toLocal(QString::fromStdString(fileName)));
        };
        m_sftp->listRemoteDirectoryRecursive(directoryPath, remoteFileFeedBackFn);
        emit startDownloading();
    }

}
