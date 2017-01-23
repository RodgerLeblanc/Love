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

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <src/HeadlessCommunication/HeadlessCommunication.h>

#include <QObject>
#include <QSettings>

namespace bb {
    namespace cascades {
        class LocaleHandler;
    }
    namespace system {
        class InvokeManager;
        class InvokeRequest;
    }
}

class QTranslator;

/*!
 * @brief Application UI object
 *
 * Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class ApplicationUI: public QObject
{
    Q_OBJECT
public:
    ApplicationUI();
    virtual ~ApplicationUI();

    Q_INVOKABLE QVariant getSettings(const QString& key, const QVariant& defaultValue);
    Q_INVOKABLE void sendToHL(const QString& action, const QString& data = "");

private slots:
    void handleInvoke(const bb::system::InvokeRequest & request);
    void onReceivedData(QString data);
    void onSystemLanguageChanged();

private:
    void emitSignalForStep(int step, bool success, QVariantMap extras);

    HeadlessCommunication* m_headlessCommunication;
    bb::system::InvokeManager* m_invokeManager;
    bb::cascades::LocaleHandler* m_localeHandler;
    QTranslator* m_translator;

    QSettings m_settings;

signals:
    void checkingIfDocumentExistDone(bool success);
    void languageDetected(bool success, QString language);
    void messageNotUpdated(const QString errorMessage);
    void messageUpdateAborted(const QString errorMessage);
    void messageUpdatedSuccessfully();
    void profanityAndAnonimityChecked(bool success, QString illegalWords);
    void updatingDocumentDone(bool success);
};

#endif /* ApplicationUI_HPP_ */
