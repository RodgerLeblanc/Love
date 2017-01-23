/*
 * RequestResetHandler.h
 *
 *  Created on: 2017-01-17
 *      Author: Roger
 */

#ifndef REQUESTRESETHANDLER_H_
#define REQUESTRESETHANDLER_H_

#include "src/Utils/Utils.hpp"

#include <QObject>
#include <QtNetwork>

class RequestResetHandler : public QObject
{
    Q_OBJECT

public:
    RequestResetHandler(QString& id, QObject *_parent = NULL);
    virtual ~RequestResetHandler() {}

    void requestReset();

private slots:
    void onGetDocError(QString errorMessage);
    void onGetDocSuccess(QVariantMap doc);
    void onUpdateDocError(QString errorMessage);
    void onUpdateDocSuccess(QVariantMap doc);

private:
    QString m_userDocumentId;

signals:
    void requestResetResponse(bool success, QString message);
};

#endif /* REQUESTRESETHANDLER_H_ */
