/*
 * File:   qtssh.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

//
// Class: QtSFTP
//
// Description: Class for client SSH SFTP server connections. Its uses the Antik::CSSH C++
// wrapper classes for third party library libssh. Its translates to/from Qt to standard
// C++ data structures as and when needed to keep the whole interface Qt orientated.
//

// =============
// INCLUDE FILES
// =============

#include "qtsftp.h"

#include <functional>

/**
 * @brief QtSFTP::QtSFTP
 * @param session
 * @param parent
 */
QtSFTP::QtSFTP(QtSSH &session, QObject *parent) : QObject {parent},
    m_sftp { new CSFTP (session.getSession()) }
{

}

/**
 * @brief QtSFTP::open
 */
void QtSFTP::open()
{

    try {
        m_sftp->open();
        emit opened();
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

/**
 * @brief QtSFTP::close
 */
void QtSFTP::close()
{

    try {
        m_sftp->close();
        emit closed();
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

/**
 * @brief QtSFTP::openDirectory
 * @param directoryPath
 * @return
 */
CSFTP::Directory QtSFTP::openDirectory(const QString &directoryPath)
{
    try {
        return(m_sftp->openDirectory(directoryPath.toStdString()));
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

    return(CSFTP::Directory());
}

/**
 * @brief QtSFTP::readDirectory
 * @param directoryHandle
 * @param fileAttributes
 * @return
 */
bool QtSFTP::readDirectory(const QtSFTP::Directory &directoryHandle, QtSFTP::FileAttributes &fileAttributes)
{
    try {
        return(m_sftp->readDirectory(directoryHandle,fileAttributes ));
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

    return(false);
}

/**
 * @brief QtSFTP::endOfDirectory
 * @param directoryHandle
 * @return
 */
bool QtSFTP::endOfDirectory(const QtSFTP::Directory &directoryHandle)
{
    try {
        return(m_sftp->endOfDirectory(directoryHandle));
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }


    return(false);
}

/**
 * @brief QtSFTP::closeDirectory
 * @param directoryHandle
 */
void QtSFTP::closeDirectory(QtSFTP::Directory &directoryHandle)
{
    try {
        m_sftp->closeDirectory(directoryHandle);
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

/**
 * @brief QtSFTP::removeLink
 * @param filePath
 */
void QtSFTP::removeLink(const QString &filePath)
{
    try {
        m_sftp->removeLink(filePath.toStdString());
        emit removedLink(filePath);
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }
}

/**
 * @brief QtSFTP::isADirectory
 * @param fileAttributes
 * @return
 */
bool QtSFTP::isADirectory(const QtSFTP::FileAttributes &fileAttributes)
{

    try {
        return(m_sftp->isADirectory(fileAttributes));
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

    return(false);
}

/**
 * @brief QtSFTP::isARegularFile
 * @param fileAttributes
 * @return
 */
bool QtSFTP::isARegularFile(const QtSFTP::FileAttributes &fileAttributes)
{
    try {
        return(m_sftp->isARegularFile(fileAttributes));
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

    return(false);
}

/**
 * @brief QtSFTP::isASymbolicLink
 * @param fileAttributes
 * @return
 */
bool QtSFTP::isASymbolicLink(const QtSFTP::FileAttributes &fileAttributes)
{
    try {
        return(m_sftp->isASymbolicLink(fileAttributes));
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

    return(false);
}

/**
 * @brief QtSFTP::getRemoteFile
 * @param sourceFile
 * @param destinationFile
 */
void QtSFTP::getRemoteFile(const QString &sourceFile, const QString &destinationFile)
{
    try {
        getFile(*m_sftp, sourceFile.toStdString(), destinationFile.toStdString());
        emit downloadFinished(sourceFile, destinationFile);
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }
}

/**
 * @brief QtSFTP::putLocalFile
 * @param sourceFile
 * @param destinationFile
 */
void QtSFTP::putLocalFile(const QString &sourceFile, const QString &destinationFile)
{
    try {
        putFile(*m_sftp, sourceFile.toStdString(), destinationFile.toStdString());
        emit uploadFinished(sourceFile, destinationFile);
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }
}

void QtSFTP::listRemoteDirectoryRecursive(const QString &directoryPath)
{
    try {

        Antik::FileList remoteFiles;
        Antik::RemoteFileListFn remoteFileNameFeedback = [this]
                (const std::string &fileName) { emit listedRemoteFileName(QString::fromStdString(fileName));};

        listRemoteRecursive(*m_sftp, directoryPath.toStdString(), remoteFiles, remoteFileNameFeedback);

    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

/**
 * @brief QtSFTP::sftp
 * @return
 */
CSFTP *QtSFTP::sftp() const
{
    return m_sftp;
}
