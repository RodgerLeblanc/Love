/*
 * CloudantGetDoc.h
 *
 *  Created on: 2017-01-18
 *      Author: Roger
 */

#ifndef CLOUDANTGETRANDOMLOVEMESSAGE_H_
#define CLOUDANTGETRANDOMLOVEMESSAGE_H_

#include <QObject>
#include <QtNetwork>

class CloudantGetRandomLoveMessage : public QObject
{
    Q_OBJECT

public:
    CloudantGetRandomLoveMessage(QObject *_parent = NULL);
    virtual ~CloudantGetRandomLoveMessage() {}

    void getRandomLoveMessageAsync();

private slots:
    void onResponse();
    void onResponseSuccess(QVariantMap response);
    void onTimeout();

private:
    void getAllDocs();

    QNetworkAccessManager* m_networkAccessManager;

    QVariantMap m_allDocs;
    bool m_randomLoveMessageRequested;

signals:
    void randomLoveMessageRetrieved(QString message);
    void responseError(QString errorMessage);
    void responseSuccess(QVariantMap response);
};

#endif /* CLOUDANTGETRANDOMLOVEMESSAGE_H_ */
