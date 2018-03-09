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
// Description: Class to create/display an SFTP session dialog(window) for the viewing of remote
// files and their upload/download/deletion.
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

#include "helical.h"
#include "QtSSH/qtsftp.h"

#include "helicalfiletransfertask.h"
#include "helicalfiletransfercontroller.h"

// =================
// CLASS DECLARATION
// =================

namespace Ui {
class HelicalSFTPDialog;
}

class HelicalSFTPDialog : public QDialog
{
    Q_OBJECT

    class HelicalRemoteFileItem : public QListWidgetItem {
    public:
        HelicalRemoteFileItem(const QString &name) : QListWidgetItem(name) {}
        QtSFTP::FileAttributes m_fileAttributes;
        QString m_remoteFilePath;
    };

public:
    explicit HelicalSFTPDialog(QtSSH &session, const QString &remoteUserHome,  const QString &localUserHome, QWidget *parent = 0);
    ~HelicalSFTPDialog();

signals:
    void openSession(const QString &serverName, const QString serverPort, const QString &userName, const QString &userPassword);
    void closeSession();
    void processFile(const FileTransferAction &fileTransaction);
    void processDirectory(const FileTransferAction &fileTransaction);
    void queueFileForProcessing(const FileTransferAction &fileTransaction);
    void processNextFile();

public slots:
    void error(const QString &errorTransactionMessage, int errorCode, quint64 transactionID);
    void statusMessage(const QString &message);
    void finishedTransactionMessage(const QString &message);
    void errorTransactionMessage(const QString &message);
    void updateRemoteFileList();

private slots:
    void remoteFileClicked(QListWidgetItem *item);
    void remoteFileDoubleClicked(QListWidgetItem * item);
    void localDirectoryViewClicked(const QModelIndex &index);
    void localDirectoryViewDoubleClicked(const QModelIndex &index);
    void localFileViewClicked(const QModelIndex &index);
    void localFileViewDoubleClicked(const QModelIndex &index);

    void showRemoteFileContextMenu(const QPoint &pos);
    void showlocalDirectoryViewContextMenu(const QPoint &pos);
    void showLocalFileViewContextMenu(const QPoint &pos);

    void viewSelectedFiles();
    void downloadSelectedFile();
    void deleteSelectedFiles();
    void enterSelectedDirectory();
    void refreshSelectedDirectory();
    void uploadSelectedDirectory();
    void uploadSelectedFiles();


protected:

    // Override for dialog close event

    void closeEvent(QCloseEvent *event) override;

private:
    Ui::HelicalSFTPDialog *ui;

    QString m_remoteSystemRoot;
    QString m_localSystemRoot;
    QString m_currentRemoteDirectory {m_remoteSystemRoot};
    QString m_currentLocalDirectory { m_localSystemRoot};

    QFileSystemModel *m_localDirectorysModel;
    QTreeView *m_localDirectorysView;
    QListView *m_localFilesView;
    QFileSystemModel *m_localFilesModel;
    QListWidget *m_remoteFileSystemList;

    QScopedPointer<QtSFTP> m_sftp;

    QScopedPointer<QtSFTP::FileMapper> m_fileMapper;

    HelicalFileTransferController m_helicalTransferController;

};

#endif // HELICALSFTPDIALOG_H
