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

    // Remote file system custom list wdiget

    class HelicalRemoteFileItem : public QListWidgetItem {
    public:
        HelicalRemoteFileItem(const QString &name) : QListWidgetItem(name) {}
        QtSFTP::FileAttributes m_fileAttributes;    // File attributes
        QString m_remoteFilePath;                   // File full remote path
    };

public:

    // Constructor / destructor

    explicit HelicalSFTPDialog(QtSSH &session, const QString &remoteUserHome,  const QString &localUserHome, QWidget *parent = 0);
    ~HelicalSFTPDialog();

signals:

      // File transaction processing

    void queueFileTransaction(const FileTransferAction &fileTransaction);

public slots:
    void error(const QString &errorTransactionMessage, int errorCode, quint64 transactionID);
    void statusMessage(const QString &message);
    void finishedTransactionMessage(const QString &message);
    void errorTransactionMessage(const QString &message);
    void updateTransactionTotals(qint64 queuedTransactions, qint64 inProgressTransactions,  qint64 errorTransactions);
    void updateRemoteFileList();

private slots:

    // UI event handling

    void remoteFileClicked(QListWidgetItem *item);
    void remoteFileDoubleClicked(QListWidgetItem * item);
    void localDirectoryViewClicked(const QModelIndex &index);
    void localDirectoryViewDoubleClicked(const QModelIndex &index);
    void localFileViewClicked(const QModelIndex &index);
    void localFileViewDoubleClicked(const QModelIndex &index);

    // Context menu handling

    void showRemoteFileContextMenu(const QPoint &pos);
    void showlocalDirectoryViewContextMenu(const QPoint &pos);
    void showLocalFileViewContextMenu(const QPoint &pos);

    // UI command processing

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

    void startupControllers(QtSSH &session);
    void terminateControllers();

    // Qt dialog data

    Ui::HelicalSFTPDialog *ui;

    // Local/remote system roots and current working directory

    QString m_remoteSystemRoot;
    QString m_localSystemRoot;
    QString m_currentRemoteDirectory {m_remoteSystemRoot};
    QString m_currentLocalDirectory { m_localSystemRoot};

    // Local file/folder views and model

    QFileSystemModel *m_localDirectorysModel;
    QTreeView *m_localDirectorysView;
    QListView *m_localFilesView;
    QFileSystemModel *m_localFilesModel;

    // Remote file system widget list

    QListWidget *m_remoteFileSystemList;

    // SFTP connection

    QScopedPointer<QtSFTP> m_sftp;

    // Local/Remote file mapper

    QScopedPointer<QtSFTP::FileMapper> m_fileMapper;

    // Controllers for file transfer tasks

    QVector<HelicalFileTransferController*> m_helicalTransferController;

};

#endif // HELICALSFTPDIALOG_H
