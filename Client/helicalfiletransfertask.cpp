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
 * @brief HelicalFileTransferTask::processFile
 * @param action
 * @param sourceFile
 * @param destinationFile
 */
void HelicalFileTransferTask::processFile(FileAction action, const QString &sourceFile, const QString &destinationFile)
{
    if (m_sftp) {

        QtSFTP::FileAttributes filAttributes;

        switch(action) {

        case UPLOAD:
            qDebug() << "UPLOAD FILE " << sourceFile;
            m_sftp->putLocalFile(sourceFile, destinationFile);
            break;

        case DOWNLOAD:
            qDebug() << "DOWNLOAD FILE" << sourceFile;
            m_sftp->getRemoteFile(sourceFile, destinationFile);
            break;

        case DELETE:
            qDebug() << "DELETE FILE " << sourceFile;
            m_sftp->getFileAttributes(sourceFile, filAttributes);
            if (m_sftp->isARegularFile(filAttributes)) {
                m_sftp->removeLink(sourceFile);
            } else if (m_sftp->isADirectory(filAttributes)) {
                m_sftp->removeDirectory(sourceFile);
            }
            break;

        }
    }
}

/**
 * @brief HelicalFileTransferTask::processDirectory
 * @param action
 * @param directoryPath
 * @param fileMappingPair
 */
void HelicalFileTransferTask::processDirectory(FileAction action, const QString &directoryPath, const FileMappingPair &fileMappingPair)
{
    if (m_sftp) {

        QtSFTP::FileMapper fileMapper {fileMappingPair.first, fileMappingPair.second};
        Antik::FileFeedBackFn fileFeedBackFn;
        Antik::FileList localFileList;

        switch(action) {

        case UPLOAD:


            fileFeedBackFn = [this, &fileMapper] (const std::string &fileName)
            {
                emit queueFileForProcessing(UPLOAD, QString::fromStdString(fileName),fileMapper.toRemote(QString::fromStdString(fileName)));
            };

            Antik::listLocalRecursive(directoryPath.toStdString(), localFileList, fileFeedBackFn);

            emit startFileProcessing(UPLOAD);

            break;


        case DOWNLOAD:

            fileFeedBackFn = [this, &fileMapper] (const std::string &fileName)
            {
                emit queueFileForProcessing(DOWNLOAD, QString::fromStdString(fileName), fileMapper.toLocal(QString::fromStdString(fileName)));
            };
            m_sftp->listRemoteDirectoryRecursive(directoryPath, fileFeedBackFn);
            emit startFileProcessing(DOWNLOAD);

            break;

        case DELETE:

            fileFeedBackFn = [this] (const std::string &fileName)
            {
                emit queueFileForProcessing(DELETE, QString::fromStdString(fileName));
            };
            m_sftp->listRemoteDirectoryRecursive(directoryPath, fileFeedBackFn);
            emit queueFileForProcessing(DELETE, directoryPath);
            emit startFileProcessing(DELETE);

            break;

        }
    }
}


