/*
 * CloudantGetDoc.h
 *
 *  Created on: 2017-01-18
 *      Author: Roger
 */

#ifndef CLOUDANTGETDOC_H_
#define CLOUDANTGETDOC_H_

#include <QObject>
#include <QtNetwork>

class CloudantGetDoc : public QObject
{
    Q_OBJECT

public:
    CloudantGetDoc(QString id, QObject *_parent = NULL);
    virtual ~CloudantGetDoc() {}

private slots:
    void onResponse();
    void onTimeout();

private:
    QNetworkAccessManager* m_networkAccessManager;

signals:
    void responseError(QString errorMessage);
    void responseSuccess(QVariantMap response);
};

#endif /* CLOUDANTGETDOC_H_ */
