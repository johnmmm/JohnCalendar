QT       += core gui xml
linux { QT += x11extras }

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = JohnCalendar
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    dragthings.cpp \
    storage.cpp \
    calendar.cpp

HEADERS += \
    mainwindow.h \
    dragthings.h \
    storage.h \
    calendar.h

TRANSLATIONS += \
    Johncalendar_zh.ts

linux { LIBS += -lXext }

RESOURCES += \
    src.qrc
