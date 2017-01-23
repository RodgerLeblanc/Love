#include "src/WebCall/AzureCheckLanguage.h"
#include "src/Utils/ApiKeys.hpp"
#include "src/Utils/Utils.hpp"

#include <bb/data/JsonDataAccess>
#include <QTimer>

using namespace bb::data;

AzureCheckLanguage::AzureCheckLanguage(QString text, QObject *_parent):
    QObject(_parent),
    m_networkAccessManager(new QNetworkAccessManager(this))
{
    QNetworkRequest request(QString(UTILS_AZURE_DETECT_LANGUAGE_URL));
    request.setRawHeader("Ocp-Apim-Subscription-Key", APIKEYS_AZURE_SUBSCRIPTION_KEY);
    request.setRawHeader("Content-Type", "text/plain");

    debugv(request.url());
    debugv(text);
    debugv(text.toUtf8());

    QNetworkReply* reply = m_networkAccessManager->post(request, text.toUtf8());
    connect(reply, SIGNAL(finished()), this, SLOT(onResponse()));
    reply->setParent(this);

    QTimer::singleShot(UTILS_AZURE_LANGUAGE_TIMEOUT, this, SLOT(onTimeout()));
}

void AzureCheckLanguage::onResponse() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    debugv(reply);

    if (reply) {
        debugv(reply->error());
        if (reply->error() == QNetworkReply::NoError) {
            const int available = reply->bytesAvailable();
            debugv(available);
            if (available > 0) {
                const QByteArray buffer(reply->readAll());
                debugv(buffer);
                JsonDataAccess ja;
                QVariantMap response = ja.loadFromBuffer(buffer).toMap();
                debugv(response);

                if (response.contains("DetectedLanguage")) {
                    emit returnedLanguage(response["DetectedLanguage"].toString());
                    emit responseSuccess(response, response["DetectedLanguage"].toString());
                }
                else {
                    emit returnedLanguage(UTILS_UNKNOWN_LANGUAGE);
                    emit responseError("Azure response indicates an error: ", UTILS_UNKNOWN_LANGUAGE);
                }

            }
            else {
                emit returnedLanguage(UTILS_UNKNOWN_LANGUAGE);
                emit responseError("Empty string received from Azure", UTILS_UNKNOWN_LANGUAGE);
            }
        }
        else {
            emit returnedLanguage(UTILS_UNKNOWN_LANGUAGE);
            emit responseError(QString::number(reply->error()) + " " + reply->errorString(), UTILS_UNKNOWN_LANGUAGE);
        }

        reply->deleteLater();
    }
    else {
        emit returnedLanguage(UTILS_UNKNOWN_LANGUAGE);
        emit responseError("Couldn't retrieve reply", UTILS_UNKNOWN_LANGUAGE);
    }
}

void AzureCheckLanguage::onTimeout() {
    emit returnedLanguage(UTILS_UNKNOWN_LANGUAGE);
    emit responseError("Request timed out", UTILS_UNKNOWN_LANGUAGE);
}
