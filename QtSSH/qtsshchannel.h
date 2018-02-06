#ifndef QTSSHCHANNEL_H
#define QTSSHCHANNEL_H

#include <QObject>

#include "QtSSH/qtssh.h"
#include "SSHChannelUtil.hpp"

using namespace Antik::SSH;

class QtSSHChannel : public QObject
{
    Q_OBJECT

    class QtChannelIOContext : public IOContext {
    public:
        QtChannelIOContext(void *context) : IOContext {context} { m_internalInput = false;}
        virtual void writeOutFn(void *data, uint32_t size) {
            if (m_contextData) {
                QtSSHChannel *channel = static_cast<QtSSHChannel *> (m_contextData);
                if (size) {
                    emit channel->writeStdOut(QString::fromLocal8Bit(static_cast<char *>(data), size));
                }
            }
        }
        virtual void writeErrFn(void *data, uint32_t size) {
            if (m_contextData) {
                QtSSHChannel *channel = static_cast<QtSSHChannel *> (m_contextData);
                if (size) {
                    emit channel->writeStdErr(QString::fromLocal8Bit(static_cast<char *>(data), size));
                }
            }
        }
    };

public:    

    explicit QtSSHChannel(QtSSH &session, QObject *parent = nullptr);

    void open();
    void close();

    void executeRemoteCommand(const QString &command);
    void remoteShell(int columns, int rows);

    void write(const QByteArray &ioBuffer);
    void read(QByteArray &ioBuffer);
    void sendEndOfFile();

    bool isOpen();
    bool isClosed();
    bool isEndOfFile();

signals:

    void error(const QString &errorMessage, int errorCode);
    void opened();
    void closed();
    void remoteShellClosed();
    void writeStdOut(const QString &text);
    void writeStdErr(const QString &text);

public slots:

private:

    CSSHChannel *m_channel;

};

#endif // QTSSHCHANNEL_H
