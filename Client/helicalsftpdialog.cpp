#include "helicalsftpdialog.h"
#include "ui_helicalsftpdialog.h"

#include <QSplitter>
#include <QVBoxLayout>
#include <QListWidget>
#include <QTableWidget>
#include <QSizePolicy>
#include <QFileIconProvider>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>

HelicalSFTPDialog::HelicalSFTPDialog(QtSSH &session, const QString &userHome, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelicalSFTPDialog),
    m_RemoteFileSystemRoot {userHome}
{

    ui->setupUi(this);

    ui->localLineEdit->setReadOnly(true);
    ui->localLineEdit->setText(m_currentLocalDirectory);

    ui->remoteLineEdit->setReadOnly(true);
    ui->remoteLineEdit->setText(m_RemoteFileSystemRoot);

    QSplitter *splitter = new QSplitter(this);

    m_localFileSystemModel = new QFileSystemModel(this);
    m_localFileSystemView  = new QTreeView(this);
    m_localFileSystemView->setModel(m_localFileSystemModel);
    m_localFileSystemModel->setRootPath(m_localFileSystemRoot);
    m_localFileSystemView->setRootIndex(m_localFileSystemModel->index(m_localFileSystemRoot));
    m_localFileSystemView->setContextMenuPolicy(Qt::CustomContextMenu);

    m_remoteFileSystemList = new QListWidget(this);
    m_remoteFileSystemList->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->fileViewFrame->setLayout(new QVBoxLayout(this));
    splitter = new QSplitter(this);
    splitter->addWidget(m_localFileSystemView);
    splitter->addWidget(m_remoteFileSystemList);

    ui->fileViewFrame->layout()->addWidget(splitter);

    adjustSize();

    m_sftp.reset(new QtSFTP(session));

    m_sftp->open();

    updateRemoteFileList(m_currentRemoteDirectory);

    connect(m_remoteFileSystemList, &QListWidget::customContextMenuRequested, this, &HelicalSFTPDialog::showRemoteFileContextMenu);
    connect(m_remoteFileSystemList, &QListWidget::itemDoubleClicked, this, &HelicalSFTPDialog::fileDoubleClicked);
    connect(m_localFileSystemView, &QTreeView::clicked, this, &HelicalSFTPDialog::localFileViewClicked);
    connect(m_localFileSystemView, &QTreeView::customContextMenuRequested, this, &HelicalSFTPDialog::showLocalFileContextMenu);

}

HelicalSFTPDialog::~HelicalSFTPDialog()
{

    m_sftp->close();
    m_sftp.reset();

    delete ui;
}

void HelicalSFTPDialog::updateRemoteFileList(const QString &currentDirectory)
{
    QtSFTP::Directory directoryHandle;
    QFileIconProvider iconProvider;

    m_remoteFileSystemList->clear();

    ui->remoteLineEdit->setText(currentDirectory);

    directoryHandle= m_sftp->openDirectory(m_currentRemoteDirectory);

    if (directoryHandle==nullptr) {
        return;
    }

    while(!m_sftp->endOfDirectory(directoryHandle)) {
        QtSFTP::FileAttributes fileAttributes;
        m_sftp->readDirectory(directoryHandle, fileAttributes);
        if (fileAttributes) {
            if (static_cast<QString>(fileAttributes->name)==".") {
                continue;
            }
            if ((static_cast<QString>(fileAttributes->name)=="..") &&
                    (m_currentRemoteDirectory==m_RemoteFileSystemRoot)){
                continue;
            }
            HelicalFileItem *fileItem;
            m_remoteFileSystemList->addItem(new HelicalFileItem(fileAttributes->name));
            fileItem=dynamic_cast<HelicalFileItem*>(m_remoteFileSystemList->item (m_remoteFileSystemList->count()-1));
            fileItem->m_fileAttributes = std::move(fileAttributes);
            if (m_sftp->isADirectory(fileItem->m_fileAttributes)) {
                fileItem->setIcon(iconProvider.icon(QFileIconProvider::Folder));
            } else if (m_sftp->isARegularFile(fileItem->m_fileAttributes)) {
                fileItem->setIcon(iconProvider.icon(QFileIconProvider::File));
            }

        }

    }

    m_sftp->closeDirectory(directoryHandle);

    m_remoteFileSystemList->sortItems();

}

void HelicalSFTPDialog::fileDoubleClicked(QListWidgetItem *item)
{
    HelicalFileItem *fileItem = dynamic_cast<HelicalFileItem*>(item);

    if (m_sftp->isADirectory(fileItem->m_fileAttributes)) {
        if (static_cast<QString>(fileItem->m_fileAttributes->name)!="..") {
            m_currentRemoteDirectory = m_currentRemoteDirectory+"/"+fileItem->m_fileAttributes->name;
        } else {
            while(!m_currentRemoteDirectory.endsWith("/"))m_currentRemoteDirectory.chop(1);
            m_currentRemoteDirectory.chop(1);
        }

        updateRemoteFileList(m_currentRemoteDirectory);
    }

}

void HelicalSFTPDialog::localFileViewClicked(const QModelIndex &index)
{
    if (m_localFileSystemModel->isDir(index)) {
        m_currentLocalDirectory = m_localFileSystemModel->filePath(index);
        ui->localLineEdit->setText(m_currentLocalDirectory);
    }
}

void HelicalSFTPDialog::showRemoteFileContextMenu(const QPoint &pos)
{

    QMenu contextMenu("Remote File Context Menu", this);
    HelicalFileItem *fileItem = dynamic_cast<HelicalFileItem*>(m_remoteFileSystemList->currentItem());

    if (m_sftp->isARegularFile(fileItem->m_fileAttributes)) {
        contextMenu.addAction(new QAction("View", this));
        contextMenu.addAction(new QAction("Download", this));
    } else if (m_sftp->isADirectory(fileItem->m_fileAttributes)) {
        contextMenu.addAction(new QAction("Open", this));
    }

    QAction* selectedItem = contextMenu.exec(m_remoteFileSystemList->mapToGlobal(pos));
    QString remoteFile { m_currentRemoteDirectory + "/" + fileItem->m_fileAttributes->name};

    if (selectedItem){
        QString localFile;
        if (selectedItem->text()=="View") {
            localFile =  QDir::tempPath() + "/"+ fileItem->m_fileAttributes->name;
            m_sftp->getRemoteFile(remoteFile, localFile);
            QDesktopServices::openUrl(QUrl(localFile));
        } else if (selectedItem->text()=="Download") {
            localFile =  m_currentLocalDirectory + "/"+ fileItem->m_fileAttributes->name;
            m_sftp->getRemoteFile(remoteFile, localFile);
        } else if (selectedItem->text()=="Open") {
            m_currentRemoteDirectory =  m_currentRemoteDirectory + "/"+ fileItem->m_fileAttributes->name;
            updateRemoteFileList(m_currentRemoteDirectory);
        }

    }
}

void HelicalSFTPDialog::showLocalFileContextMenu(const QPoint &pos)
{

}


