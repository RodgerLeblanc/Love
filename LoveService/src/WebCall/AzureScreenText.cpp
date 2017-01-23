#include "src/WebCall/AzureScreenText.h"

#include "src/Utils/ApiKeys.hpp"
#include "src/Utils/Utils.hpp"

#include <bb/data/JsonDataAccess>
#include <QTimer>

using namespace bb::data;

AzureScreenText::AzureScreenText(QString text, QString language, QObject *_parent):
    QObject(_parent),
    m_networkAccessManager(new QNetworkAccessManager(this))
{
    QUrl url(QString(UTILS_AZURE_SCREEN_URL));
    url.addQueryItem("language", language);
    url.addQueryItem("autocorrect", "false");
    url.addQueryItem("urls", "true");
    url.addQueryItem("PII", "true");
    debugv(url);

    QNetworkRequest request(url);
    request.setRawHeader("Ocp-Apim-Subscription-Key", APIKEYS_AZURE_SUBSCRIPTION_KEY);
    request.setRawHeader("Content-Type", "text/plain");

    QNetworkReply* reply = m_networkAccessManager->post(request, text.toUtf8());
    connect(reply, SIGNAL(finished()), this, SLOT(onResponse()));
    reply->setParent(this);

    QTimer::singleShot(UTILS_AZURE_PROFANITY_TIMEOUT, this, SLOT(onTimeout()));
}

void AzureScreenText::onResponse() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QStringList illegalWords;

    if (reply) {
        if (reply->error() == QNetworkReply::NoError) {
            const int available = reply->bytesAvailable();
            if (available > 0) {
                const QByteArray buffer(reply->readAll());
                JsonDataAccess ja;
                QVariantMap response = ja.loadFromBuffer(buffer).toMap();
                debugv(response);

                if (response["Status"].toMap()["Code"].toInt() == 3000) {
                    foreach(QVariant url, response["Urls"].toList()) {
                        illegalWords.append(url.toMap()["URL"].toString());
                    }

                    foreach(QString key, response["PII"].toMap().keys()) {
                        foreach(QVariant pii, response["PII"].toMap()[key].toList()) {
                            illegalWords.append(pii.toMap()["Text"].toString());
                        }
                    }

                    foreach(QVariant term, response["Terms"].toList())
                        illegalWords.append(term.toMap()["Term"].toString());

                    illegalWords.removeDuplicates();

                    if (illegalWords.isEmpty()) {
                        emit finished(true, illegalWords);
                        emit responseSuccess(response);
                    }
                    else {
                        emit finished(false, illegalWords);
                        emit illegalWordsFound(illegalWords);
                        QString errorMessage = "Illegal word" + QString((illegalWords.size() > 1) ? "s" : "") + " found: " + illegalWords.join(", ");
                        emit responseError(errorMessage);
                    }
                }
                else {
                    emit finished(false, illegalWords);
                    emit responseError("Azure response indicates an error: ");
                }

            }
            else {
                emit finished(false, illegalWords);
                emit responseError("Empty string received from Azure");
            }
        }
        else {
            emit finished(false, illegalWords);
            emit responseError(QString::number(reply->error()) + " " + reply->errorString());
        }

        reply->deleteLater();
    }
    else {
        emit finished(false, illegalWords);
        emit responseError("Couldn't retrieve reply");
    }
}

void AzureScreenText::onTimeout() {
    QStringList illegalWords;
    emit finished(false, illegalWords);
    emit responseError("Request timed out");
}
