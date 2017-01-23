/*
 * Utils.hpp
 *
 *  Created on: 2017-01-17
 *      Author: Roger
 */

// #include "src/Utils/Utils.hpp"

#ifndef UTILS_HPP_
#define UTILS_HPP_


#define debug() \
    qDebug() << QString("%1() [%2]") \
        .arg(__FUNCTION__) \
        .arg(__LINE__);

#define debugm(message) \
    qDebug() << message;

#define debugv(value) \
    qDebug() << QString("%1() [%2]") \
        .arg(__FUNCTION__) \
        .arg(__LINE__) \
        << #value << "=" << value;


#define UTILS_UPDATE_MESSAGE                        "UPDATE_MESSAGE"
#define UTILS_START_HEADLESS                        "START"
#define UTILS_SHUTDOWN_APP                          "SHUTDOWN"
#define UTILS_RESET_ALL                             "RESET_ALL"
#define UTILS_RESET_COUNTS                          "RESET_COUNTS"
#define UTILS_USER_REQUEST_RESET                    "USER_REQUEST_RESET"
#define UTILS_MESSAGE_NOT_UPDATED                   "MESSAGE_NOT_UPDATED"
#define UTILS_MESSAGE_UPDATED_SUCCESSFULLY          "MESSAGE_UPDATED_SUCCESSFULLY"
#define UTILS_MESSAGE_UPDATE_ABORTED                "MESSAGE_UPDATE_ABORTED"
#define UTILS_UPDATE_MESSAGE                        "UPDATE_MESSAGE"
#define UTILS_UPDATE_MESSAGE                        "UPDATE_MESSAGE"
#define UTILS_UPDATE_MESSAGE                        "UPDATE_MESSAGE"
#define UTILS_SIGNAL_FROM_HL                        "SIGNAL_FROM_HL"
#define UTILS_LOVE_URI_SCHEME                       "loveApp://"
#define UTILS_UNKNOWN_LANGUAGE                      ""

#define UTILS_AZURE_DETECT_LANGUAGE_URL             "https://westus.api.cognitive.microsoft.com/contentmoderator/moderate/v1.0/ProcessText/DetectLanguage/?"
#define UTILS_AZURE_SCREEN_URL                      "https://westus.api.cognitive.microsoft.com/contentmoderator/moderate/v1.0/ProcessText/Screen/?"
#define UTILS_CLOUDANT_DATABASE_URL                 "https://rodgerleblanc.cloudant.com/loveblackberry/"

#define UTILS_AZURE_LANGUAGE_TIMEOUT                (1000 * 10) // 10 seconds
#define UTILS_AZURE_PROFANITY_TIMEOUT               (1000 * 10) // 10 seconds
#define UTILS_CLOUDANT_GET_DOC_TIMEOUT              (1000 * 20) // 20 seconds
#define UTILS_CLOUDANT_UPDATE_DOC_TIMEOUT           (1000 * 20) // 20 seconds


enum UpdateDocSteps {
    DetectLanguage = 0,
    CheckProfanityAndAnonymity = 1,
    GetExistingDocument = 2,
    UpdateDocument = 3,
    UnknownStep = 4
};



#endif /* UTILS_HPP_ */
