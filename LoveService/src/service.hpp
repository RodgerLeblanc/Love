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

#ifndef SERVICE_H_
#define SERVICE_H_

#include "src/MessageUpdateHandler.h"
#include "src/RequestResetHandler.h"
#include <src/HeadlessCommunication/HeadlessCommunication.h>
#include <src/Pebble/Talk2WatchApi.h>
#include "src/Utils/Utils.hpp"
#include "src/WebCall/CloudantGetRandomLoveMessage.h"

#include <QObject>
#include <QSettings>

namespace bb {
    class Application;
    namespace platform {
        class Notification;
    }
    namespace system {
        class InvokeManager;
        class InvokeRequest;
    }
}

class Service: public QObject
{
    Q_OBJECT
public:
    Service();
    virtual ~Service() {}

private slots:
    void handleInvoke(const bb::system::InvokeRequest &);
    void onErrorUpdatingDoc(const QString&, const QVariantMap&);
    void onRandomLoveMessageRetrieved(QString message);
    void onReceivedData(QString data);
    void onRequestResetResponse(bool success, QString message);
    void onSendLoveTimeout();
    void onStateUpdated(UpdateDocSteps step, bool success, QVariantMap extras);
    void onUpdateAborted(const QString errorMessage);
    void onUpdateSuccessfully(const QVariantMap& cloudantResponse, const QString& message);

private:
    int getRandomInterval();

    CloudantGetRandomLoveMessage* m_cloudantGetRandomLoveMessage;
    HeadlessCommunication* m_headlessCommunication;
    bb::system::InvokeManager * m_invokeManager;
    MessageUpdateHandler* m_messageUpdateHandler;
    QSettings m_settings;
};

#endif /* SERVICE_H_ */
