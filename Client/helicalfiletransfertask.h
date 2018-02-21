#ifndef HELICALFILETRANSFERTASK_H
#define HELICALFILETRANSFERTASK_H

#include <QObject>
#include <QThread>
#include <QScopedPointer>

#include "QtSSH/qtssh.h"
#include "QtSSH/qtsftp.h"

class HelicalFileTransferTask : public QObject
{
    Q_OBJECT
public:

    // Class exception

    struct Exception : public std::runtime_error {

        Exception(const QString & messageStr)
            : std::runtime_error(static_cast<QString>("HelicalFileTransferTask Failure: " + messageStr).toStdString()) {
        }

    };

    explicit HelicalFileTransferTask(QObject *parent = nullptr);

    QThread *fileTaskThread() const;
    void setFileTaskThread(QThread *fileTaskThread);

signals:
    void uploadFinished(const QString &sourceFile, const QString &destinationFile);
    void downloadFinished(const QString &sourceFile, const QString &destinationFile);

public slots:
    void openSession(const QString &serverName, const QString serverPort, const QString &userName, const QString &userPassword);
    void closeSession();
    void uploadFile(const QString &sourceFile, const QString &destinationFile);
    void downloadFile(const QString &sourceFile, const QString &destinationFile);

private:
    QThread *m_fileTaskThread;
    QScopedPointer<QtSSH> m_session;
    QScopedPointer<QtSFTP> m_sftp;
};

#endif // HELICALFILETRANSFERTASK_H
