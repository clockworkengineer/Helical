/*
 * File:   qtsshchannel.h
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

#ifndef QTSSHCHANNEL_H
#define QTSSHCHANNEL_H

//
// Class: QtSSHChannel
//
// Description: Class for client SSH channel connections. Its uses the Antik::CSSH C++
// wrapper classes for third party library libssh. Its translates to/from Qt to standard
// C++ data structures as and when needed to keep the whole interface Qt orientated.
//

// =============
// INCLUDE FILES
// =============

#include <QObject>

#include "QtSSH/qtssh.h"
#include "SSHChannelUtil.hpp"

// =================
// CLASS DECLARATION
// =================

using namespace Antik::SSH;

class QtSSHChannel : public QObject
{
    Q_OBJECT

    // Qt context for command/shell IO feedback

    class QtChannelIOContext : public IOContext {
    public:
        QtChannelIOContext(void *context) : IOContext {context} { m_internalInput = false;}
        virtual void writeOutput(void *data, uint32_t size) final;
        virtual void writeError(void *data, uint32_t size) final;
    };

public:    

    // Constructor

    explicit QtSSHChannel(QtSSH &session, QObject *parent = nullptr);

    // Open/close channel

    void open();
    void close();

    // Execute remote command/ create remote shell

    void executeRemoteCommand(const QString &command);
    void remoteShell(int columns, int rows);

    // Read/write channel data

    void write(const QByteArray &ioBuffer);
    void read(QByteArray &ioBuffer);

    // Send end of file to server

    void sendEndOfFile();

    // Check channel status

    bool isOpen();
    bool isClosed();
    bool isEndOfFile();

signals:

    void error(const QString &errorMessage, int errorCode); // Channel error
    void opened();                                          // Channel opened
    void closed();                                          // Channel closed

    void remoteShellClosed();                               // Remote shell has been closed

    void writeStdOutput(const QString &text);               // Write standard output text
    void writeStdError(const QString &text);                // Write standard error text

public slots:

private:

    // Channel object

    CSSHChannel *m_channel;

};

#endif // QTSSHCHANNEL_H
