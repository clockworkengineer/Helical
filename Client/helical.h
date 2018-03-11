/*
 * File:   helical.h
 *
 * Author: Robert Tizzard
 *
 * Created on January 10, 2018
 *
 * Copyright 2018.
 *
 */

#ifndef HELICAL_H
#define HELICAL_H

//
// Description: Common client definitions.
//

// =============
// INCLUDE FILES
// =============

#include <QString>

// File mapping pair

typedef std::pair<QString, QString> FileMappingPair;

// Supported file transactions

enum FileAction {
    UPLOAD,
    DOWNLOAD,
    DELETE
};

// File transfer action

struct FileTransferAction {

    // Constructors

    FileTransferAction() {}

    FileTransferAction(const FileAction &action, const QString &sourceFile, const QString &destinationFile="", bool directory=false,const FileMappingPair &fileMappingPair=FileMappingPair()) :
        m_action {action}, m_sourceFile {sourceFile}, m_destinationFile{destinationFile}, m_directory {directory}, m_fileMappingPair {fileMappingPair} {}

    std::uint64_t m_fileTransferID {0}; // Transfer ID
    FileAction m_action;                // File transaction type
    QString m_sourceFile;               // Source file
    QString m_destinationFile;          // Destination dile
    bool m_directory {false};           // == true processing directory
    FileMappingPair m_fileMappingPair;  // File Mapping pair

};

Q_DECLARE_METATYPE(FileTransferAction);

#endif // HELICAL_H




