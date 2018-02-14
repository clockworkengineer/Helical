#ifndef QTSFTP_H
#define QTSFTP_H

//
// Class: QtSFTP
//
// Description: Class for client SFTP to server connections. Its uses the Antik::SFTP C++
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

    bool isADirectory(const FileAttributes &fileAttributes);
     bool isARegularFile(const FileAttributes &fileAttributes);
     bool isASymbolicLink(const FileAttributes &fileAttributes);

     void getRemoteFile(const QString &sourceFile, const QString &destinationFile);
     void putFile(CSFTP &sftpServer, const std::string &sourceFile, QString &destinationFile);

    CSFTP *sftp() const;

signals:

    void error(const QString &errorMessage, int errorCode);
    void opened();
    void closed();

public slots:

private:
    CSFTP *m_sftp;

};

#endif // QTSFTP_H
