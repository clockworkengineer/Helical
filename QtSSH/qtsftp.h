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

    // typedef std::function<void(const QString &, const QString &)> FileCompletionFn;

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
    void putLocalFile(const QString &sourceFile, QString &destinationFile);

    CSFTP *sftp() const;

signals:

    void error(const QString &errorMessage, int errorCode);
    void opened();
    void closed();
    void uploadFinished(const QString &sourceFile, const QString &destinationFile);
    void downloadFinished(const QString &sourceFile, const QString &destinationFile);
    void removedLink(const QString &filePath);

public slots:

private:
    CSFTP *m_sftp;

};

#endif // QTSFTP_H
