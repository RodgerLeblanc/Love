/*
 * HeadlessCommunication.h
 *
 *  Created on: 16.01.2014
 *      Author: benjaminsliwa
 */

#ifndef HEADLESSCOMMUNICATION_H_
#define HEADLESSCOMMUNICATION_H_

#define UI_LISTENING_PORT 11985
#define HEADLESS_LISTENING_PORT 11986

#include <QObject>
#include <QtNetwork/QUdpSocket>

class HeadlessCommunication : public QObject
{
	Q_OBJECT
public:
	HeadlessCommunication(QObject *_parent = 0);
	virtual ~HeadlessCommunication() {}

	// RX
	void listenOnPort(int _port);

	// TX
    Q_INVOKABLE void sendMessage(QString _data);
    Q_INVOKABLE void sendMessage(const QString& reason, const QVariant& data);
    Q_INVOKABLE void sendToWatchface(QString _data);

private:
	QUdpSocket *m_socket;
	QUdpSocket *m_server;

signals:
	void receivedData(QString);

public slots:
	void onReadyRead();
};

#endif /* HEADLESSCOMMUNICATION_H_ */
