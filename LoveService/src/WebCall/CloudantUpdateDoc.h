/*
 * CloudantUpdateDoc.h
 *
 *  Created on: 2017-01-18
 *      Author: Roger
 */

#ifndef CLOUDANTUPDATEDOC_H_
#define CLOUDANTUPDATEDOC_H_

#include <QObject>
#include <QtNetwork>

class CloudantUpdateDoc : public QObject
{
    Q_OBJECT

public:
    CloudantUpdateDoc(QString id, QVariantMap doc, QObject *_parent = NULL);
    virtual ~CloudantUpdateDoc() {}

private slots:
    void onResponse();
    void onTimeout();

private:
    QNetworkAccessManager* m_networkAccessManager;

signals:
    void responseError(QString errorMessage);
    void responseSuccess(QVariantMap response);
};

#endif /* CLOUDANTUPDATEDOC_H_ */
