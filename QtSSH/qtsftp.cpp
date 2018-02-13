#include "qtsftp.h"

QtSFTP::QtSFTP(QtSSH &session, QObject *parent) : QObject {parent},
    m_sftp { new CSFTP (session.getSession()) }
{

}

void QtSFTP::open()
{

    try {
        m_sftp->open();
        emit opened();
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

void QtSFTP::close()
{

    try {
        m_sftp->close();
        emit closed();
    }catch(const CSFTP::Exception &e) {
        emit error(QString::fromStdString(e.getMessage()),e.getCode());
    }

}

CSFTP *QtSFTP::sftp() const
{
    return m_sftp;
}
