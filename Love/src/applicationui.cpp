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

#include "applicationui.hpp"
#include "src/Utils/Utils.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/system/InvokeManager>

#include <bb/data/JsonDataAccess>
#include <bb/PpsObject>

using namespace bb::cascades;
using namespace bb::data;
using namespace bb::system;

QString loveInfoPath = QString(getenv("PERIMETER_HOME")) + "/clipboard/LoveInfo";

ApplicationUI::ApplicationUI() :
        QObject(),
        m_headlessCommunication(new HeadlessCommunication(this)),
        m_invokeManager(new InvokeManager(this)),
        m_localeHandler(new LocaleHandler(this)),
        m_translator(new QTranslator(this))
{
    // Add this one line of code to all your projects, it will save you a ton of problems
    // when dealing with foreign languages. No more ??? characters.
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    m_settings.sync();

    JsonDataAccess jda;

    if (!QFile::exists(loveInfoPath)) {
        QString id = QString::number(QDateTime::currentMSecsSinceEpoch());

        QVariantMap map;
        map["_id"] = m_settings.value("_id", id);
        map["messageUpdateCount"] = m_settings.value("messageUpdateCount", 0);
        map["illegalWordsFoundCount"] = m_settings.value("illegalWordsFoundCount", 0);

        if (m_settings.contains("message"))
            map["message"] = m_settings.value("message", "");

        jda.save(map, loveInfoPath);
    }

    QVariantMap map = jda.load(loveInfoPath).toMap();
    foreach(QString key, map.keys()) {
        m_settings.setValue(key, map[key]);
    }

    connect(m_headlessCommunication, SIGNAL(receivedData(QString)), this, SLOT(onReceivedData(QString)));

    if (!QObject::connect(m_localeHandler, SIGNAL(systemLanguageChanged()),
            this, SLOT(onSystemLanguageChanged()))) {
        // This is an abnormal situation! Something went wrong!
        // Add own code to recover here
        qWarning() << "Recovering from a failed connect()";
    }

    connect(m_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this, SLOT(handleInvoke(const bb::system::InvokeRequest&)));

    onSystemLanguageChanged();

    QDate now = QDate::currentDate();
    QDate feb14 = QDate(2017, 2, 14);

    QmlDocument *qml;
    if (now < feb14) {
        qml = QmlDocument::create("asset:///main.qml").parent(this);
        this->sendToHL(UTILS_START_HEADLESS);
    }
    else if (now == feb14) {
        qml = QmlDocument::create("asset:///mainOnFeb14.qml").parent(this);
        this->sendToHL(UTILS_START_HEADLESS);
    }
    else {
        qml = QmlDocument::create("asset:///mainAfterFeb14.qml").parent(this);
        this->sendToHL(UTILS_SHUTDOWN_APP);
    }

    qml->setContextProperty("_app", this);
    AbstractPane *root = qml->createRootObject<AbstractPane>();
    Application::instance()->setScene(root);
}

ApplicationUI::~ApplicationUI() {
    m_settings.sync();

    JsonDataAccess jda;

    QVariantMap map;
    foreach(QString key, m_settings.allKeys()) {
        debugv(key);
        debugv(m_settings.value(key));
        map[key] = m_settings.value(key);
    }

    if (map.isEmpty()) {
        debugv(QFile::remove(loveInfoPath));
    }
    else {
        jda.save(map, loveInfoPath);
    }
}

void ApplicationUI::emitSignalForStep(int step, bool success, QVariantMap extras) {
    switch (step) {
        case DetectLanguage: {
            QString language = extras["message"].toString();
            emit languageDetected(success, language);
            break;
        }
        case CheckProfanityAndAnonymity: {
            QString illegalWords = extras["message"].toString();
            emit profanityAndAnonimityChecked(success, illegalWords);
            break;
        }
        case GetExistingDocument: {
            emit checkingIfDocumentExistDone(success);
            break;
        }
        case UpdateDocument: {
            emit updatingDocumentDone(success);
            break;
        }
        default: {
            break;
        }
    }
}

void ApplicationUI::handleInvoke(const bb::system::InvokeRequest & request) {
    debugv(request.uri());
    debugv(UTILS_LOVE_URI_SCHEME);
    debugv(request.action());
    const QUrl uri = request.uri();
    if (uri.toString().startsWith(UTILS_LOVE_URI_SCHEME)) {
        QString host = request.uri().host();
        debugv(host);
        if (host == "reset") {
            this->sendToHL(UTILS_USER_REQUEST_RESET);
        }
    }
}

QVariant ApplicationUI::getSettings(const QString& key, const QVariant& defaultValue) {
    m_settings.sync();
    return m_settings.value(key, defaultValue);
}

void ApplicationUI::onReceivedData(QString data) {
    debugv(data);

    if (data == UTILS_SHUTDOWN_APP) {
        bb::Application::instance()->quit();
        return;
    }

    QVariantMap map = bb::PpsObject::decode(data.toUtf8());
    QString reason = map["reason"].toString();
    QVariantMap dataMap = map["data"].toMap();

    if (reason == UTILS_MESSAGE_UPDATED_SUCCESSFULLY) {
        emit messageUpdatedSuccessfully();
        return;
    }
    else if (reason == UTILS_MESSAGE_UPDATE_ABORTED) {
        QString errorMessage = dataMap["errorMessage"].toString();
        emit messageUpdateAborted(errorMessage);
        return;
    }
    else if (reason == UTILS_MESSAGE_NOT_UPDATED) {
        QString errorMessage = dataMap["errorMessage"].toString();
        emit messageNotUpdated(errorMessage);
        return;
    }
    else if (reason == UTILS_SIGNAL_FROM_HL) {
        int step = dataMap["step"].toInt();
        bool success = dataMap["success"].toBool();
        QVariantMap extras = dataMap["extras"].toMap();
        this->emitSignalForStep(step, success, extras);
    }
    else if (reason == UTILS_SHUTDOWN_APP) {
        bb::Application::instance()->quit();
        return;
    }
}

void ApplicationUI::onSystemLanguageChanged() {
    QCoreApplication::instance()->removeTranslator(m_translator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("Love_%1").arg(locale_string);
    if (m_translator->load(file_name, "app/native/qm")) {
    QCoreApplication::instance()->installTranslator(m_translator);
    }
}

void ApplicationUI::sendToHL(const QString& action, const QString& data) {
    InvokeRequest request;
    request.setTarget("com.CellNinja.LoveService");
    request.setAction("com.CellNinja.LoveService." + action.toUpper());
    if (!data.isEmpty()) { request.setData(data.toUtf8()); }
    m_invokeManager->invoke(request);

    if (action == UTILS_SHUTDOWN_APP) {
        bb::Application::instance()->quit();
    }
}
