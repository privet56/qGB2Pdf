QT       += core gui webenginewidgets webengine

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    def.cpp \
    gbworker.cpp \
    gbwriter.cpp \
    main.cpp \
    mainwindow.cpp \
    util/anibutton.cpp \
    util/anilabel.cpp \
    util/cfg.cpp \
    util/emitter.cpp \
    util/f.cpp \
    util/logger.cpp \
    util/logwnd.cpp \
    util/logwndcontrol.cpp \
    util/str.cpp \
    util/tray.cpp \
    util/w.cpp

HEADERS += \
    def.h \
    gbworker.h \
    gbwriter.h \
    mainwindow.h \
    res.rc \
    resource.h \
    util/anibutton.h \
    util/anilabel.h \
    util/cfg.h \
    util/emitter.h \
    util/f.h \
    util/logger.h \
    util/logwnd.h \
    util/logwndcontrol.h \
    util/str.h \
    util/tray.h \
    util/w.h

FORMS += \
    mainwindow.ui \
    util/logwndcontrol.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += res.qrc
DISTFILES += res/ico.ico
RC_FILE = res.rc

win32: LIBS += -luser32
