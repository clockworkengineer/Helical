#ifndef HELICAL_H
#define HELICAL_H

#include <QString>

typedef std::pair<QString, QString> FileTransferPair;
typedef std::pair<QString, QString> FileMappingPair;

enum FileAction {
    UPLOAD,
    DOWNLOAD,
    DELETE
};

struct FileTransferAction {
    FileTransferAction() {}
    FileTransferAction(const FileAction &action, const QString &sourceFile, const QString &destinationFile) :
        m_action {action}, m_sourceFile {sourceFile}, m_destinationFile{destinationFile} {}
    FileTransferAction(const FileAction &action, const QString &sourceFile) :
        m_action {action}, m_sourceFile {sourceFile} {}
    std::uint64_t m_id;
    FileAction m_action;
    QString m_sourceFile;
    QString m_destinationFile;
};

Q_DECLARE_METATYPE(FileMappingPair);
Q_DECLARE_METATYPE(FileAction);
Q_DECLARE_METATYPE(FileTransferAction);

#endif // HELICAL_H
