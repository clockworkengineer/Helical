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

#include <QSplitter>
#include <QVBoxLayout>
#include <QListWidget>
#include <QTableWidget>
#include <QSizePolicy>
#include <QFileIconProvider>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>

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

    m_localFileSystemView->hideColumn(1);
    m_localFileSystemView->hideColumn(2);
    m_localFileSystemView->hideColumn(3);

    adjustSize();

    m_sftp.reset(new QtSFTP(session));

    m_sftp->open();

    updateRemoteFileList(m_currentRemoteDirectory);

    connect(m_sftp.data(), &QtSFTP::downloadFinished, this, &HelicalSFTPDialog::downloadFinished);
    connect(m_sftp.data(), &QtSFTP::uploadFinished, this, &HelicalSFTPDialog::uploadFinished);
    connect(m_sftp.data(), &QtSFTP::removedLink, this, &HelicalSFTPDialog::fileDeleted);
    connect(m_sftp.data(), &QtSFTP::error, this, &HelicalSFTPDialog::error);

    connect(m_remoteFileSystemList, &QListWidget::customContextMenuRequested, this, &HelicalSFTPDialog::showRemoteFileContextMenu);
    connect(m_remoteFileSystemList, &QListWidget::itemDoubleClicked, this, &HelicalSFTPDialog::fileDoubleClicked);
    connect(m_localFileSystemView, &QTreeView::clicked, this, &HelicalSFTPDialog::localFileViewClicked);
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

/**
 * @brief HelicalSFTPDialog::fileDoubleClicked
 * @param item
 */
void HelicalSFTPDialog::fileDoubleClicked(QListWidgetItem *item)
{
    HelicalFileItem *fileItem = dynamic_cast<HelicalFileItem*>(item);

    if (fileItem->text()=="..") {
        while(!m_currentRemoteDirectory.endsWith("/"))m_currentRemoteDirectory.chop(1);
        if (m_currentRemoteDirectory.size()!=1)m_currentRemoteDirectory.chop(1);
        updateRemoteFileList(m_currentRemoteDirectory);
        return;
    }

    if (m_sftp->isADirectory(fileItem->m_fileAttributes)) {
        if(m_currentRemoteDirectory.endsWith("/"))m_currentRemoteDirectory.chop(1);
        m_currentRemoteDirectory = m_currentRemoteDirectory+"/"+fileItem->m_fileAttributes->name;
        updateRemoteFileList(m_currentRemoteDirectory);
    }

}

/**
 * @brief HelicalSFTPDialog::localFileViewClicked
 * @param index
 */
void HelicalSFTPDialog::localFileViewClicked(const QModelIndex &index)
{
    if (m_localFileSystemModel->isDir(index)) {
        m_currentLocalDirectory = m_localFileSystemModel->filePath(index);
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

    if (fileItem==nullptr) {
        return;
    }

    if (fileItem->m_fileAttributes!=nullptr) {
        if (m_sftp->isARegularFile(fileItem->m_fileAttributes)) {
            contextMenu.addAction(new QAction("View", this));
            contextMenu.addAction(new QAction("Download", this));
            contextMenu.addAction(new QAction("Delete", this));
        } else if (m_sftp->isADirectory(fileItem->m_fileAttributes)) {
            contextMenu.addAction(new QAction("Open", this));
        }
    }

    contextMenu.addAction(new QAction("Refresh", this));

    QAction* selectedItem = contextMenu.exec(m_remoteFileSystemList->mapToGlobal(pos));

    if (selectedItem!=nullptr){
        QString localFile;
        QString remoteFile;
        if (selectedItem->text()=="View") {
            localFile =  QDir::tempPath() + "/"+ fileItem->m_fileAttributes->name;
            remoteFile = m_currentRemoteDirectory + "/" + fileItem->m_fileAttributes->name;
            m_sftp->getRemoteFile(remoteFile, localFile);
            QDesktopServices::openUrl(QUrl::fromLocalFile(localFile.toUtf8()));
        } else if (selectedItem->text()=="Download") {
            localFile =  m_currentLocalDirectory + "/" + fileItem->m_fileAttributes->name;
            remoteFile = m_currentRemoteDirectory + "/" + fileItem->m_fileAttributes->name;
            m_sftp->getRemoteFile(remoteFile, localFile);
        } else if (selectedItem->text()=="Open") {
            m_currentRemoteDirectory = m_currentRemoteDirectory + "/" + fileItem->m_fileAttributes->name;
            updateRemoteFileList(m_currentRemoteDirectory);
        }  else if (selectedItem->text()=="Delete") {
            remoteFile = m_currentRemoteDirectory + "/" + fileItem->m_fileAttributes->name;
            m_sftp->removeLink(remoteFile);
            updateRemoteFileList(m_currentRemoteDirectory);
        }  else if (selectedItem->text()=="Refresh") {
            updateRemoteFileList(m_currentRemoteDirectory);
        }

    }
}

/**
 * @brief HelicalSFTPDialog::showLocalFileContextMenu
 * @param pos
 */
void HelicalSFTPDialog::showLocalFileContextMenu(const QPoint &pos)
{

    QMenu contextMenu("Local File Context Menu", this);

    if(!m_localFileSystemModel->isDir(m_localFileSystemView->currentIndex())) {
        contextMenu.addAction(new QAction("Upload", this));
    }

    QAction* selectedItem = contextMenu.exec(m_localFileSystemView->mapToGlobal(pos));

    if (selectedItem!=nullptr){
        if (selectedItem->text()=="Upload") {
            QString localFile{m_localFileSystemModel->filePath(m_localFileSystemView->currentIndex())};
            QString remoteFile {m_currentRemoteDirectory + "/" + m_localFileSystemModel->fileName(m_localFileSystemView->currentIndex())};
            m_sftp->putLocalFile(localFile, remoteFile);
        }
    }

}

/**
 * @brief HelicalSFTPDialog::error
 * @param errorMessage
 * @param errorCode
 */
void HelicalSFTPDialog::error(const QString &errorMessage, int errorCode)
{
    Q_UNUSED(errorCode);

    ui->statusMessages->insertPlainText(errorMessage+"\n");
    ui->statusMessages->moveCursor(QTextCursor::End);
}

/**
 * @brief HelicalSFTPDialog::uploadFinished
 * @param sourceFile
 * @param destinationFile
 */
void HelicalSFTPDialog::uploadFinished(const QString &sourceFile, const QString &destinationFile)
{
    ui->statusMessages->insertPlainText(QString("Uploaded File \"%1\" to \"%2\".\n").arg(sourceFile).arg(destinationFile));
    ui->statusMessages->moveCursor(QTextCursor::End);
    updateRemoteFileList(m_currentRemoteDirectory);
}

/**
 * @brief HelicalSFTPDialog::downloadFinished
 * @param sourceFile
 * @param destinationFile
 */
void HelicalSFTPDialog::downloadFinished(const QString &sourceFile, const QString &destinationFile)
{
    ui->statusMessages->insertPlainText(QString("Downloaded File \"%1\" to \"%2\".\n").arg(sourceFile).arg(destinationFile));
    ui->statusMessages->moveCursor(QTextCursor::End);
}

/**
 * @brief HelicalSFTPDialog::fileDeleted
 * @param filePath
 */
void HelicalSFTPDialog::fileDeleted(const QString &filePath)
{
    ui->statusMessages->insertPlainText(QString("Deleted File \"%1\".\n").arg(filePath));
    ui->statusMessages->moveCursor(QTextCursor::End);
}


