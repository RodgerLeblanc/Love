/*
 * MessageUpdateHandler.cpp
 *
 *  Created on: 2017-01-17
 *      Author: Roger
 */

#include "src/MessageUpdateHandler.h"

#include "src/WebCall/AzureCheckLanguage.h"
#include "src/WebCall/AzureScreenText.h"
#include "src/WebCall/CloudantGetDoc.h"
#include "src/WebCall/CloudantUpdateDoc.h"

#include <bb/data/JsonDataAccess>
#include <QSettings>

using namespace bb::data;

MessageUpdateHandler::MessageUpdateHandler(QObject *_parent):
    QObject(_parent),
    m_updateDocSteps(UnknownStep)
{
    debug();
}

void MessageUpdateHandler::doNextStep(QVariant optionalArgument) {
    this->incrementStep();

    debugv(m_updateDocSteps);
    switch(m_updateDocSteps) {
        case DetectLanguage: {
            QString language = optionalArgument.toString();
            AzureCheckLanguage* azureCheckLanguage = new AzureCheckLanguage(m_message, this);
            connect(azureCheckLanguage, SIGNAL(returnedLanguage(QString)), this, SLOT(onReturnedLanguage(QString)));
            connect(azureCheckLanguage, SIGNAL(responseError(QString, QString)), this, SLOT(onCheckLanguageError(QString, QString)));
            break;
        }
        case CheckProfanityAndAnonymity: {
            QString language = optionalArgument.toString();
            AzureScreenText* azureScreenText = new AzureScreenText(m_message, language, this);
            connect(azureScreenText, SIGNAL(finished(bool, QStringList)), this, SLOT(onScreenTextFinished(bool, QStringList)));
            break;
        }
        case GetExistingDocument: {
            CloudantGetDoc* cloudantGetDoc = new CloudantGetDoc(m_documentId, this);
            connect(cloudantGetDoc, SIGNAL(responseError(QString)), this, SLOT(onGetDocError(QString)));
            connect(cloudantGetDoc, SIGNAL(responseSuccess(QVariantMap)), this, SLOT(onGetDocSuccess(QVariantMap)));
            break;
        }
        case UpdateDocument: {
            QVariantMap doc = optionalArgument.toMap();
            CloudantUpdateDoc* cloudantUpdateDoc = new CloudantUpdateDoc(m_documentId, doc, this);
            connect(cloudantUpdateDoc, SIGNAL(responseError(QString)), this, SLOT(onUpdateDocError(QString)));
            connect(cloudantUpdateDoc, SIGNAL(responseSuccess(QVariantMap)), this, SLOT(onUpdateDocSuccess(QVariantMap)));
            break;
        }
        default: {
            m_updateDocSteps = UnknownStep;
            break;
        }
    }
}

void MessageUpdateHandler::incrementStep() {
    if (m_updateDocSteps != UnknownStep) {
        int i = (int)m_updateDocSteps;
        m_updateDocSteps = (UpdateDocSteps)(i+1);
    }
    else {
        m_updateDocSteps = DetectLanguage;
    }
}

void MessageUpdateHandler::onCheckLanguageError(QString errorMessage, QString language) {
    debugv(errorMessage);
    debugv(language);
}

void MessageUpdateHandler::onGetDocError(QString errorMessage){
    debugv(errorMessage);

    bool success = false;
    QVariantMap extras;
    extras.insert("_id", m_documentId);
    extras.insert("message", m_message);
    emit stateUpdated(m_updateDocSteps, success, extras);

    sender()->deleteLater();

    this->doNextStep(extras);
}

void MessageUpdateHandler::onGetDocSuccess(QVariantMap doc){
    debugv(doc);
    doc["message"] = m_message;

    bool success = true;
    emit stateUpdated(m_updateDocSteps, success, doc);

    sender()->deleteLater();

    this->doNextStep(doc);
}

void MessageUpdateHandler::onReturnedLanguage(QString language) {
    debugv(language);

    bool success = true;
    QVariantMap extras;
    extras.insert("message", language.toUpper());
    emit stateUpdated(m_updateDocSteps, success, extras);

    sender()->deleteLater();

    this->doNextStep(language == UTILS_UNKNOWN_LANGUAGE ? "eng" : language);
}

void MessageUpdateHandler::onScreenTextFinished(bool passed, QStringList illegalWords) {
    debugv(passed);
    debugv(illegalWords);

    bool success = passed;
    QVariantMap extras;
    extras.insert("message", illegalWords.join(", "));
    emit stateUpdated(m_updateDocSteps, success, extras);

    if (illegalWords.isEmpty()) {
        this->doNextStep();
    }
    else {
        QSettings settings;
        settings.sync();
        int illegalWordsFoundCount = settings.value("illegalWordsFoundCount", 0).toInt();
        illegalWordsFoundCount++;
        settings.setValue("illegalWordsFoundCount", illegalWordsFoundCount);

        QString errorMessage;
        if (illegalWordsFoundCount <= 1) {
            errorMessage = "Illegal word" + QString((illegalWords.size() > 1) ? "s" : "") + " found: " + illegalWords.join(", ") + "\n\nWe enforce a clean language and anonymity. If you use bad language or personally identifiable information again, you'll be banned from using the app.";
        }
        else {
            errorMessage = "Illegal word" + QString((illegalWords.size() > 1) ? "s" : "") + " found: " + illegalWords.join(", ") + "\n\nYou tried to publish inappropriate language or personnaly identifiable information twice, you are now banned from using the app.";
            settings.setValue("messageUpdateCount", 2);
        }
        settings.sync();
        emit updateAborted(errorMessage);
    }

    sender()->deleteLater();
}

void MessageUpdateHandler::onUpdateDocError(QString errorMessage){
    debugv(errorMessage);

    bool success = false;
    QVariantMap extras;
    extras.insert("message", errorMessage);
    emit stateUpdated(m_updateDocSteps, success, extras);

    emit updateAborted("Error updating online database, make sure you have access to internet");

    sender()->deleteLater();

    m_updateDocSteps = UnknownStep;
}

void MessageUpdateHandler::onUpdateDocSuccess(QVariantMap doc){
    debugv(doc);

    bool success = true;
    QVariantMap extras;
    emit stateUpdated(m_updateDocSteps, success, extras);

    emit updateSuccessfully(doc, m_message);

    sender()->deleteLater();

    m_updateDocSteps = UnknownStep;
}

void MessageUpdateHandler::updateMessage(const QString& id, const QString& message) {
    debug();
    m_updateDocSteps = UnknownStep;

    m_documentId = id;
    m_message = message;
    debugv(m_documentId);
    debugv(m_message);

    this->doNextStep();
}
