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
        CSFTP::FileAttributes m_fileAttributes;
    };

public:
    explicit HelicalSFTPDialog(QtSSH &session, QWidget *parent = 0);
    ~HelicalSFTPDialog();

    void getCurrentDirectoryFiles(const QString &currentDirectory);
    void populateDirectory();

public slots:
    void fileDoubleClicked(QListWidgetItem * item);

private:
    Ui::HelicalSFTPDialog *ui;

    QString m_localFileSystemRoot {"/home/robt"};
    QString m_RemoteFileSystemRoot {"/home/pi"};
    QString m_currentDirectory {m_RemoteFileSystemRoot};

    QFileSystemModel *m_localFileSystemModel;
    QTreeView *m_localFileSystemView;
    QListWidget *m_remoteFileSystem;

    QScopedPointer<QtSFTP> m_sftp;

};

#endif // HELICALSFTPDIALOG_H
