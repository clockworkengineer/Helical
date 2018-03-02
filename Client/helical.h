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

Q_DECLARE_METATYPE(FileMappingPair);
Q_DECLARE_METATYPE(FileAction);

#endif // HELICAL_H
