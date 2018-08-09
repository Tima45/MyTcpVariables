#ifndef TCPVARIABLESERVER_H
#define TCPVARIABLESERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>


class SocketWrapper : public QObject{
    Q_OBJECT
public:
    explicit SocketWrapper(QTcpSocket *socket,QObject* parent = 0);
    ~SocketWrapper();
private:
    QTcpSocket *socket;
    QDataStream stream;

signals:
    void newMessage(QByteArray);
private slots:
    void readFull();
};


class AbstractTcpVariableServer : public QObject{
    Q_OBJECT
public:
    explicit AbstractTcpVariableServer(QString networkName, QObject *parent = 0);
    virtual ~AbstractTcpVariableServer();
    virtual void decodeValue(QByteArray undecodedValue) = 0;
    static void setPort(int serverPort);
private:
    static int port;

    static QTcpServer* server;
    static QVector<AbstractTcpVariableServer *>variables;
    static void handleNewConnection();
    static QVector<SocketWrapper*> clientsSockets;
    static void parseMessage(QByteArray text);
    static int findByName(int hashName);

    int hashName;

signals:
    void valueUpdated();

};



template<typename T>
class TcpVariableServer : public AbstractTcpVariableServer
{
public:
    TcpVariableServer(QString networkName,QObject *parent = 0) : AbstractTcpVariableServer(networkName,parent){

    }
    ~TcpVariableServer(){

    }
    void decodeValue(QByteArray undecodedValue){
        char *bytesOfValue = &value;
        for(int i = 0; i < undecodedValue.count(); i++){
            bytesOfValue[i] = undecodedValue[i];
        }
        emit valueUpdated();
    }
    void setValue(T value){
        this->value = value;
    }
    T getValue(){
        return value;
    }
    void synchronize(){

    }

private:
    T value;
    TcpVariableServer(const TcpVariableServer &);
};

#endif // TCPVARIABLESERVER_H
