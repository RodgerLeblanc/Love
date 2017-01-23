/*
 * Copyright (c) 2013-2015 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "service.hpp"

#include <bb/Application>
#include <bb/platform/Notification>
#include <bb/platform/NotificationDefaultApplicationSettings>
#include <bb/system/InvokeManager>

#include <QTimer>
#include <bb/data/JsonDataAccess>
#include <bb/PpsObject>

using namespace bb::data;
using namespace bb::platform;
using namespace bb::system;

Service::Service() :
        QObject(),
        m_cloudantGetRandomLoveMessage(new CloudantGetRandomLoveMessage(this)),
        m_headlessCommunication(new HeadlessCommunication(this)),
        m_invokeManager(new InvokeManager(this)),
        m_messageUpdateHandler(new MessageUpdateHandler(this))
{
    debugm("Starting headless");

    QDate now = QDate::currentDate();
    QDate feb14 = QDate(2017, 2, 14);

    if (now > feb14) {
        this->onRandomLoveMessageRetrieved("Valentine's day is over, but keep on loving BlackBerry 10 forever!\n\nYou can uninstall this app, it won't do anything else from now on.");
        QTimer::singleShot(5000, bb::Application::instance(), SLOT(quit()));
        return;
    }

    // Add this one line of code to all your projects, it will save you a ton of problems
    // when dealing with foreign languages. No more ??? characters.
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    m_invokeManager->connect(m_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
            this, SLOT(handleInvoke(const bb::system::InvokeRequest&)));

    connect(m_cloudantGetRandomLoveMessage, SIGNAL(randomLoveMessageRetrieved(QString)), this, SLOT(onRandomLoveMessageRetrieved(QString)));
    connect(m_headlessCommunication, SIGNAL(receivedData(QString)), this, SLOT(onReceivedData(QString)));
//    connect(m_messageUpdateHandler, SIGNAL(errorReadingDoc(const QString&, const QVariantMap&)), this, SLOT(onErrorReadingDoc(const QString&, const QVariantMap&)))
    connect(m_messageUpdateHandler, SIGNAL(errorUpdatingDoc(const QString&, const QVariantMap&)), this, SLOT(onErrorUpdatingDoc(const QString&, const QVariantMap&)));
    connect(m_messageUpdateHandler, SIGNAL(stateUpdated(UpdateDocSteps, bool, QVariantMap)), this, SLOT(onStateUpdated(UpdateDocSteps, bool, QVariantMap)));
    connect(m_messageUpdateHandler, SIGNAL(updateAborted(const QString)), this, SLOT(onUpdateAborted(const QString)));
    connect(m_messageUpdateHandler, SIGNAL(updateSuccessfully(const QVariantMap&, const QString&)), this, SLOT(onUpdateSuccessfully(const QVariantMap&, const QString&)));


    NotificationDefaultApplicationSettings notificationDefaultApplicationSettings;
    notificationDefaultApplicationSettings.setPreview(NotificationPriorityPolicy::Allow);
    notificationDefaultApplicationSettings.apply();

    srand(QDateTime::currentMSecsSinceEpoch());
    QTimer::singleShot(this->getRandomInterval(), this, SLOT(onSendLoveTimeout()));

    // Next code will start LoveService on Feb 14 in case it gets terminated
    InvokeDateTime invokeFeb14(QDate(2017, 2, 14), QTime(8, 0), "");
    InvokeTimerRequest invokeTimerRequest("loveService", invokeFeb14, "com.CellNinja.LoveService");
    InvokeReply* m_invokeReply = m_invokeManager->registerTimer(invokeTimerRequest);
    connect(m_invokeReply, SIGNAL(finished()), m_invokeReply, SLOT(deleteLater()));
}

int Service::getRandomInterval() {
    return ((rand() % 20) + 10) * 60 * 1000; // Random minute between 10 and 29
//    return ((rand() % 10) + 10) * 1000; // Random second between 10 and 19
//    return 5 * 1000;
}

void Service::handleInvoke(const bb::system::InvokeRequest & request) {
    QString action = request.action().split(".").last();
    QString data = QString::fromUtf8(request.data());

    if (action == UTILS_START_HEADLESS) {
        debugm("Received START invocation");
    }
    else if (action == UTILS_UPDATE_MESSAGE) {
        m_settings.sync();
        QString id = m_settings.value("_id", "").toString();
        m_messageUpdateHandler->updateMessage(id, data);
    }
    else if (action == UTILS_SHUTDOWN_APP) {
        bb::Application::instance()->quit();
    }
    else if (action == UTILS_RESET_COUNTS) {
        m_settings.sync();
        m_settings.setValue("messageUpdateCount", 0);
        m_settings.setValue("illegalWordsFoundCount", 0);
        m_settings.sync();

        m_headlessCommunication->sendMessage(UTILS_SHUTDOWN_APP, QVariantMap());
        bb::Application::instance()->quit();
    }
    else if (action == UTILS_RESET_ALL) {
        m_settings.sync();
        m_settings.clear();
        m_settings.sync();

        m_headlessCommunication->sendMessage(UTILS_SHUTDOWN_APP, QVariantMap());
        bb::Application::instance()->quit();
    }
    else if (action == UTILS_USER_REQUEST_RESET) {
        m_settings.sync();
        QString id = m_settings.value("_id", "").toString();

        RequestResetHandler* requestResetHandler = new RequestResetHandler(id, this);
        connect(requestResetHandler, SIGNAL(requestResetResponse(bool, QString)), this, SLOT(onRequestResetResponse(bool, QString)));
        requestResetHandler->requestReset();
    }
}

void Service::onErrorUpdatingDoc(const QString& errorMessage, const QVariantMap& originalDoc) {
    debugv(errorMessage);
    debugv(originalDoc);

    QVariantMap data;
    data["errorMessage"] = errorMessage;
    m_headlessCommunication->sendMessage(UTILS_MESSAGE_NOT_UPDATED, data);
}

void Service::onRandomLoveMessageRetrieved(QString message) {
    Notification::clearEffectsForAll();

    Notification* notification = new Notification(this);
    const QString iconPath = QString::fromUtf8("file://%1/app/public/native/icon_144.png").arg(QDir::currentPath());
    notification->setIconUrl(iconPath);
    notification->setTitle(bb::Application::applicationName());
    notification->setBody(message);

    notification->notify();

    Talk2WatchApi* t2w = new Talk2WatchApi(this);
    t2w->sendBbmMessage(message, bb::Application::applicationName());
    t2w->deleteLater();
}

void Service::onReceivedData(QString data) {
    debugv(data);

    QVariantMap map = bb::PpsObject::decode(data.toUtf8());
    QString reason = map["reason"].toString();

    if (reason == "REASON_SHUTDOWN") {
        bb::Application::instance()->quit();
        return;
    }
}

void Service::onRequestResetResponse(bool success, QString message) {
    debugv(success);

    sender()->deleteLater();

    this->onRandomLoveMessageRetrieved(QString(success ? "" : "ERROR: ") + message);

    if (success) {
        m_settings.sync();
        m_settings.setValue("messageUpdateCount", 0);
        m_settings.setValue("illegalWordsFoundCount", 0);
        m_settings.sync();

        m_headlessCommunication->sendMessage(UTILS_SHUTDOWN_APP, QVariantMap());
        bb::Application::instance()->quit();
    }
}

void Service::onSendLoveTimeout() {
    QDateTime now = QDateTime::currentDateTime();
    QDateTime loveStart = QDateTime(QDate(2017, 2, 14), QTime(8, 0));
    QDateTime loveEnd = QDateTime(QDate(2017, 2, 14), QTime(21, 0));

    if ((now > loveStart) && (now < loveEnd)) {
        m_cloudantGetRandomLoveMessage->getRandomLoveMessageAsync();
    }

    QTimer::singleShot(this->getRandomInterval(), this, SLOT(onSendLoveTimeout()));
}

void Service::onStateUpdated(UpdateDocSteps step, bool success, QVariantMap extras) {
    QVariantMap infos;
    infos.insert("step", (int)step);
    infos.insert("success", success);
    infos.insert("extras", extras);
    m_headlessCommunication->sendMessage(UTILS_SIGNAL_FROM_HL, infos);
}

void Service::onUpdateAborted(const QString errorMessage) {
    debugv(errorMessage);

    QVariantMap extras;
    extras.insert("errorMessage", errorMessage);

    m_headlessCommunication->sendMessage(UTILS_MESSAGE_UPDATE_ABORTED, extras);
}

void Service::onUpdateSuccessfully(const QVariantMap& cloudantResponse, const QString& message) {
    debugv(cloudantResponse);
    debugv(message);

    m_settings.sync();
    m_settings.setValue("message", message);
    int messageUpdateCount = m_settings.value("messageUpdateCount", 0).toInt();
    messageUpdateCount++;
    m_settings.setValue("messageUpdateCount", messageUpdateCount);
    m_settings.sync();

    m_headlessCommunication->sendMessage(UTILS_MESSAGE_UPDATED_SUCCESSFULLY, QVariantMap());
}
