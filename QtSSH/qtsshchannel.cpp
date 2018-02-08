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
// Description:
//

// =============
// INCLUDE FILES
// =============

#include "qtsshchannel.h"

/**
 * @brief QtSSHChannel::QtSSHChannel
 * @param session
 * @param parent
 */
QtSSHChannel::QtSSHChannel(QtSSH &session, QObject *parent) : QObject {parent},
    m_channel { new CSSHChannel(session.getSession()) }
{

}

/**
 * @brief QtSSHChannel::open
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
 * @param columns
 * @param rows
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
 */
void QtSSHChannel::sendEndOfFile()
{
    m_channel->sendEndOfFile();
}

/**
 * @brief QtSSHChannel::isOpen
 * @return
 */
bool QtSSHChannel::isOpen()
{
    return(m_channel->isOpen());
}

/**
 * @brief QtSSHChannel::isClosed
 * @return
 */
bool QtSSHChannel::isClosed()
{
    return(m_channel->isClosed());
}

/**
 * @brief QtSSHChannel::isEndOfFile
 * @return
 */
bool QtSSHChannel::isEndOfFile()
{
    return(m_channel->isEndOfFile());
}

