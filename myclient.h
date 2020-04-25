#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QThreadPool>
#include <QtGui>
#include <QRegExp>
//#include "myserver.h"
class MyServer;

class MyClient : public QObject
{
    friend class MyServer;
    Q_OBJECT

public:

//    static const quint8 comMessageToAll = 5;

    explicit MyClient(int desc, MyServer *serv, QObject *parent = 0);
    ~MyClient();
    void setName(QString name) {_name = name;}
    QString getName() const {return _name;}
    bool getAutched() const {return _isAutched;}
    void doSendCommand(quint8 comm) const;
    void doSendUsersOnline() const;

signals:
    void addUserToGui(QString name);
    void removeUserFromGui(QString name);
    void removeUser(MyClient *client);
    void messageToGui(QString message, QString from, const QStringList &users);

private slots:
    void onConnect();
    void onDisconnect();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError) const;

private:
    QTcpSocket *_sok;
    MyServer *_serv;
    quint16 _blockSize;
    QString _name;
    bool _isAutched;

};

#endif // MYCLIENT_H
