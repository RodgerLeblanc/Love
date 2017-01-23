/*
 * AzureCheckLanguage.h
 *
 *  Created on: 2017-01-18
 *      Author: Roger
 */

#ifndef AZURECHECKLANGUAGE_H_
#define AZURECHECKLANGUAGE_H_

#include <QObject>
#include <QtNetwork>

class AzureCheckLanguage : public QObject
{
    Q_OBJECT

public:
    AzureCheckLanguage(QString message, QObject *_parent = NULL);
    virtual ~AzureCheckLanguage() {}

private slots:
    void onResponse();
    void onTimeout();

private:
    QNetworkAccessManager* m_networkAccessManager;

signals:
    void returnedLanguage(QString);
    void responseError(QString errorMessage, QString language);
    void responseSuccess(QVariantMap response, QString language);
};

#endif /* AZURECHECKLANGUAGE_H_ */
