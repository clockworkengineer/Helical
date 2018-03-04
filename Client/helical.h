#ifndef HELICAL_H
#define HELICAL_H

#include <QString>

typedef std::pair<QString, QString> FileMappingPair;

enum FileAction {
    UPLOAD,
    DOWNLOAD,
    DELETE
};

struct FileTransferAction {
    FileTransferAction() {}
    FileTransferAction(const FileAction &action, const QString &sourceFile, const QString &destinationFile="", const FileMappingPair &fileMappingPair=FileMappingPair()) :
        m_action {action}, m_sourceFile {sourceFile}, m_destinationFile{destinationFile}, m_fileMappingPair {fileMappingPair} {}
    FileAction m_action;
    QString m_sourceFile;
    QString m_destinationFile;
    FileMappingPair m_fileMappingPair;
};

//Q_DECLARE_METATYPE(FileMappingPair);
//Q_DECLARE_METATYPE(FileAction);
Q_DECLARE_METATYPE(FileTransferAction);

#endif // HELICAL_H
