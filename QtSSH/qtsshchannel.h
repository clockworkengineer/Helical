#ifndef QTSSHCHANNEL_H
#define QTSSHCHANNEL_H

#include <QObject>

class QtSSHChannel : public QObject
{
    Q_OBJECT
public:
    explicit QtSSHChannel(QObject *parent = nullptr);

signals:

public slots:
};

#endif // QTSSHCHANNEL_H