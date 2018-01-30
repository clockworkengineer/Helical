#include "qtsshchannel.h"
#include <QDebug>

QtSSHChannel::QtSSHChannel(QtSSH &session, QObject *parent) : QObject {parent},
    m_channel { new CSSHChannel(session.getSession()) }
{

}

void QtSSHChannel::open()
{

    try {
        m_channel->open();
        emit opened();
    }catch(const CSSHChannel::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

void QtSSHChannel::close()
{

    try {
        m_channel->close();
        emit closed();
    }catch(const CSSHChannel::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

void QtSSHChannel::executeRemoteCommand(const QString &command)
{

    QtChannelIOContext writeContext {this};

    try {
        executeCommand(*m_channel, command.toStdString(), writeContext);
    }catch(const CSSHChannel::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

void QtSSHChannel::remoteShell(int columns, int rows)
{

    QtChannelIOContext writeContext {this};

    try {
        interactiveShell(*m_channel,  columns, rows, writeContext);
        emit remoteShellClosed();
    }catch(const CSSHChannel::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

void QtSSHChannel::write(const QByteArray &ioBuffer)
{

    try {
        m_channel->write(static_cast<void*>(&ioBuffer.toStdString()[0]), ioBuffer.size());
    }catch(const CSSHChannel::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

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

void QtSSHChannel::sendEndOfFile()
{
    m_channel->sendEndOfFile();
}

bool QtSSHChannel::isOpened()
{
    return(m_channel->isOpen());
}

bool QtSSHChannel::isClosed()
{
    return(m_channel->isClosed());
}

bool QtSSHChannel::isEndOfFile()
{
    return(m_channel->isEndOfFile());
}

