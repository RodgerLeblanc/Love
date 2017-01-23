APP_NAME = LoveService

CONFIG += qt warn_on

include(config.pri)

LIBS += -lbb -lbbdata -lbbsystem -lbbplatform
QT += sql declarative network
