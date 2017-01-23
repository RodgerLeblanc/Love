#include "src/WebCall/CloudantGetDoc.h"

#include "src/Utils/ApiKeys.hpp"
#include "src/Utils/Utils.hpp"

#include <bb/data/JsonDataAccess>
#include <QTimer>

using namespace bb::data;

CloudantGetDoc::CloudantGetDoc(QString id, QObject *_parent):
    QObject(_parent),
    m_networkAccessManager(new QNetworkAccessManager(this))
{
    QNetworkRequest request(QString(UTILS_CLOUDANT_DATABASE_URL + id));
    request.setRawHeader("Authorization", "Basic " + QByteArray(APIKEYS_CLOUDANT_AUTH));

    QNetworkReply* reply = m_networkAccessManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(onResponse()));
    reply->setParent(this);

    QTimer::singleShot(UTILS_CLOUDANT_GET_DOC_TIMEOUT, this, SLOT(onTimeout()));
}

void CloudantGetDoc::onResponse() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (reply) {
        if (reply->error() == QNetworkReply::NoError) {
            const int available = reply->bytesAvailable();
            if (available > 0) {
                const QByteArray buffer(reply->readAll());
                JsonDataAccess ja;
                QVariantMap response = ja.loadFromBuffer(buffer).toMap();
                debugv(response);

                if (response.contains("_rev")) {
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

void CloudantGetDoc::onTimeout() {
    emit responseError("Request timed out");
}
