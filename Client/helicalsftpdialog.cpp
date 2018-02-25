/*
 * File:   helicalterminaldialog.cpp
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

//
// Class: HelicalSFTPDialog
//
// Description: Class to create/display SFTP session dialog for the viewing of remote
// files and their upload/download and manipulation.
//

// =============
// INCLUDE FILES
// =============

#include "helicalsftpdialog.h"
#include "ui_helicalsftpdialog.h"


/**
 * @brief HelicalSFTPDialog::HelicalSFTPDialog
 * @param session
 * @param remoteUserHome
 * @param parent
 */
HelicalSFTPDialog::HelicalSFTPDialog(QtSSH &session, const QString &remoteUserHome, const QString &localUserHome, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelicalSFTPDialog),
    m_remoteFileSystemRoot {remoteUserHome},
    m_localFileSystemRoot {localUserHome }
{

    ui->setupUi(this);

    setWindowTitle("SFTP");

    ui->localLineEdit->setReadOnly(true);
    ui->localLineEdit->setText(m_currentLocalDirectory);

    ui->remoteLineEdit->setReadOnly(true);
    ui->remoteLineEdit->setText(m_remoteFileSystemRoot);

    m_fileMapper.reset(new QtSFTP::FileMapper(m_currentLocalDirectory, m_currentRemoteDirectory));

    m_localFileSystemModel = new QFileSystemModel(this);
    m_localFileSystemView  = new QTreeView(this);
    m_localFileSystemView->setModel(m_localFileSystemModel);
    m_localFileSystemModel->setRootPath(m_localFileSystemRoot);
    m_localFileSystemView->setRootIndex(m_localFileSystemModel->index(m_localFileSystemRoot));
    m_localFileSystemView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_localFileSystemView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_remoteFileSystemList = new QListWidget(this);
    m_remoteFileSystemList->setContextMenuPolicy(Qt::CustomContextMenu);
    m_remoteFileSystemList->setSelectionMode(QAbstractItemView::ExtendedSelection);

    ui->localFileViewFrame->setLayout(new QHBoxLayout(this));
    ui->localFileViewFrame->layout()->addWidget(m_localFileSystemView);

    ui->remoteFileViewFrame->setLayout(new QHBoxLayout(this));
    ui->remoteFileViewFrame->layout()->addWidget(m_remoteFileSystemList);

    m_localFileSystemView->hideColumn(1);
    m_localFileSystemView->hideColumn(2);
    m_localFileSystemView->hideColumn(3);

    adjustSize();

    m_sftp.reset(new QtSFTP(session));

    m_sftp->open();

    updateRemoteFileList(m_currentRemoteDirectory);

    createFileTransferTask(session);

    connect(m_sftp.data(), &QtSFTP::removedLink, this, &HelicalSFTPDialog::fileDeleted);
    connect(m_sftp.data(), &QtSFTP::error, this, &HelicalSFTPDialog::error);

    connect(m_remoteFileSystemList, &QListWidget::customContextMenuRequested, this, &HelicalSFTPDialog::showRemoteFileContextMenu);
    connect(m_remoteFileSystemList, &QListWidget::itemDoubleClicked, this, &HelicalSFTPDialog::remoteFileDoubleClicked);
    connect(m_remoteFileSystemList, &QListWidget::itemClicked, this, &HelicalSFTPDialog::remoteFileClicked);
    connect(m_localFileSystemView, &QTreeView::clicked, this, &HelicalSFTPDialog::localFileViewClicked);
    connect(m_localFileSystemView, &QTreeView::doubleClicked, this, &HelicalSFTPDialog::localFileDoubleClicked);
    connect(m_localFileSystemView, &QTreeView::customContextMenuRequested, this, &HelicalSFTPDialog::showLocalFileContextMenu);

}

/**
 * @brief HelicalSFTPDialog::~HelicalSFTPDialog
 */
HelicalSFTPDialog::~HelicalSFTPDialog()
{

    m_sftp->close();
    m_sftp.reset();

    delete ui;
}


/**
 * @brief HelicalSFTPDialog::updateRemoteFileList
 * @param currentDirectory
 */
void HelicalSFTPDialog::updateRemoteFileList(const QString &currentDirectory)
{
    QtSFTP::Directory directoryHandle;
    QFileIconProvider iconProvider;

    m_remoteFileSystemList->clear();

    ui->remoteLineEdit->setText(currentDirectory);

    directoryHandle= m_sftp->openDirectory(m_currentRemoteDirectory);

    if (directoryHandle!=nullptr) {

        while(!m_sftp->endOfDirectory(directoryHandle)) {
            QtSFTP::FileAttributes fileAttributes;
            m_sftp->readDirectory(directoryHandle, fileAttributes);
            if (fileAttributes) {
                if ((static_cast<QString>(fileAttributes->name)==".")||
                        (static_cast<QString>(fileAttributes->name)=="..")) {
                    continue;
                }
                HelicalFileItem *fileItem;
                m_remoteFileSystemList->addItem(new HelicalFileItem(fileAttributes->name));
                fileItem=dynamic_cast<HelicalFileItem*>(m_remoteFileSystemList->item (m_remoteFileSystemList->count()-1));
                fileItem->m_remoteFilePath = m_currentRemoteDirectory + "/" + fileAttributes->name;
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

    if (m_currentRemoteDirectory!=m_remoteFileSystemRoot){
        m_remoteFileSystemList->insertItem(0, new HelicalFileItem(".."));
    }

}

void HelicalSFTPDialog::createFileTransferTask(QtSSH &session)
{
    QScopedPointer<QThread> fileTransferThread { new QThread() };

    m_fileTransferTask.reset(new HelicalFileTransferTask());
    m_fileTransferTask->setFileTaskThread(fileTransferThread.take());
    m_fileTransferTask->moveToThread(m_fileTransferTask->fileTaskThread());
    m_fileTransferTask->fileTaskThread()->start();

    connect(this,&HelicalSFTPDialog::openSession, m_fileTransferTask.data(), &HelicalFileTransferTask::openSession);
    connect(this,&HelicalSFTPDialog::closeSession, m_fileTransferTask.data(), &HelicalFileTransferTask::closeSession);
    connect(this,&HelicalSFTPDialog::uploadFile, m_fileTransferTask.data(), &HelicalFileTransferTask::uploadFile);
    connect(this,&HelicalSFTPDialog::downloadFile, m_fileTransferTask.data(), &HelicalFileTransferTask::downloadFile);
    connect(this,&HelicalSFTPDialog::listRemoteDirectoryRecursive, m_fileTransferTask.data(), &HelicalFileTransferTask::listRemoteDirectoryRecursive);
    connect(this,&HelicalSFTPDialog::listLocalDirectoryRecursive, m_fileTransferTask.data(), &HelicalFileTransferTask::listLocalDirectoryRecursive);

    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::downloadFinished, this, &HelicalSFTPDialog::downloadFinished);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::uploadFinished, this, &HelicalSFTPDialog::uploadFinished);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::listedRemoteFileName, this, &HelicalSFTPDialog::queueFileForDownload);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::listedLocalFileName, this, &HelicalSFTPDialog::queueFileForUpload);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::startDownloading, this, &HelicalSFTPDialog::downloadNextFile);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::startUploading, this, &HelicalSFTPDialog::uploadNextFile);
    connect(m_fileTransferTask.data(), &HelicalFileTransferTask::error, this, &HelicalSFTPDialog::error);

    // Delete thread when it is finished

    connect(m_fileTransferTask->fileTaskThread(),&QThread::finished,m_fileTransferTask->fileTaskThread(), &QThread::deleteLater );
    emit openSession(session.getServerName(), session.getServerPort(), session.getUserName(), session.getUserPassword());

}

/**
 * @brief HelicalSFTPDialog::destroyFileTransferTask
 */
void HelicalSFTPDialog::destroyFileTransferTask()
{

    m_downloadQueue.clear();
    m_uploadQueue.clear();

    emit closeSession();
    m_fileTransferTask.take();

}

/**
 * @brief HelicalSFTPDialog::statusMessage
 * @param message
 */
void HelicalSFTPDialog::statusMessage(const QString &message)
{
    ui->statusMessages->insertPlainText(message);
    ui->statusMessages->moveCursor(QTextCursor::End);

}

/**
 * @brief HelicalSFTPDialog::remoteFileDoubleClicked
 * @param item
 */
void HelicalSFTPDialog::remoteFileDoubleClicked(QListWidgetItem *item)
{
    HelicalFileItem *fileItem = dynamic_cast<HelicalFileItem*>(item);

    if (fileItem->text()=="..") {
        while(!m_currentRemoteDirectory.endsWith("/"))m_currentRemoteDirectory.chop(1);
        if (m_currentRemoteDirectory.size()!=1)m_currentRemoteDirectory.chop(1);
        updateRemoteFileList(m_currentRemoteDirectory);
        return;
    }

    if (m_sftp->isADirectory(fileItem->m_fileAttributes)) {
        m_currentRemoteDirectory = fileItem->m_remoteFilePath;
        m_fileMapper.reset(new  QtSFTP::FileMapper(m_currentLocalDirectory, m_currentRemoteDirectory));
        updateRemoteFileList(m_currentRemoteDirectory);
    }

}
/**
 * @brief HelicalSFTPDialog::remoteFileClicked
 * @param item
 */
void HelicalSFTPDialog::remoteFileClicked(QListWidgetItem *item)
{
    HelicalFileItem *fileItem = dynamic_cast<HelicalFileItem*>(item);

    if (fileItem->text()=="..") {
        return;
    }

    if (m_sftp->isADirectory(fileItem->m_fileAttributes)) {
        ui->remoteLineEdit->setText(m_currentRemoteDirectory);
    }

}

/**
 * @brief HelicalSFTPDialog::localFileViewClicked
 * @param index
 */
void HelicalSFTPDialog::localFileViewClicked(const QModelIndex &index)
{
    if (m_localFileSystemModel->isDir(index)) {
         ui->localLineEdit->setText(m_currentLocalDirectory);
    }
}

void HelicalSFTPDialog::localFileDoubleClicked(const QModelIndex &index)
{
    if (m_localFileSystemModel->isDir(index)) {
        m_currentLocalDirectory = m_localFileSystemModel->filePath(index);
        m_fileMapper.reset(new  QtSFTP::FileMapper(m_currentLocalDirectory, m_currentRemoteDirectory));
        ui->localLineEdit->setText(m_currentLocalDirectory);
    }
}

/**
 * @brief HelicalSFTPDialog::showRemoteFileContextMenu
 * @param pos
 */
void HelicalSFTPDialog::showRemoteFileContextMenu(const QPoint &pos)
{

    QMenu contextMenu("Remote File Context Menu", this);
    HelicalFileItem *fileItem = dynamic_cast<HelicalFileItem*>(m_remoteFileSystemList->currentItem());
    QAction *menuAction;

    if (fileItem==nullptr) {
        return;
    }

    if (fileItem->m_fileAttributes!=nullptr) {
        if ((m_sftp->isARegularFile(fileItem->m_fileAttributes)||(m_sftp->isADirectory(fileItem->m_fileAttributes)))) {
            menuAction = new QAction("View", this);
            connect(menuAction, &QAction::triggered, this, &HelicalSFTPDialog::viewSelectedFiles);
            contextMenu.addAction(menuAction);
            menuAction = new QAction("Download", this);
            connect(menuAction, &QAction::triggered, this, &HelicalSFTPDialog::downloadSelectedFile);
            contextMenu.addAction(menuAction);
            menuAction = new QAction("Delete", this);
            connect(menuAction, &QAction::triggered, this, &HelicalSFTPDialog::deleteSelectedFiles);
            contextMenu.addAction(menuAction);
        } else if (m_sftp->isADirectory(fileItem->m_fileAttributes) && (m_remoteFileSystemList->selectedItems().size()==1)) {
            menuAction = new QAction("Enter", this);
            connect(menuAction, &QAction::triggered, this, &HelicalSFTPDialog::enterSelectedDirectory);
            contextMenu.addAction(menuAction);
        }
    }

    menuAction = new QAction("Refresh", this);
    connect(menuAction, &QAction::triggered, this, &HelicalSFTPDialog::refreshSelectedDirectory);
    contextMenu.addAction(menuAction);

    contextMenu.exec(m_remoteFileSystemList->mapToGlobal(pos));

}

/**
 * @brief HelicalSFTPDialog::showLocalFileContextMenu
 * @param pos
 */
void HelicalSFTPDialog::showLocalFileContextMenu(const QPoint &pos)
{

    QMenu contextMenu("Local File Context Menu", this);
    QAction *menuAction;

    menuAction = new QAction("Upload", this);
    connect(menuAction, &QAction::triggered, this, &HelicalSFTPDialog::uploadSelectedFiles);
    contextMenu.addAction(menuAction);

    contextMenu.exec(m_localFileSystemView->mapToGlobal(pos));

}

/**
 * @brief HelicalSFTPDialog::error
 * @param errorMessage
 * @param errorCode
 */
void HelicalSFTPDialog::error(const QString &errorMessage, int errorCode)
{
    Q_UNUSED(errorCode);

    statusMessage(errorMessage+"\n");

    uploadNextFile();
    downloadNextFile();

}

/**
 * @brief HelicalSFTPDialog::uploadFinished
 * @param sourceFile
 * @param destinationFile
 */
void HelicalSFTPDialog::uploadFinished(const QString &sourceFile, const QString &destinationFile)
{

    statusMessage(QString("Uploaded File \"%1\" to \"%2\".\n").arg(sourceFile).arg(destinationFile));

    uploadNextFile();

    updateRemoteFileList(m_currentRemoteDirectory);

}

/**
 * @brief HelicalSFTPDialog::downloadFinished
 * @param sourceFile
 * @param destinationFile
 */
void HelicalSFTPDialog::downloadFinished(const QString &sourceFile, const QString &destinationFile)
{
    statusMessage(QString("Downloaded File \"%1\" to \"%2\".\n").arg(sourceFile).arg(destinationFile));

    downloadNextFile();

}

/**
 * @brief HelicalSFTPDialog::fileDeleted
 * @param filePath
 */
void HelicalSFTPDialog::fileDeleted(const QString &filePath)
{
    statusMessage(QString("Deleted File \"%1\".\n").arg(filePath));
}

void HelicalSFTPDialog::viewSelectedFiles()
{
    for  (auto listItem : m_remoteFileSystemList->selectedItems()) {
        HelicalFileItem *fileItem = dynamic_cast<HelicalFileItem*>(listItem);
        if (fileItem->m_fileAttributes) {
            if (m_sftp->isARegularFile(fileItem->m_fileAttributes)) {
                QString localFile {QDir::tempPath() + "/"+ fileItem->m_fileAttributes->name};
                m_sftp->getRemoteFile(fileItem->m_remoteFilePath, localFile);
                QDesktopServices::openUrl(QUrl::fromLocalFile(localFile.toUtf8()));
            }
        }
    }
}

/**
 * @brief HelicalSFTPDialog::downloadSelectedFile
 */
void HelicalSFTPDialog::downloadSelectedFile()
{
    for (auto listItem : m_remoteFileSystemList->selectedItems()) {
        HelicalFileItem *fileItem = dynamic_cast<HelicalFileItem*>(listItem);
        if (fileItem->m_fileAttributes) {
            if (m_sftp->isARegularFile(fileItem->m_fileAttributes)) {
                ui->statusMessages->insertPlainText(QString("File \"%1\" queued for download.\n").arg(fileItem->m_remoteFilePath));
                m_downloadQueue.push_back({fileItem->m_remoteFilePath, m_fileMapper->toLocal(fileItem->m_remoteFilePath)});
            } else if (m_sftp->isADirectory(fileItem->m_fileAttributes)) {
                emit listRemoteDirectoryRecursive(fileItem->m_remoteFilePath);
            }
        }
    }
    downloadNextFile();
}

/**
 * @brief HelicalSFTPDialog::deleteSelectedFiles
 */
void HelicalSFTPDialog::deleteSelectedFiles()
{
    for  (auto listItem : m_remoteFileSystemList->selectedItems()) {
        HelicalFileItem *fileItem = dynamic_cast<HelicalFileItem*>(listItem);
        if (fileItem->m_fileAttributes) {
            if (m_sftp->isARegularFile(fileItem->m_fileAttributes)) {
                m_sftp->removeLink(fileItem->m_remoteFilePath);
            }
        }
    }
    updateRemoteFileList(m_currentRemoteDirectory);
}

/**
 * @brief HelicalSFTPDialog::enterSelectedDirectory
 */
void HelicalSFTPDialog::enterSelectedDirectory()
{
    HelicalFileItem *fileItem = dynamic_cast<HelicalFileItem*>(m_remoteFileSystemList->currentItem());
    if (fileItem->m_fileAttributes) {
        m_currentRemoteDirectory = fileItem->m_remoteFilePath;
        updateRemoteFileList(m_currentRemoteDirectory);
    }
}

/**
 * @brief HelicalSFTPDialog::refreshSelectedDirectory
 */
void HelicalSFTPDialog::refreshSelectedDirectory()
{
    updateRemoteFileList(m_currentRemoteDirectory);
}

/**
 * @brief HelicalSFTPDialog::uploadSelectedFiles
 */
void HelicalSFTPDialog::uploadSelectedFiles()
{
    QModelIndexList indexList = m_localFileSystemView->selectionModel()->selectedIndexes();
    bool deferUpload=false;

    for (auto rowIndex : indexList) {
        if (rowIndex.column()==0) {
            if (!m_localFileSystemModel->isDir(rowIndex)) {
                QString localFile{m_localFileSystemModel->filePath(rowIndex)};
       //         QString remoteFile {m_currentRemoteDirectory + "/" + m_localFileSystemModel->fileName(rowIndex)};
                statusMessage(QString("File \"%1\" queued for upload.\n").arg(localFile));
                m_uploadQueue.push_back({localFile, m_fileMapper->toRemote(localFile)});
            } else {
                QString localFile{m_localFileSystemModel->filePath(rowIndex)};
                emit listLocalDirectoryRecursive(localFile);
                deferUpload=true;
            }
        }
    }

    if (!deferUpload) {
        uploadNextFile();
    }

}

/**
 * @brief HelicalSFTPDialog::queueFileForDownload
 * @param fileName
 */
void HelicalSFTPDialog::queueFileForDownload(const QString &fileName)
{
    statusMessage(QString("File \"%1\" queued for download.\n").arg(fileName));
    m_downloadQueue.push_back({fileName, m_fileMapper->toLocal(fileName)});
}

/**
 * @brief HelicalSFTPDialog::queueFileForUpload
 * @param fileName
 */
void HelicalSFTPDialog::queueFileForUpload(const QString &fileName)
{
    statusMessage(QString("File \"%1\" queued for upload.\n").arg(fileName));
    m_uploadQueue.push_back({fileName, m_fileMapper->toRemote(fileName)});
    qDebug() << fileName << " " << m_fileMapper->toRemote(fileName);
}

/**
 * @brief HelicalSFTPDialog::downloadNextFile
 */
void HelicalSFTPDialog::downloadNextFile()
{
    if (!m_downloadQueue.isEmpty()) {
        emit downloadFile(m_downloadQueue.front().first, m_downloadQueue.front().second);
        m_downloadQueue.pop_front();
    } else {
        statusMessage("Download queue cleared.");
    }
}

/**
 * @brief HelicalSFTPDialog::uploadNextFile
 */
void HelicalSFTPDialog::uploadNextFile()
{
    if (!m_uploadQueue.isEmpty()) {
        emit uploadFile(m_uploadQueue.front().first, m_uploadQueue.front().second);
        m_uploadQueue.pop_front();
    } else {
        statusMessage("Upload queue cleared.");
    }
}

/**
 * @brief HelicalSFTPDialog::closeEvent
 *
 * Overrde for window close event.
 *
 * @param event
 */
void HelicalSFTPDialog::closeEvent(QCloseEvent *event)
{
    destroyFileTransferTask();
    QDialog::closeEvent(event);
}
