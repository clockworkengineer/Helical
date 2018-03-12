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
// Description: Class to create/display an SFTP session dialog(window) for the viewing of remote
// files and their upload/download/deletion.
//

// =============
// INCLUDE FILES
// =============

#include "helicalsftpdialog.h"
#include "ui_helicalsftpdialog.h"

/**
 * @brief HelicalSFTPDialog::HelicalSFTPDialog
 *
 * Create SFTP session window.
 *
 * @param session           Currently active session
 * @param remoteUserHome    Remote user home directory
 * @param localUserHome     Local user home directory
 * @param parent            Unused
 */
HelicalSFTPDialog::HelicalSFTPDialog(QtSSH &session, const QString &remoteUserHome, const QString &localUserHome, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelicalSFTPDialog),
    m_remoteSystemRoot {remoteUserHome},
    m_localSystemRoot {localUserHome }
{

    // Register custom signal/slot types

    qRegisterMetaType<FileTransferAction>();

    ui->setupUi(this);

    // Window title

    setWindowTitle("SFTP");

    // Read local/remote file roots

    ui->localLineEdit->setReadOnly(true);
    ui->localLineEdit->setText(m_localSystemRoot);

    ui->remoteLineEdit->setReadOnly(true);
    ui->remoteLineEdit->setText(m_remoteSystemRoot);

    // Local Directory/file views

    m_localDirectorysModel = new QFileSystemModel(this);
    m_localDirectorysView  = new QTreeView(this);
    m_localDirectorysView->setModel(m_localDirectorysModel);
    m_localDirectorysModel->setRootPath(m_localSystemRoot);
    m_localDirectorysModel->setFilter(QDir::Hidden|QDir::NoDotAndDotDot|QDir::AllDirs);
    m_localDirectorysView->setRootIndex(m_localDirectorysModel->index(m_localSystemRoot));
    m_localDirectorysView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_localDirectorysView->setSelectionMode(QAbstractItemView::SingleSelection);

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
    ui->localFileViewsFrame->layout()->addWidget(m_localDirectorysView);
    ui->localFileViewsFrame->layout()->addWidget(m_localFilesView);

    ui->remoteFileViewsFrame->setLayout(new QHBoxLayout(this));
    ui->remoteFileViewsFrame->layout()->addWidget(m_remoteFileSystemList);

    // Hide local Directory view columns

    m_localDirectorysView->setHeaderHidden(true);
    m_localDirectorysView->hideColumn(1);
    m_localDirectorysView->hideColumn(2);
    m_localDirectorysView->hideColumn(3);

    adjustSize();

    // Open SFTP session and create file transfer task

    m_sftp.reset(new QtSFTP(session));

    if (m_sftp) {

        m_sftp->open();

        m_fileMapper.reset(new QtSFTP::FileMapper(m_currentLocalDirectory, m_currentRemoteDirectory));

        updateRemoteFileList();

        connect(m_sftp.data(), &QtSFTP::error, this, &HelicalSFTPDialog::error);

        connect(m_remoteFileSystemList, &QListWidget::customContextMenuRequested, this, &HelicalSFTPDialog::showRemoteFileContextMenu);
        connect(m_remoteFileSystemList, &QListWidget::itemDoubleClicked, this, &HelicalSFTPDialog::remoteFileDoubleClicked);
        connect(m_remoteFileSystemList, &QListWidget::itemClicked, this, &HelicalSFTPDialog::remoteFileClicked);
        connect(m_localDirectorysView, &QTreeView::clicked, this, &HelicalSFTPDialog::localDirectoryViewClicked);
        connect(m_localDirectorysView, &QTreeView::doubleClicked, this, &HelicalSFTPDialog::localDirectoryViewDoubleClicked);
        connect(m_localDirectorysView, &QTreeView::customContextMenuRequested, this, &HelicalSFTPDialog::showlocalDirectoryViewContextMenu);
        connect(m_localFilesView, &QTreeView::clicked, this, &HelicalSFTPDialog::localFileViewClicked);
        connect(m_localFilesView, &QTreeView::doubleClicked, this, &HelicalSFTPDialog::localFileViewDoubleClicked);
        connect(m_localFilesView, &QTreeView::customContextMenuRequested, this, &HelicalSFTPDialog::showLocalFileViewContextMenu);

        startupControllers(session);

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
 * Update remote file widget list for current remote directory.
 *
 * @param currentDirectory
 */
void HelicalSFTPDialog::updateRemoteFileList()
{
    QtSFTP::Directory directoryHandle;
    QFileIconProvider iconProvider;

    m_remoteFileSystemList->clear();

    ui->remoteLineEdit->setText(m_currentRemoteDirectory);

    // Read directory contents list and place in widget list

    directoryHandle= m_sftp->openDirectory(m_currentRemoteDirectory);

    if (directoryHandle!=nullptr) {

        while(!m_sftp->endOfDirectory(directoryHandle)) {
            QtSFTP::FileAttributes fileAttributes;
            m_sftp->readDirectory(directoryHandle, fileAttributes);
            if (fileAttributes) {   //  Ignore "." ".."
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

    // If not user home then enable parent directory link

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
 * @brief HelicalSFTPDialog::finishedTransactionMessage
 *
 * Output file transaction finished message.
 *
 * @param message
 */
void HelicalSFTPDialog::finishedTransactionMessage(const QString &message)
{
    ui->finishedTransactions->insertPlainText(message);
    ui->statusMessages->moveCursor(QTextCursor::End);
}

/**
 * @brief HelicalSFTPDialog::errorTransactionMessage
 *
 * Output file transaction error message.
 *
 * @param message
 */
void HelicalSFTPDialog::errorTransactionMessage(const QString &message)
{
    ui->errorTransactions->insertPlainText(message);
    ui->statusMessages->moveCursor(QTextCursor::End);
}

/**
 * @brief HelicalSFTPDialog::remoteFileDoubleClicked
 *
 * Remote file widget double clicked processing.
 *
 * @param item
 */
void HelicalSFTPDialog::remoteFileDoubleClicked(QListWidgetItem *item)
{
    HelicalRemoteFileItem *fileItem = dynamic_cast<HelicalRemoteFileItem*>(item);

    if (fileItem) {

        // Parent link so go up a directory

        if (fileItem->text()=="..") {
            while(!m_currentRemoteDirectory.endsWith(Antik::kServerPathSep))m_currentRemoteDirectory.chop(1);
            if (m_currentRemoteDirectory.size()!=1)m_currentRemoteDirectory.chop(1);
            updateRemoteFileList();
            return;
        }

        // Go into directory and list

        if (m_sftp->isADirectory(fileItem->m_fileAttributes)) {
            m_currentRemoteDirectory = fileItem->m_remoteFilePath;
            m_fileMapper.reset(new  QtSFTP::FileMapper(m_currentLocalDirectory, m_currentRemoteDirectory));
            updateRemoteFileList();
        }

    }

}
/**
 * @brief HelicalSFTPDialog::remoteFileClicked
 *
 * Remote file widget clicked processing.
 *
 * @param item
 */
void HelicalSFTPDialog::remoteFileClicked(QListWidgetItem *item)
{
    HelicalRemoteFileItem *fileItem = dynamic_cast<HelicalRemoteFileItem*>(item);

    if (fileItem) {

        // Ignore parent link

        if (fileItem->text()=="..") {
            return;
        }

        // File is a directory so change current remote directory (do not enter)

        if (m_sftp->isADirectory(fileItem->m_fileAttributes)) {
            ui->remoteLineEdit->setText(m_currentRemoteDirectory);
        }

    }

}

/**
 * @brief HelicalSFTPDialog::localDirectoryViewClicked
 *
 * Local Directory view clicked processing.
 *
 * @param index
 */
void HelicalSFTPDialog::localDirectoryViewClicked(const QModelIndex &index)
{

    // If a Directory(Directory) update current local directory, file mapper, file view root.

    if (m_localDirectorysModel->isDir(index)) {
        m_currentLocalDirectory = m_localDirectorysModel->filePath(index);
        ui->localLineEdit->setText(m_currentLocalDirectory);
        m_fileMapper.reset(new  QtSFTP::FileMapper(m_currentLocalDirectory, m_currentRemoteDirectory));
        m_localFilesView->setRootIndex(m_localFilesModel->index( m_localDirectorysModel->filePath(index)));
        if (m_currentLocalDirectory!=m_localSystemRoot) { // Enable ".." for non-root files view
            m_localFilesModel->setFilter(QDir::Hidden|QDir::AllEntries|QDir::NoDot);
        }
    }
}

/**
 * @brief HelicalSFTPDialog::localDirectoryViewDoubleClicked
 *
 * Local Directory view double click processing.
 *
 * @param index
 */
void HelicalSFTPDialog::localDirectoryViewDoubleClicked(const QModelIndex &index)
{

    // If a directory selected change current local direcrory and reset filemapper.

    if (m_localDirectorysModel->isDir(index)) {
        m_currentLocalDirectory = m_localDirectorysModel->filePath(index);
        ui->localLineEdit->setText(m_currentLocalDirectory);
        m_fileMapper.reset(new  QtSFTP::FileMapper(QFileInfo(m_currentLocalDirectory).dir().path(), m_currentRemoteDirectory));
    }

}

/**
 * @brief HelicalSFTPDialog::localFileViewClicked
 *
 * Local Directory view click processing.
 *
 * @param index
 */
void HelicalSFTPDialog::localFileViewClicked(const QModelIndex &index)
{

    // Just reset filemapper

    Q_UNUSED(index);
    m_fileMapper.reset(new  QtSFTP::FileMapper(m_currentLocalDirectory, m_currentRemoteDirectory));
}

/**
 * @brief HelicalSFTPDialog::localFileViewDoubleClicked
 *
 * Local file view double click processing.
 *
 * @param index
 */
void HelicalSFTPDialog::localFileViewDoubleClicked(const QModelIndex &index)
{

    if (m_localDirectorysModel->isDir(index)) {
        m_currentLocalDirectory = m_localFilesModel->fileInfo(index).canonicalFilePath();
        m_localFilesView->setRootIndex(m_localFilesModel->index(m_currentLocalDirectory));
        m_localDirectorysView->setCurrentIndex(m_localDirectorysModel->index(m_currentLocalDirectory));
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
 *
 * Show and process remote file context menu commands.
 *
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
 *
 * Show and process local Directory view context menu commands.
 *
 * @param pos
 */
void HelicalSFTPDialog::showlocalDirectoryViewContextMenu(const QPoint &pos)
{

    QMenu contextMenu("Local Directory Context Menu", this);
    QAction *menuAction;

    menuAction = new QAction("Upload", this);
    connect(menuAction, &QAction::triggered, this, &HelicalSFTPDialog::uploadSelectedDirectory);
    contextMenu.addAction(menuAction);

    contextMenu.exec(m_localDirectorysView->mapToGlobal(pos));

}

/**
 * @brief HelicalSFTPDialog::showLocalFileViewContextMenu
 *
 * Show and process local file view context menu commands.
 *
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
 *
 * Error message processing slot.
 *
 * @param errorMessage   Error message
 * @param errorCode      Error code
 * @param transactionID  File transaction ID
 */
void HelicalSFTPDialog::error(const QString &errorMessage, int errorCode, quint64 /*transactionID*/)
{
    Q_UNUSED(errorCode);

    statusMessage(errorMessage+"\n");

}

/**
 * @brief HelicalSFTPDialog::viewSelectedFiles
 *
 * Download and view selected files. This is performed directly using the local SFTP session and
 * not queued for processing.
 *
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
 *
 * Queue selected files/Directorys for download. Directories result in a resrusive list of the selected
 * directories and the files found queued by the file transaction task controller.
 *
 */
void HelicalSFTPDialog::downloadSelectedFile()
{

    for (auto listItem : m_remoteFileSystemList->selectedItems()) {
        HelicalRemoteFileItem *fileItem = dynamic_cast<HelicalRemoteFileItem*>(listItem);
        if (fileItem->m_fileAttributes) {
            if (m_sftp->isARegularFile(fileItem->m_fileAttributes)) {
                emit queueFileTransaction({ DOWNLOAD,fileItem->m_remoteFilePath, m_fileMapper->toLocal(fileItem->m_remoteFilePath) });
            } else if (m_sftp->isADirectory(fileItem->m_fileAttributes)) {
                emit queueFileTransaction({DOWNLOAD,fileItem->m_remoteFilePath, "", true, {m_currentLocalDirectory, m_currentRemoteDirectory}});
            }
        }
    }

}

/**
 * @brief HelicalSFTPDialog::deleteSelectedFiles
 *
 * Queue selected files/directories for deletion.
 *
 */
void HelicalSFTPDialog::deleteSelectedFiles()
{

    for  (auto listItem : m_remoteFileSystemList->selectedItems()) {
        HelicalRemoteFileItem *fileItem = dynamic_cast<HelicalRemoteFileItem*>(listItem);
        if (fileItem->m_fileAttributes) {
            if (m_sftp->isARegularFile(fileItem->m_fileAttributes)) {
                emit  queueFileTransaction({ DELETE, fileItem->m_remoteFilePath });
            } else if (m_sftp->isADirectory(fileItem->m_fileAttributes)) {
                queueFileTransaction({ DELETE, fileItem->m_remoteFilePath, "", true });
            }
        }
    }

}

/**
 * @brief HelicalSFTPDialog::enterSelectedDirectory
 *
 * Update the current remote directory and update its file widget list.
 *
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
 *
 * Refresh current remote directory file (widget) list.
 *
 */
void HelicalSFTPDialog::refreshSelectedDirectory()
{
    updateRemoteFileList();
}

/**
 * @brief HelicalSFTPDialog::uploadSelectedDirectory
 *
 * Queue selected local directory for upload to current remote directory.
 *
 */
void HelicalSFTPDialog::uploadSelectedDirectory()
{
    QModelIndexList indexList = m_localDirectorysView->selectionModel()->selectedIndexes();
    for (auto rowIndex : indexList) {
        if (rowIndex.column()==0) {
            QString localFile{m_localDirectorysModel->filePath(rowIndex)};
            emit queueFileTransaction({ UPLOAD, localFile, "", true, {QFileInfo(m_currentLocalDirectory).dir().path(),
                                                                      m_currentRemoteDirectory}});
        }
    }

}

/**
 * @brief HelicalSFTPDialog::uploadSelectedFiles
 *
 * Queue selected local directory/files for upload to current remote directory.
 *
 */
void HelicalSFTPDialog::uploadSelectedFiles()
{
    QModelIndexList indexList = m_localFilesView->selectionModel()->selectedIndexes();

    for (auto rowIndex : indexList) {
        if (rowIndex.column()==0) {
            if (!m_localDirectorysModel->isDir(rowIndex)) {
                emit queueFileTransaction({ UPLOAD,m_localDirectorysModel->filePath(rowIndex), m_fileMapper->toRemote(m_localDirectorysModel->filePath(rowIndex))});
            } else {
                emit queueFileTransaction({ UPLOAD, m_localDirectorysModel->filePath(rowIndex), "", true, {m_currentLocalDirectory, m_currentRemoteDirectory}});
            }
        }
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
    terminateControllers();
    QDialog::closeEvent(event);
}

/**
 * @brief HelicalSFTPDialog::startupControllers
 * @param session
 */
void HelicalSFTPDialog::startupControllers(QtSSH &session)
{

    for (auto controller=0; controller < kMaxControllers; controller++) {
        m_helicalTransferController[controller].createFileTransferTask(session);
        connect(&m_helicalTransferController[controller], &HelicalFileTransferController::statusMessage, this, &HelicalSFTPDialog::statusMessage);
        connect(&m_helicalTransferController[controller], &HelicalFileTransferController::finishedTransactionMessage, this, &HelicalSFTPDialog::finishedTransactionMessage);
        connect(&m_helicalTransferController[controller], &HelicalFileTransferController::errorTransactionMessage, this, &HelicalSFTPDialog::errorTransactionMessage);
        connect(&m_helicalTransferController[controller], &HelicalFileTransferController::updateRemoteFileList, this, &HelicalSFTPDialog::updateRemoteFileList);
    }

    m_helicalTransferController[0].setSupportedTransactions(m_helicalTransferController[0].supportedTransactions()|DELETE);
    connect(this,&HelicalSFTPDialog::queueFileTransaction, &m_helicalTransferController[0], &HelicalFileTransferController::queueFileTransaction);

}

/**
 * @brief HelicalSFTPDialog::terminateControllers
 */
void HelicalSFTPDialog::terminateControllers()
{
    for (auto controller=0; controller < kMaxControllers; controller++) {
        m_helicalTransferController[controller].destroyFileTransferTask();
    }
}
