/*
 * MessageUpdateHandler.h
 *
 *  Created on: 2017-01-17
 *      Author: Roger
 */

#ifndef MESSAGEUPDATEHANDLER_H_
#define MESSAGEUPDATEHANDLER_H_

#include "src/Utils/Utils.hpp"

#include <QObject>
#include <QtNetwork>

class MessageUpdateHandler : public QObject
{
    Q_OBJECT

public:
    MessageUpdateHandler(QObject *_parent = NULL);
    virtual ~MessageUpdateHandler() {}

    void updateMessage(const QString& id, const QString& message);

private slots:
    void onCheckLanguageError(QString errorMessage, QString language);
    void onGetDocError(QString errorMessage);
    void onGetDocSuccess(QVariantMap doc);
    void onUpdateDocError(QString errorMessage);
    void onUpdateDocSuccess(QVariantMap doc);
    void onReturnedLanguage(QString language);
    void onScreenTextFinished(bool passed, QStringList illegalWords);

private:
    void doNextStep(QVariant optionalArgument = QVariant());
    void incrementStep();

    QString m_documentId;
    QString m_message;
    UpdateDocSteps m_updateDocSteps;

signals:
    void errorReadingDoc(const QString& errorMessage, const QVariantMap& originalDoc);
    void errorUpdatingDoc(const QString& errorMessage, const QVariantMap& originalDoc);
    void stateUpdated(UpdateDocSteps step, bool success, QVariantMap extras);
    void updateAborted(const QString errorMessage);
    void updateSuccessfully(const QVariantMap& cloudantResponse, const QString& message);
};

#endif /* MESSAGEUPDATEHANDLER_H_ */
