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
#include <QFileIconProvider>
#include <QMenu>
#include <QDesktopServices>

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

    typedef std::pair<QString, QString> FileTransferPair;

    class HelicalFileItem : public QListWidgetItem {
    public:
        HelicalFileItem(const QString &name) : QListWidgetItem(name) {}
        QtSFTP::FileAttributes m_fileAttributes;
        QString m_remoteFilePath;
    };

public:
    explicit HelicalSFTPDialog(QtSSH &session, const QString &remoteUserHome,  const QString &localUserHome, QWidget *parent = 0);
    ~HelicalSFTPDialog();

    void updateRemoteFileList(const QString &currentDirectory);
    void createFileTransferTask(QtSSH &session);
    void destroyFileTransferTask();
    void statusMessage(const QString &message);

signals:
    void openSession(const QString &serverName, const QString serverPort, const QString &userName, const QString &userPassword);
    void closeSession();
    void uploadFile(const QString &sourceFile, const QString &destinationFile);
    void downloadFile(const QString &sourceFile, const QString &destinationFile);
    void listRemoteDirectoryRecursive(const QString &directoryPath);
    void listedRemoteFileName(const QString &fileName);
    void listLocalDirectoryRecursive(const QString &directoryPath);

    public slots:

private slots:
    void remoteFileClicked(QListWidgetItem *item);
    void remoteFileDoubleClicked(QListWidgetItem * item);
    void localFileSystemViewClicked(const QModelIndex &index);
    void localFileSystemViewDoubleClicked(const QModelIndex &index);
    void localFileViewClicked(const QModelIndex &index);
    void localFileViewDoubleClicked(const QModelIndex &index);

    void showRemoteFileContextMenu(const QPoint &pos);
    void showLocalFileFiewSystemContextMenu(const QPoint &pos);
    void showLocalFileViewContextMenu(const QPoint &pos);
    void error(const QString &errorMessage, int errorCode);
    void uploadFinished(const QString &sourceFile, const QString &destinationFile);
    void downloadFinished(const QString &sourceFile, const QString &destinationFile);
    void fileDeleted(const QString &filePath);

    void viewSelectedFiles();
    void downloadSelectedFile();
    void deleteSelectedFiles();
    void enterSelectedDirectory();
    void refreshSelectedDirectory();
    void uploadSelectedFolder();
    void uploadSelectedFiles();
    void queueFileForDownload(const QString &fileName);
    void queueFileForUpload(const QString &fileName);
    void downloadNextFile();
    void uploadNextFile();

protected:

    // Override for dialog close event

    void closeEvent(QCloseEvent *event) override;

private:
    Ui::HelicalSFTPDialog *ui;

    QString m_remoteFileSystemRoot;
    QString m_localFileSystemRoot;
    QString m_currentRemoteDirectory {m_remoteFileSystemRoot};
    QString m_currentLocalDirectory { m_localFileSystemRoot};

    QFileSystemModel *m_localFileSystemModel;
    QTreeView *m_localFileSystemView;
    QListView *m_localFileView;
    QFileSystemModel *m_localFileModel;
    QListWidget *m_remoteFileSystemList;

    QScopedPointer<QtSFTP> m_sftp;
    QScopedPointer<HelicalFileTransferTask> m_fileTransferTask;

    QScopedPointer<QtSFTP::FileMapper> m_fileMapper;

    QList<FileTransferPair> m_downloadQueue;
    QList<FileTransferPair> m_uploadQueue;

};

#endif // HELICALSFTPDIALOG_H
