/*
 * File:   qtsftp.h
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

#ifndef QTSFTP_H
#define QTSFTP_H

//
// Class: QtSFTP
//
// Description: Class for client SSH SFTP to server connections. Its uses the Antik::SFTP C++
// wrapper classes for third party library libssh. Its translates to/from Qt to standard
// C++ data structures as and when needed to keep the whole interface Qt orientated.
//

// =============
// INCLUDE FILES
// =============

#include <QObject>

#include "QtSSH/qtssh.h"
#include "SFTPUtil.hpp"

// =================
// CLASS DECLARATION
// =================

using namespace Antik::SSH;

class QtSFTP : public QObject
{
    Q_OBJECT
public:

    class FileMapper {
    public:
        FileMapper(const QString &localDirectory, const QString &remoteDirectory) :
            m_mapper {Antik::FileMapper(localDirectory.toStdString(),remoteDirectory.toStdString())} {}

        QString toLocal(const QString &filePath)
        {
            return(QString::fromStdString(m_mapper.toLocal(filePath.toStdString())));
        }

        QString toRemote(const QString &filePath)
        {
            return(QString::fromStdString(m_mapper.toRemote(filePath.toStdString())));
        }

    private:
        Antik::FileMapper m_mapper;

    };

    typedef CSFTP::Directory Directory;
    typedef CSFTP::FileAttributes FileAttributes;

    // Constructor

    explicit QtSFTP(QtSSH &session, QObject *parent = nullptr);

    void open();
    void close();

    CSFTP::Directory openDirectory(const QString &directoryPath);
    bool readDirectory(const Directory &directoryHandle, FileAttributes &fileAttributes);
    bool endOfDirectory(const Directory &directoryHandle);
    void closeDirectory(Directory &directoryHandle);

    void removeLink(const QString &filePath);

    bool isADirectory(const FileAttributes &fileAttributes);
    bool isARegularFile(const FileAttributes &fileAttributes);
    bool isASymbolicLink(const FileAttributes &fileAttributes);

    void getRemoteFile(const QString &sourceFile, const QString &destinationFile);
    void putLocalFile(const QString &sourceFile, const QString &destinationFile);

    void listRemoteDirectoryRecursive(const QString &directoryPath);

    CSFTP *sftp() const;

signals:

    void error(const QString &errorMessage, int errorCode);
    void opened();
    void closed();
    void uploadFinished(const QString &sourceFile, const QString &destinationFile);
    void downloadFinished(const QString &sourceFile, const QString &destinationFile);
    void removedLink(const QString &filePath);

    void listedRemoteFileName(const QString &fileName);

public slots:

private:
    CSFTP *m_sftp;

};

#endif // QTSFTP_H
