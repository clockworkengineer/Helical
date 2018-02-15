#include "qtsftp.h"

QtSFTP::QtSFTP(QtSSH &session, QObject *parent) : QObject {parent},
    m_sftp { new CSFTP (session.getSession()) }
{

}

void QtSFTP::open()
{

    try {
        m_sftp->open();
        emit opened();
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

void QtSFTP::close()
{

    try {
        m_sftp->close();
        emit closed();
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

CSFTP::Directory QtSFTP::openDirectory(const QString &directoryPath)
{
    try {
        return(m_sftp->openDirectory(directoryPath.toStdString()));
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

    return(CSFTP::Directory());
}

bool QtSFTP::readDirectory(const QtSFTP::Directory &directoryHandle, QtSFTP::FileAttributes &fileAttributes)
{
    try {
        return(m_sftp->readDirectory(directoryHandle,fileAttributes ));
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

    return(false);
}

bool QtSFTP::endOfDirectory(const QtSFTP::Directory &directoryHandle)
{
    try {
        return(m_sftp->endOfDirectory(directoryHandle));
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }


    return(false);
}

void QtSFTP::closeDirectory(QtSFTP::Directory &directoryHandle)
{
    try {
        m_sftp->closeDirectory(directoryHandle);
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

bool QtSFTP::isADirectory(const QtSFTP::FileAttributes &fileAttributes)
{

    try {
        return(m_sftp->isADirectory(fileAttributes));
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

    return(false);
}

bool QtSFTP::isARegularFile(const QtSFTP::FileAttributes &fileAttributes)
{
    try {
        return(m_sftp->isARegularFile(fileAttributes));
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

    return(false);
}

bool QtSFTP::isASymbolicLink(const QtSFTP::FileAttributes &fileAttributes)
{
    try {
        return(m_sftp->isASymbolicLink(fileAttributes));
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

    return(false);
}

void QtSFTP::getRemoteFile(const QString &sourceFile, const QString &destinationFile)
{
    try {
        getFile(*m_sftp, sourceFile.toStdString(), destinationFile.toStdString());
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }
}

void QtSFTP::putLocalFile(const QString &sourceFile, QString &destinationFile)
{
    try {
        putFile(*m_sftp, sourceFile.toStdString(), destinationFile.toStdString());
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }
}

CSFTP *QtSFTP::sftp() const
{
    return m_sftp;
}
