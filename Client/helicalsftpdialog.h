#ifndef HELICALSFTPDIALOG_H
#define HELICALSFTPDIALOG_H

#include <QDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QListWidget>
#include <QScopedPointer>
#include <QDebug>
#include "QtSSH/qtsftp.h"

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
    explicit HelicalSFTPDialog(QtSSH &session, const QString &userHome, QWidget *parent = 0);
    ~HelicalSFTPDialog();

    void updateRemoteFileList(const QString &currentDirectory);

public slots:
    void fileDoubleClicked(QListWidgetItem * item);
    void localFileViewClicked(const QModelIndex &index);
    void showRemoteFileContextMenu(const QPoint &pos);
    void showLocalFileContextMenu(const QPoint &pos);

private:
    Ui::HelicalSFTPDialog *ui;

    QString m_localFileSystemRoot {"/home/robt"};
    QString m_RemoteFileSystemRoot;
    QString m_currentRemoteDirectory {m_RemoteFileSystemRoot};
    QString m_currentLocalDirectory {m_localFileSystemRoot};

    QFileSystemModel *m_localFileSystemModel;
    QTreeView *m_localFileSystemView;
    QListWidget *m_remoteFileSystemList;

    QScopedPointer<QtSFTP> m_sftp;

};

#endif // HELICALSFTPDIALOG_H
