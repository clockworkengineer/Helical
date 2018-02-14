#include "helicalsftpdialog.h"
#include "ui_helicalsftpdialog.h"

#include <QSplitter>
#include <QVBoxLayout>
#include <QListWidget>
#include <QTableWidget>
#include <QSizePolicy>
#include <QFileIconProvider>
//#include <QLabel>
//#include <QLineEdit>

#include <QDebug>

HelicalSFTPDialog::HelicalSFTPDialog(QtSSH &session, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelicalSFTPDialog)
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

    m_remoteFileSystemList = new QListWidget(this);

    ui->fileViewFrame->setLayout(new QVBoxLayout(this));
    splitter = new QSplitter(this);
    splitter->addWidget(m_localFileSystemView);
    splitter->addWidget(m_remoteFileSystemList);

    ui->fileViewFrame->layout()->addWidget(splitter);

    adjustSize();

    m_sftp.reset(new QtSFTP(session));

    m_sftp->open();

    updateRemoteFileList(m_currentRemoteDirectory);

    connect(m_remoteFileSystemList, &QListWidget::itemDoubleClicked, this, &HelicalSFTPDialog::fileDoubleClicked);
    connect(m_localFileSystemView, &QAbstractItemView::clicked, this, &HelicalSFTPDialog::localFileViewClicked);
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
            fileItem=static_cast<HelicalFileItem*>(m_remoteFileSystemList->item (m_remoteFileSystemList->count()-1));
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
    HelicalFileItem *fileItem = static_cast<HelicalFileItem*>(item);

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
