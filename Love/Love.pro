APP_NAME = Love
HEADLESS_SERVICE = $${APP_NAME}Service

CONFIG += qt warn_on cascades10

include(config.pri)
INCLUDEPATH += ../../$${HEADLESS_SERVICE}

LIBS += -lbb -lbbdata -lbbsystem
QT += network
