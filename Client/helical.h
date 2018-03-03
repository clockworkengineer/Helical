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
    std::uint64_t id;
    FileAction action;
    QString sourceFile;
    QString destinationFile;
};

Q_DECLARE_METATYPE(FileMappingPair);
Q_DECLARE_METATYPE(FileAction);

#endif // HELICAL_H
