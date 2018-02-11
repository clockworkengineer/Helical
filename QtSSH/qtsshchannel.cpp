/*
 * File:   qtsshchannels.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

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

#include "qtsshchannel.h"

//
// Overridden IOContext feedback methods.
//

/**
 * @brief QtSSHChannel::QtChannelIOContext::writeOutFn
 * @param data
 * @param size
 */
void QtSSHChannel::QtChannelIOContext::writeOutput(void *data, uint32_t size) {
    if (m_contextData) {
        QtSSHChannel *channel = static_cast<QtSSHChannel *> (m_contextData);
        if (size) {
            emit channel->writeStdOutput(QString::fromLocal8Bit(static_cast<char *>(data), size));
        }
    }
}

/**
 * @brief QtSSHChannel::QtChannelIOContext::writeErrFn
 * @param data
 * @param size
 */
void QtSSHChannel::QtChannelIOContext::writeError(void *data, uint32_t size)  {
    if (m_contextData) {
        QtSSHChannel *channel = static_cast<QtSSHChannel *> (m_contextData);
        if (size) {
            emit channel->writeStdError(QString::fromLocal8Bit(static_cast<char *>(data), size));
        }
    }
}

/**
 * @brief QtSSHChannel::QtSSHChannel
 *
 * Create channel object.
 *
 * @param session
 * @param parent
 */
QtSSHChannel::QtSSHChannel(QtSSH &session, QObject *parent) : QObject {parent},
    m_channel { new CSSHChannel(session.getSession()) }
{

}

/**
 * @brief QtSSHChannel::open
 *
 * Open an SSH channel over and existing SSH session.
 *
 */
void QtSSHChannel::open()
{

    try {
        m_channel->open();
        emit opened();
    }catch(const CSSHChannel::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

/**
 * @brief QtSSHChannel::close
 *
 * Close an open channel.
 *
 */
void QtSSHChannel::close()
{

    try {
        m_channel->close();
        emit closed();
    }catch(const CSSHChannel::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

/**
 * @brief QtSSHChannel::executeRemoteCommand
 *
 * Execute a command on remote server using an open channel.
 *
 * @param command
 */
void QtSSHChannel::executeRemoteCommand(const QString &command)
{

    QtChannelIOContext ioContext {this};

    try {
        executeCommand(*m_channel, command.toStdString(), ioContext);
    }catch(const CSSHChannel::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

/**
 * @brief QtSSHChannel::remoteShell
 *
 * Open a remote shell on server for the execution of commands. At present
 * it opens a vt100 terminal as thats what is currently supported by the
 * terminal sumaltion class cterminal.
 *
 * @param columns   - terminal columns
 * @param rows      - terminal rows
 */
void QtSSHChannel::remoteShell(int columns, int rows)
{

    QtChannelIOContext ioContext {this};

    try {
        interactiveShell(*m_channel,  "vt100", columns, rows, ioContext);
        emit remoteShellClosed();
    }catch(const CSSHChannel::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

/**
 * @brief QtSSHChannel::write
 *
 * Write data over a given channel to server.
 *
 * @param ioBuffer
 */
void QtSSHChannel::write(const QByteArray &ioBuffer)
{

    try {
        m_channel->write(static_cast<void*>(&ioBuffer.toStdString()[0]), ioBuffer.size());
    }catch(const CSSHChannel::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

/**
 * @brief QtSSHChannel::read
 *
 * Read data from server over a given channel.
 *
 * @param ioBuffer
 */
void QtSSHChannel::read(QByteArray &ioBuffer)
{

    try {
        auto bytesRead = m_channel->read(m_channel->getIoBuffer().get(), m_channel->getIoBufferSize());
        ioBuffer.clear();
        ioBuffer.resize(bytesRead);
        ioBuffer.append(m_channel->getIoBuffer().get(), bytesRead);
    }catch(const CSSHChannel::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

/**
 * @brief QtSSHChannel::sendEndOfFile
 *
 * Send end of file to remote server over given channel.
 *
 */
void QtSSHChannel::sendEndOfFile()
{
    m_channel->sendEndOfFile();
}

/**
 * @brief QtSSHChannel::isOpen
 *
 * @return == true if channelis open
 */
bool QtSSHChannel::isOpen()
{
    return(m_channel->isOpen());
}

/**
 * @brief QtSSHChannel::isClosed
 *
 * @return == true if channel closed.
 */
bool QtSSHChannel::isClosed()
{
    return(m_channel->isClosed());
}

/**
 * @brief QtSSHChannel::isEndOfFile
 *
 * @return == true if end of file has been sent over channel from server.
 */
bool QtSSHChannel::isEndOfFile()
{
    return(m_channel->isEndOfFile());
}

