/*
 * AzureScreenText.h
 *
 *  Created on: 2017-01-18
 *      Author: Roger
 */

#ifndef AZURESCREENTEXT_H_
#define AZURESCREENTEXT_H_

#include <QObject>
#include <QtNetwork>

class AzureScreenText : public QObject
{
    Q_OBJECT

public:
    AzureScreenText(QString text, QString language, QObject *_parent = NULL);
    virtual ~AzureScreenText() {}

private slots:
    void onResponse();
    void onTimeout();

private:
    QNetworkAccessManager* m_networkAccessManager;

signals:
    void responseError(QString errorMessage);
    void illegalWordsFound(QStringList illegalWords);
    void responseSuccess(QVariantMap response);
    void finished(bool passed, QStringList illegalWords);
};

#endif /* AZURESCREENTEXT_H_ */
