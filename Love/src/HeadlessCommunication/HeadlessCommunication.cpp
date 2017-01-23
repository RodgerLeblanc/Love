/*
 * HeadlessCommunication.cpp
 *
 *  Created on: 16.01.2014
 *      Author: benjaminsliwa
 */

#include <src/HeadlessCommunication/HeadlessCommunication.h>

#include <bb/PpsObject>

HeadlessCommunication::HeadlessCommunication(QObject *_parent) :
    QObject(_parent)
{
	m_server = new QUdpSocket(this);
	m_socket = new QUdpSocket(this);

	listenOnPort(UI_LISTENING_PORT);
}

void HeadlessCommunication::listenOnPort(int _port)
{
    qDebug() << "HeadlessCommunication::listenOnPort()" << _port;
	m_server->bind(QHostAddress::Any, _port);
	bool ok = connect(m_server, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

	ok==true ? qDebug() << "binding ok" : qDebug() << "binding failed";
}

void HeadlessCommunication::sendMessage(QString _data)
{
//  qDebug() << "HeadlessCommunication::sendMessage()" << _data;
    m_socket->writeDatagram(_data.toStdString().c_str(),QHostAddress("127.0.0.1"), HEADLESS_LISTENING_PORT);
}

void HeadlessCommunication::sendMessage(const QString& reason, const QVariant& data) {
    QVariantMap map;
    map.insert("reason", reason);
    map.insert("data", data);
    this->sendMessage(bb::PpsObject::encode(map));
}

void HeadlessCommunication::sendToWatchface(QString _data)
{
    qDebug() << "HeadlessCommunication::sendToWatchface()" << _data;
    m_socket->writeDatagram(_data.toStdString().c_str(),QHostAddress("127.0.0.1"), 9877);
}

void HeadlessCommunication::onReadyRead()
{
//	qDebug() << "HeadlessCommunication::dataReceived";
    while (m_server->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(m_server->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_server->readDatagram(datagram.data(), datagram.size(),&sender, &senderPort);
        QString data = QString(datagram);

//        qDebug() << data;
        emit receivedData(data);

    }
}
