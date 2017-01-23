#include "src/WebCall/CloudantUpdateDoc.h"

#include "src/Utils/ApiKeys.hpp"
#include "src/Utils/Utils.hpp"

#include <bb/data/JsonDataAccess>
#include <QTimer>

using namespace bb::data;

CloudantUpdateDoc::CloudantUpdateDoc(QString id, QVariantMap doc, QObject *_parent):
    QObject(_parent),
    m_networkAccessManager(new QNetworkAccessManager(this))
{
    QNetworkRequest request(QString(UTILS_CLOUDANT_DATABASE_URL + id));
    request.setRawHeader("Authorization", "Basic " + QByteArray(APIKEYS_CLOUDANT_AUTH));
    request.setRawHeader("Content-Type", "application/json");

    QString buffer;
    JsonDataAccess jda;
    jda.saveToBuffer(doc, &buffer);

    debugv(request.url());
    debugv(buffer);

    QNetworkReply* reply = m_networkAccessManager->put(request, buffer.toUtf8());
    connect(reply, SIGNAL(finished()), this, SLOT(onResponse()));
    reply->setParent(this);

    QTimer::singleShot(UTILS_CLOUDANT_UPDATE_DOC_TIMEOUT, this, SLOT(onTimeout()));
}

void CloudantUpdateDoc::onResponse() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (reply) {
        if (reply->error() == QNetworkReply::NoError) {
            const int available = reply->bytesAvailable();
            if (available > 0) {
                const QByteArray buffer(reply->readAll());
                JsonDataAccess ja;
                QVariantMap response = ja.loadFromBuffer(buffer).toMap();
                debugv(response);

                if (response["ok"].toBool()) {
                    emit responseSuccess(response);
                }
                else {
                    emit responseError("Cloudant response indicates an error: ");
                }
            }
            else {
                emit responseError("Empty string received from Cloudant DB");
            }
        }
        else {
            emit responseError(QString::number(reply->error()) + " " + reply->errorString());
        }

        reply->deleteLater();
    }
    else {
        emit responseError("Couldn't retrieve reply");
    }
}

void CloudantUpdateDoc::onTimeout() {
    emit responseError("Request timed out");
}
