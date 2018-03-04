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
    m_remoteSystemRoot {remoteUserHome},
    m_localSystemRoot {localUserHome }
{

//    qRegisterMetaType<FileMappingPair>();
//    qRegisterMetaType<FileAction>();
    qRegisterMetaType<FileTransferAction>();

    ui->setupUi(this);

    // Window title

    setWindowTitle("SFTP");

    // Read local/remote file roots

    ui->localLineEdit->setReadOnly(true);
    ui->localLineEdit->setText(m_localSystemRoot);

    ui->remoteLineEdit->setReadOnly(true);
    ui->remoteLineEdit->setText(m_remoteSystemRoot);

    // Local folder/file views

    m_localFoldersModel = new QFileSystemModel(this);
    m_localFoldersView  = new QTreeView(this);
    m_localFoldersView->setModel(m_localFoldersModel);
    m_localFoldersModel->setRootPath(m_localSystemRoot);
    m_localFoldersModel->setFilter(QDir::Hidden|QDir::NoDotAndDotDot|QDir::AllDirs);
    m_localFoldersView->setRootIndex(m_localFoldersModel->index(m_localSystemRoot));
    m_localFoldersView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_localFoldersView->setSelectionMode(QAbstractItemView::SingleSelection);

    m_localFilesModel = new QFileSystemModel(this);
    m_localFilesView  = new QListView(this);
    m_localFilesView->setModel(m_localFilesModel);
    m_localFilesModel->setRootPath(m_localSystemRoot);
    m_localFilesModel->setFilter(QDir::Hidden|QDir::AllEntries|QDir::NoDotAndDotDot);
    m_localFilesView->setRootIndex(m_localFilesModel->index(m_localSystemRoot));
    m_localFilesView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_localFilesView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Remote file system view (widget list)

    m_remoteFileSystemList = new QListWidget(this);
    m_remoteFileSystemList->setContextMenuPolicy(Qt::CustomContextMenu);
    m_remoteFileSystemList->setSelectionMode(QAbstractItemView::ExtendedSelection);

    // Put views onto window

    ui->localFileViewsFrame->setLayout(new QVBoxLayout(this));
    ui->localFileViewsFrame->layout()->addWidget(m_localFoldersView);
    ui->localFileViewsFrame->layout()->addWidget(m_localFilesView);

    ui->remoteFileViewsFrame->setLayout(new QHBoxLayout(this));
    ui->remoteFileViewsFrame->layout()->addWidget(m_remoteFileSystemList);

    // Hide local folder view columns

    m_localFoldersView->setHeaderHidden(true);
    m_localFoldersView->hideColumn(1);
    m_localFoldersView->hideColumn(2);
    m_localFoldersView->hideColumn(3);

    adjustSize();

    // Open SFTP session and create file transfer task

    m_sftp.reset(new QtSFTP(session));

    if (m_sftp) {

        m_sftp->open();

        m_fileMapper.reset(new QtSFTP::FileMapper(m_currentLocalDirectory, m_currentRemoteDirectory));

        updateRemoteFileList();

        //       connect(m_sftp.data(), &QtSFTP::removedLink, this, &HelicalSFTPDialog::deleteFileFinised);
        connect(m_sftp.data(), &QtSFTP::error, this, &HelicalSFTPDialog::error);

        connect(m_remoteFileSystemList, &QListWidget::customContextMenuRequested, this, &HelicalSFTPDialog::showRemoteFileContextMenu);
        connect(m_remoteFileSystemList, &QListWidget::itemDoubleClicked, this, &HelicalSFTPDialog::remoteFileDoubleClicked);
        connect(m_remoteFileSystemList, &QListWidget::itemClicked, this, &HelicalSFTPDialog::remoteFileClicked);
        connect(m_localFoldersView, &QTreeView::clicked, this, &HelicalSFTPDialog::localFolderViewClicked);
        connect(m_localFoldersView, &QTreeView::doubleClicked, this, &HelicalSFTPDialog::localFolderViewDoubleClicked);
        connect(m_localFoldersView, &QTreeView::customContextMenuRequested, this, &HelicalSFTPDialog::showLocalFolderViewContextMenu);
        connect(m_localFilesView, &QTreeView::clicked, this, &HelicalSFTPDialog::localFileViewClicked);
        connect(m_localFilesView, &QTreeView::doubleClicked, this, &HelicalSFTPDialog::localFileViewDoubleClicked);
        connect(m_localFilesView, &QTreeView::customContextMenuRequested, this, &HelicalSFTPDialog::showLocalFileViewContextMenu);

        m_helicalTransferController.createFileTransferTask(session);

        connect(this,&HelicalSFTPDialog::openSession, &m_helicalTransferController, &HelicalFileTransferController::openSession);
        connect(this,&HelicalSFTPDialog::closeSession, &m_helicalTransferController, &HelicalFileTransferController::closeSession);
        connect(this,&HelicalSFTPDialog::processFile, &m_helicalTransferController, &HelicalFileTransferController::processFile);
        connect(this,&HelicalSFTPDialog::processDirectory, &m_helicalTransferController, &HelicalFileTransferController::processDirectory);
        connect(this,&HelicalSFTPDialog::queueFileForProcessing, &m_helicalTransferController, &HelicalFileTransferController::queueFileForProcessing);
        connect(this,&HelicalSFTPDialog::processNextFile, &m_helicalTransferController, &HelicalFileTransferController::processNextFile);

        connect(&m_helicalTransferController, &HelicalFileTransferController::statusMessage, this, &HelicalSFTPDialog::statusMessage);
        connect(&m_helicalTransferController, &HelicalFileTransferController::updateRemoteFileList, this, &HelicalSFTPDialog::updateRemoteFileList);
        connect(&m_helicalTransferController, &HelicalFileTransferController::error, this, &HelicalSFTPDialog::error);

    }

}

/**
 * @brief HelicalSFTPDialog::~HelicalSFTPDialog
 *
 * Close SFTP session and remove.
 *
 */
HelicalSFTPDialog::~HelicalSFTPDialog()
{

    m_sftp->close();
    m_sftp.reset();

    delete ui;
}

/**
 * @brief HelicalSFTPDialog::updateRemoteFileList
 *
 * Update remote file widget list for passed in directory.
 *
 * @param currentDirectory
 */
void HelicalSFTPDialog::updateRemoteFileList()
{
    QtSFTP::Directory directoryHandle;
    QFileIconProvider iconProvider;

    m_remoteFileSystemList->clear();

    ui->remoteLineEdit->setText(m_currentRemoteDirectory);

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
                HelicalRemoteFileItem *fileItem;
                m_remoteFileSystemList->addItem(new HelicalRemoteFileItem(fileAttributes->name));
                fileItem=dynamic_cast<HelicalRemoteFileItem*>(m_remoteFileSystemList->item (m_remoteFileSystemList->count()-1));
                fileItem->m_remoteFilePath = m_currentRemoteDirectory + Antik::kServerPathSep + fileAttributes->name;
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

    if (m_currentRemoteDirectory!=m_remoteSystemRoot){
        m_remoteFileSystemList->insertItem(0, new HelicalRemoteFileItem(".."));
    }

}

/**
 * @brief HelicalSFTPDialog::statusMessage
 *
 * Output message to window status area.
 *
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
    HelicalRemoteFileItem *fileItem = dynamic_cast<HelicalRemoteFileItem*>(item);

    if (fileItem->text()=="..") {
        while(!m_currentRemoteDirectory.endsWith(Antik::kServerPathSep))m_currentRemoteDirectory.chop(1);
        if (m_currentRemoteDirectory.size()!=1)m_currentRemoteDirectory.chop(1);
        updateRemoteFileList();
        return;
    }

    if (m_sftp->isADirectory(fileItem->m_fileAttributes)) {
        m_currentRemoteDirectory = fileItem->m_remoteFilePath;
        m_fileMapper.reset(new  QtSFTP::FileMapper(m_currentLocalDirectory, m_currentRemoteDirectory));
        updateRemoteFileList();
    }

}
/**
 * @brief HelicalSFTPDialog::remoteFileClicked
 * @param item
 */
void HelicalSFTPDialog::remoteFileClicked(QListWidgetItem *item)
{
    HelicalRemoteFileItem *fileItem = dynamic_cast<HelicalRemoteFileItem*>(item);

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
void HelicalSFTPDialog::localFolderViewClicked(const QModelIndex &index)
{
    if (m_localFoldersModel->isDir(index)) {
        m_currentLocalDirectory = m_localFoldersModel->filePath(index);
        ui->localLineEdit->setText(m_currentLocalDirectory);
        m_fileMapper.reset(new  QtSFTP::FileMapper(m_currentLocalDirectory, m_currentRemoteDirectory));
        m_localFilesView->setRootIndex(m_localFilesModel->index( m_localFoldersModel->filePath(index)));
        if (m_currentLocalDirectory!=m_localSystemRoot) {
            m_localFilesModel->setFilter(QDir::Hidden|QDir::AllEntries|QDir::NoDot);
        }
    }
}

/**
 * @brief HelicalSFTPDialog::localFileSystemViewDoubleClicked
 * @param index
 */
void HelicalSFTPDialog::localFolderViewDoubleClicked(const QModelIndex &index)
{

    if (m_localFoldersModel->isDir(index)) {
        m_currentLocalDirectory = m_localFoldersModel->filePath(index);
        ui->localLineEdit->setText(m_currentLocalDirectory);
        m_fileMapper.reset(new  QtSFTP::FileMapper(QFileInfo(m_currentLocalDirectory).dir().path(), m_currentRemoteDirectory));
    }

}

/**
 * @brief HelicalSFTPDialog::localFileViewClicked
 * @param index
 */
void HelicalSFTPDialog::localFileViewClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    m_fileMapper.reset(new  QtSFTP::FileMapper(m_currentLocalDirectory, m_currentRemoteDirectory));
}

/**
 * @brief HelicalSFTPDialog::localFileViewDoubleClicked
 * @param index
 */
void HelicalSFTPDialog::localFileViewDoubleClicked(const QModelIndex &index)
{

    if (m_localFoldersModel->isDir(index)) {
        m_currentLocalDirectory = m_localFilesModel->fileInfo(index).canonicalFilePath();
        m_localFilesView->setRootIndex(m_localFilesModel->index(m_currentLocalDirectory));
        m_localFoldersView->setCurrentIndex(m_localFoldersModel->index(m_currentLocalDirectory));
        if (m_currentLocalDirectory==m_localSystemRoot) {
            m_localFilesModel->setFilter(QDir::Hidden|QDir::AllEntries|QDir::NoDotAndDotDot);
        } else {
            m_localFilesModel->setFilter(QDir::Hidden|QDir::AllEntries|QDir::NoDot);
        }
        ui->localLineEdit->setText(m_currentLocalDirectory);
        m_fileMapper.reset(new  QtSFTP::FileMapper(m_currentLocalDirectory, m_currentRemoteDirectory));
    }



}

/**
 * @brief HelicalSFTPDialog::showRemoteFileContextMenu
 * @param pos
 */
void HelicalSFTPDialog::showRemoteFileContextMenu(const QPoint &pos)
{

    QMenu contextMenu("Remote File Context Menu", this);
    HelicalRemoteFileItem *fileItem = dynamic_cast<HelicalRemoteFileItem*>(m_remoteFileSystemList->currentItem());
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
void HelicalSFTPDialog::showLocalFolderViewContextMenu(const QPoint &pos)
{

    QMenu contextMenu("Local Folder Context Menu", this);
    QAction *menuAction;

    menuAction = new QAction("Upload", this);
    connect(menuAction, &QAction::triggered, this, &HelicalSFTPDialog::uploadSelectedFolder);
    contextMenu.addAction(menuAction);

    contextMenu.exec(m_localFoldersView->mapToGlobal(pos));

}

/**
 * @brief HelicalSFTPDialog::showLocalFileViewContextMenu
 * @param pos
 */
void HelicalSFTPDialog::showLocalFileViewContextMenu(const QPoint &pos)
{
    QMenu contextMenu("Local File Context Menu", this);
    QAction *menuAction;

    menuAction = new QAction("Upload", this);
    connect(menuAction, &QAction::triggered, this, &HelicalSFTPDialog::uploadSelectedFiles);
    contextMenu.addAction(menuAction);

    contextMenu.exec(m_localFilesView->mapToGlobal(pos));
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

    emit processNextFile(UPLOAD);
    emit processNextFile(DOWNLOAD);
    emit processNextFile(DELETE);

}

/**
 * @brief HelicalSFTPDialog::viewSelectedFiles
 */
void HelicalSFTPDialog::viewSelectedFiles()
{

    for  (auto listItem : m_remoteFileSystemList->selectedItems()) {
        HelicalRemoteFileItem *fileItem = dynamic_cast<HelicalRemoteFileItem*>(listItem);
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
        HelicalRemoteFileItem *fileItem = dynamic_cast<HelicalRemoteFileItem*>(listItem);
        if (fileItem->m_fileAttributes) {
            if (m_sftp->isARegularFile(fileItem->m_fileAttributes)) {
                emit queueFileForProcessing({ DOWNLOAD,fileItem->m_remoteFilePath, m_fileMapper->toLocal(fileItem->m_remoteFilePath) });
            } else if (m_sftp->isADirectory(fileItem->m_fileAttributes)) {
                emit processDirectory({DOWNLOAD,fileItem->m_remoteFilePath, "", {m_currentLocalDirectory, m_currentRemoteDirectory}});
            }
        }
    }

    emit processNextFile(DOWNLOAD);

}

/**
 * @brief HelicalSFTPDialog::deleteSelectedFiles
 */
void HelicalSFTPDialog::deleteSelectedFiles()
{

    bool deferDelete {false};

    for  (auto listItem : m_remoteFileSystemList->selectedItems()) {
        HelicalRemoteFileItem *fileItem = dynamic_cast<HelicalRemoteFileItem*>(listItem);
        if (fileItem->m_fileAttributes) {
            if (m_sftp->isARegularFile(fileItem->m_fileAttributes)) {
                emit  queueFileForProcessing({ DELETE, fileItem->m_remoteFilePath });
            } else if (m_sftp->isADirectory(fileItem->m_fileAttributes)) {
                emit processDirectory({ DELETE, fileItem->m_remoteFilePath });
                deferDelete=true;
            }
        }
    }

    if(!deferDelete) {
        emit processNextFile(DELETE);
    }


}

/**
 * @brief HelicalSFTPDialog::enterSelectedDirectory
 */
void HelicalSFTPDialog::enterSelectedDirectory()
{
    HelicalRemoteFileItem *fileItem = dynamic_cast<HelicalRemoteFileItem*>(m_remoteFileSystemList->currentItem());
    if (fileItem->m_fileAttributes) {
        m_currentRemoteDirectory = fileItem->m_remoteFilePath;
        updateRemoteFileList();
    }
}

/**
 * @brief HelicalSFTPDialog::refreshSelectedDirectory
 */
void HelicalSFTPDialog::refreshSelectedDirectory()
{
    updateRemoteFileList();
}

/**
 * @brief HelicalSFTPDialog::uploadSelectedFolder
 */
void HelicalSFTPDialog::uploadSelectedFolder()
{
    QModelIndexList indexList = m_localFoldersView->selectionModel()->selectedIndexes();
    for (auto rowIndex : indexList) {
        if (rowIndex.column()==0) {
            QString localFile{m_localFoldersModel->filePath(rowIndex)};
            emit processDirectory({ UPLOAD, localFile, "", {QFileInfo(m_currentLocalDirectory).dir().path(),
                                            m_currentRemoteDirectory}});
        }
    }

}

/**
 * @brief HelicalSFTPDialog::uploadSelectedFiles
 */
void HelicalSFTPDialog::uploadSelectedFiles()
{
    QModelIndexList indexList = m_localFilesView->selectionModel()->selectedIndexes();
    bool deferUpload=false;

    for (auto rowIndex : indexList) {
        if (!m_localFoldersModel->isDir(rowIndex)) {
            emit queueFileForProcessing({ UPLOAD,m_localFoldersModel->filePath(rowIndex), m_fileMapper->toRemote(m_localFoldersModel->filePath(rowIndex))});
        } else {
            emit processDirectory({ UPLOAD, m_localFoldersModel->filePath(rowIndex), "", {m_currentLocalDirectory, m_currentRemoteDirectory}});
            deferUpload=true;
        }
    }

    if (!deferUpload) {
        emit processNextFile(UPLOAD);
    }

}

/**
 * @brief HelicalSFTPDialog::closeEvent
 *
 * Override for window close event. Closedown file transfer task.
 *
 * @param event
 */
void HelicalSFTPDialog::closeEvent(QCloseEvent *event)
{
    this->m_helicalTransferController.destroyFileTransferTask();
    QDialog::closeEvent(event);
}
