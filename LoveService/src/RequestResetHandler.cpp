/*
 * MessageUpdateHandler.cpp
 *
 *  Created on: 2017-01-17
 *      Author: Roger
 */

#include "src/RequestResetHandler.h"

#include "src/WebCall/CloudantGetDoc.h"
#include "src/WebCall/CloudantUpdateDoc.h"

#include <bb/data/JsonDataAccess>
#include <QSettings>

using namespace bb::data;

RequestResetHandler::RequestResetHandler(QString& id, QObject *_parent):
    QObject(_parent),
    m_userDocumentId(id)
{
}

void RequestResetHandler::onGetDocError(QString errorMessage){
    debugv(errorMessage);

    sender()->deleteLater();

    emit requestResetResponse(false, errorMessage);
}

void RequestResetHandler::onGetDocSuccess(QVariantMap doc){
    debugv(doc);

    sender()->deleteLater();

    if (!doc.contains("resetAllowed") || !doc["resetAllowed"].toBool()) {
        emit requestResetResponse(false, "Reset not allowed, contact info@cellninja.ca to ask for a reset.");
    }
    else {
        doc["resetAllowed"] = false;
        doc["lastRequestBy"] = m_userDocumentId;

        CloudantUpdateDoc* cloudantUpdateDoc = new CloudantUpdateDoc(UTILS_USER_REQUEST_RESET, doc, this);
        connect(cloudantUpdateDoc, SIGNAL(responseError(QString)), this, SLOT(onUpdateDocError(QString)));
        connect(cloudantUpdateDoc, SIGNAL(responseSuccess(QVariantMap)), this, SLOT(onUpdateDocSuccess(QVariantMap)));
    }
}

void RequestResetHandler::onUpdateDocError(QString errorMessage){
    debugv(errorMessage);

    sender()->deleteLater();

    emit requestResetResponse(false, errorMessage);
}

void RequestResetHandler::onUpdateDocSuccess(QVariantMap doc){
    debugv(doc);

    sender()->deleteLater();

    emit requestResetResponse(true, "Your reset request was accepted, you can restart the app.");
}

void RequestResetHandler::requestReset() {
    debug();
    CloudantGetDoc* cloudantGetDoc = new CloudantGetDoc(UTILS_USER_REQUEST_RESET, this);
    connect(cloudantGetDoc, SIGNAL(responseError(QString)), this, SLOT(onGetDocError(QString)));
    connect(cloudantGetDoc, SIGNAL(responseSuccess(QVariantMap)), this, SLOT(onGetDocSuccess(QVariantMap)));
}
