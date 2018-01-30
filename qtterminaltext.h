#ifndef QTTERMINALTEXT_H
#define QTTERMINALTEXT_H

#include <QObject>
#include <QPlainTextEdit>
#include <QKeyEvent>

class QtTerminalText : public QPlainTextEdit
{
    Q_OBJECT
 public:
     QtTerminalText(QWidget *parent = 0);

 protected:
     virtual void keyPressEvent(QKeyEvent *e);

 signals:
     void keySend(const QByteArray &keyAscii);

 public slots:

     void terminalOutput(const QString &text);

};

#endif // QTTERMINALTEXT_H
