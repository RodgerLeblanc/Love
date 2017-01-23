#include "src/WebCall/CloudantGetRandomLoveMessage.h"

#include "src/Utils/ApiKeys.hpp"
#include "src/Utils/Utils.hpp"

#include <bb/data/JsonDataAccess>
#include <QTimer>

using namespace bb::data;

CloudantGetRandomLoveMessage::CloudantGetRandomLoveMessage(QObject *_parent):
    QObject(_parent),
    m_networkAccessManager(new QNetworkAccessManager(this)),
    m_randomLoveMessageRequested(false)
{
    srand(QDateTime::currentMSecsSinceEpoch());
    connect(this, SIGNAL(responseSuccess(QVariantMap)), this, SLOT(onResponseSuccess(QVariantMap)));
}

void CloudantGetRandomLoveMessage::getAllDocs() {
    QNetworkRequest request(QString(UTILS_CLOUDANT_DATABASE_URL) + "_all_docs?include_docs=true");
    request.setRawHeader("Authorization", "Basic " + QByteArray(APIKEYS_CLOUDANT_AUTH));

    QNetworkReply* reply = m_networkAccessManager->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(onResponse()));
    reply->setParent(this);

    QTimer::singleShot(UTILS_CLOUDANT_GET_DOC_TIMEOUT, this, SLOT(onTimeout()));
}

void CloudantGetRandomLoveMessage::onResponse() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (reply) {
        if (reply->error() == QNetworkReply::NoError) {
            const int available = reply->bytesAvailable();
            if (available > 0) {
                const QByteArray buffer(reply->readAll());
                JsonDataAccess ja;
                QVariantMap response = ja.loadFromBuffer(buffer).toMap();
                debugv(response);

                if (response.contains("rows")) {
                    m_allDocs = response;
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

void CloudantGetRandomLoveMessage::onResponseSuccess(QVariantMap response) {
    Q_UNUSED(response);
    if (m_randomLoveMessageRequested) {
        this->getRandomLoveMessageAsync();
    }
}

void CloudantGetRandomLoveMessage::getRandomLoveMessageAsync() {
    if (m_allDocs.isEmpty() || m_allDocs["rows"].toList().isEmpty()) {
        m_randomLoveMessageRequested = true;
        this->getAllDocs();
    }
    else {
        QVariantList rows = m_allDocs["rows"].toList();
        int randomIndex = rand() % rows.size();
        QVariantMap randomDoc = rows.at(randomIndex).toMap()["doc"].toMap();
        QString message = randomDoc["message"].toString();
        bool banned = randomDoc.contains("banned");

        rows.removeAt(randomIndex);
        m_allDocs["rows"] = rows;

        QSettings settings;
        settings.sync();
        QString messageFromThisUser = settings.value("message", "").toString();

        if (!message.isEmpty() && (message != messageFromThisUser) && !banned) {
            emit randomLoveMessageRetrieved(message);
        }
        else {
            this->getRandomLoveMessageAsync();
        }
    }
}

void CloudantGetRandomLoveMessage::onTimeout() {
    emit responseError("Request timed out");
}
