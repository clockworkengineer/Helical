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
// Description: Class to implement SFTP file transfer task that talks to an SFTP server and
// executes SFTP commands (upload/download/delete) files. The task runs on a separate thread
// and commuicates with the main client using signal/slots.
//

// =============
// INCLUDE FILES
// =============

#include "helicalfiletransfertask.h"

/**
 * @brief HelicalFileTransferTask::HelicalFileTransferTask
 * @param parent
 */
HelicalFileTransferTask::HelicalFileTransferTask(QObject *parent) : QObject(parent)
{

}

/**
 * @brief HelicalFileTransferTask::openSession
 *
 * Open an SFTP session to transfer files.
 *
 * @param serverName    SSH server name
 * @param serverPort    SSH server port
 * @param userName      User name
 * @param userPassword  User password
 */
void HelicalFileTransferTask::openSession(const QString &serverName, const QString serverPort, const QString &userName, const QString &userPassword)
{

    // Create new session and set session details.

    m_session.reset(new QtSSH());

    m_session->setSessionDetails(serverName, serverPort, userName, userPassword);

    // Connect to server

    m_session->connectToServer();
    m_session->verifyServer();
    m_session->authorizeUser();

    // Once connected open up SFTP channel.

    if (m_session->isConnected() && m_session->isAuthorized()) {
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
 *
 * Close SFTP channel and disconnect its SSH session.
 *
 */
void HelicalFileTransferTask::closeSession()
{

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
 *
 * Process a given file: upload/download or delete.
 *
 * @param action            DELETE/UPLOAD/DOWNLOAD
 * @param sourceFile        Source file
 * @param destinationFile   Destination file
 */
void HelicalFileTransferTask::processFile(const FileTransferAction &fileTransaction)
{
    if (m_sftp) {

        QtSFTP::FileAttributes filAttributes;

        switch(fileTransaction.m_action) {

        case UPLOAD:
            m_sftp->putLocalFile(fileTransaction.m_sourceFile, fileTransaction.m_destinationFile);
            break;

        case DOWNLOAD:
            m_sftp->getRemoteFile(fileTransaction.m_sourceFile, fileTransaction.m_destinationFile);
            break;

        case DELETE:
            m_sftp->getFileAttributes(fileTransaction.m_sourceFile, filAttributes);
            if (m_sftp->isARegularFile(filAttributes)) {
                m_sftp->removeLink(fileTransaction.m_sourceFile);
            } else if (m_sftp->isADirectory(filAttributes)) {
                m_sftp->removeDirectory(fileTransaction.m_sourceFile);
            }
            break;

        }
    }
}

/**
 * @brief HelicalFileTransferTask::processDirectory
 *
 * Process a given directory: upload/download or delete. Recursively produce a list
 * of files in directory and queue in controller to be processed.
 *
 * @param action            DELETE/UPLOAD/DOWNLOAD
 * @param directoryPath     Directory to be processed
 * @param fileMappingPair   File mapper used to map between remote/local file paths.
 */
void HelicalFileTransferTask::processDirectory(const FileTransferAction &fileTransaction)
{
    if (m_sftp) {

        QtSFTP::FileMapper fileMapper {fileTransaction.m_fileMappingPair.first, fileTransaction.m_fileMappingPair.second};
        Antik::FileFeedBackFn fileFeedBackFn;
        Antik::FileList localFileList;

        switch(fileTransaction.m_action) {

        case UPLOAD:

            fileFeedBackFn = [this, &fileMapper] (const std::string &fileName)
            {
                emit queueFileForProcessing({ UPLOAD, QString::fromStdString(fileName),fileMapper.toRemote(QString::fromStdString(fileName))});
            };
            Antik::listLocalRecursive(fileTransaction.m_sourceFile.toStdString(), localFileList, fileFeedBackFn);
            break;


        case DOWNLOAD:

            fileFeedBackFn = [this, &fileMapper] (const std::string &fileName)
            {
                emit queueFileForProcessing({DOWNLOAD, QString::fromStdString(fileName), fileMapper.toLocal(QString::fromStdString(fileName))});
            };
            m_sftp->listRemoteDirectoryRecursive(fileTransaction.m_sourceFile, fileFeedBackFn);
            break;

        case DELETE:

            fileFeedBackFn = [this] (const std::string &fileName)
            {
                emit queueFileForProcessing({ DELETE, QString::fromStdString(fileName)} );
            };
            m_sftp->listRemoteDirectoryRecursive(fileTransaction.m_sourceFile, fileFeedBackFn);
            emit queueFileForProcessing({ DELETE, fileTransaction.m_sourceFile });

            break;

        }

        emit startFileProcessing();

    }
}

/**
 * @brief HelicalFileTransferTask::fileTaskThread
 *
 * Get file transfer task thread.
 *
 * @return
 */

QThread *HelicalFileTransferTask::fileTaskThread() const
{
    return m_fileTaskThread;
}

/**
 * @brief HelicalFileTransferTask::setFileTaskThread
 *
 * Set file transfer task thread.
 *
 * @param fileTaskThread
 */
void HelicalFileTransferTask::setFileTaskThread(QThread *fileTaskThread)
{
    m_fileTaskThread = fileTaskThread;
}

