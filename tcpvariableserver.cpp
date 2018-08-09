#include "tcpvariableserver.h"

QTcpServer* AbstractTcpVariableServer::server = nullptr;
QVector<AbstractTcpVariableServer *> AbstractTcpVariableServer::variables;
int AbstractTcpVariableServer::port = -1;

AbstractTcpVariableServer::AbstractTcpVariableServer(QString networkName,QObject *parent) : QObject(parent) , hashName(qHash(networkName))
{
    if(variables.isEmpty()){
        server = new QTcpServer(0);
        if(server->listen(QHostAddress::AnyIPv4,port)){
            connect(server,&QTcpServer::newConnection,&handleNewConnection);
        }else{
            qDebug() << "Could not start server, check port";
        }
    }
    variables.append(this);
}

AbstractTcpVariableServer::~AbstractTcpVariableServer()
{
    if(variables.removeOne(this)){
        if(variables.isEmpty()){
            server->deleteLater();
        }
    }
}

void AbstractTcpVariableServer::setPort(int serverPort)
{
    port = serverPort;
}

void AbstractTcpVariableServer::handleNewConnection()
{
    while(server->hasPendingConnections()){
        QTcpSocket *newClient = server->nextPendingConnection();
        SocketWrapper *wrapper = new SocketWrapper(newClient,newClient->parent());
        connect(wrapper,&SocketWrapper::newMessage,&parseMessage);
        clientsSockets.append(wrapper);
    }
}
int AbstractTcpVariableServer::findByName(int hashName){
    for(int i = 0; i < variables.count(); i++){
        if(hashName == variables.at(i)->hashName){
            return i;
        }
    }
    qDebug() << "an unknown variable is received";
    return -1;
}

void AbstractTcpVariableServer::parseMessage(QByteArray text)
{
    if(!text.isEmpty()){
        QByteArrayList list = text.split(':');
        if(list.count() == 2){
            if(QString(list.at(0)) == "names"){
                int id = findByName(list.at(1).toInt());
                if(id != -1){

                }
            }
            int id = findByName(qHash(list.at(0)));
            if(id != -1){
                variables.at(id)->decodeValue(list.at(1));
            }
        }else{
            qDebug() << "list.count() ==" << list.count();
        }
    }
}


SocketWrapper::SocketWrapper(QTcpSocket *socket, QObject *parent) :QObject(parent), socket(socket)
{
    stream.setDevice(socket);
    connect(socket,SIGNAL(readyRead()),this,SLOT(readFull()));
}

void SocketWrapper::readFull()
{
    stream.startTransaction();

    QByteArray message;
    stream >> message;

    if(!stream.commitTransaction()){
        return;
    }
    emit newMessage(message);
}

SocketWrapper::~SocketWrapper()
{

}
