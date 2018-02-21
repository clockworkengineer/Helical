/*
 * File:   helicalsftpdialog.h
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

#ifndef HELICALSFTPDIALOG_H
#define HELICALSFTPDIALOG_H

//
// Class: HelicalSFTPDialog
//
// Description:
//

// =============
// INCLUDE FILES
// =============

#include <QDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QListWidget>
#include <QScopedPointer>
#include <QDebug>
#include "QtSSH/qtsftp.h"
#include "helicalfiletransfertask.h"

// =================
// CLASS DECLARATION
// =================

namespace Ui {
class HelicalSFTPDialog;
}

class HelicalSFTPDialog : public QDialog
{
    Q_OBJECT

    class HelicalFileItem : public QListWidgetItem {
    public:
        HelicalFileItem(const QString &name) : QListWidgetItem(name) {}
        QtSFTP::FileAttributes m_fileAttributes;
    };

public:
    explicit HelicalSFTPDialog(QtSSH &session, const QString &remoteUserHome,  const QString &localUserHome, QWidget *parent = 0);
    ~HelicalSFTPDialog();

    void updateRemoteFileList(const QString &currentDirectory);
    void createFileTransferTask(QtSSH &session);
    void destroyFileTransferTask();

signals:
    void openSession(const QString &serverName, const QString serverPort, const QString &userName, const QString &userPassword);
    void closeSession();
    void uploadFile(const QString &sourceFile, const QString &destinationFile);
    void downloadFile(const QString &sourceFile, const QString &destinationFile);

private slots:
    void fileDoubleClicked(QListWidgetItem * item);
    void localFileViewClicked(const QModelIndex &index);
    void showRemoteFileContextMenu(const QPoint &pos);
    void showLocalFileContextMenu(const QPoint &pos);
    void error(const QString &errorMessage, int errorCode);
    void uploadFinished(const QString &sourceFile, const QString &destinationFile);
    void downloadFinished(const QString &sourceFile, const QString &destinationFile);
    void fileDeleted(const QString &filePath);


private:
    Ui::HelicalSFTPDialog *ui;

    QString m_remoteFileSystemRoot;
    QString m_localFileSystemRoot;
    QString m_currentRemoteDirectory {m_remoteFileSystemRoot};
    QString m_currentLocalDirectory { m_localFileSystemRoot};

    QFileSystemModel *m_localFileSystemModel;
    QTreeView *m_localFileSystemView;
    QListWidget *m_remoteFileSystemList;

    QScopedPointer<QtSFTP> m_sftp;
    QScopedPointer<HelicalFileTransferTask> m_fileTransferTask;

};

#endif // HELICALSFTPDIALOG_H
