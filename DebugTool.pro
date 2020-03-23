#-------------------------------------------------
#
# Project created by QtCreator 2019-10-26T16:56:46
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += xlsx

RC_ICONS = tools.ico                        #程序图标

DEFINES += YF

DEFINES += QWT_DLL

if(contains(CONFIG,debug)){
    LIBS+=-lqwtd
}else{
    LIBS+=-lqwt
}


if(contains(DEFINES,YF)){
    INCLUDEPATH += D:/Qt/Qt5.9.8/5.9.8/mingw53_32/include/QWT
    DEPENDPATH += D:/Qt/Qt5.9.8/5.9.8/mingw53_32/include/QWT
}else{
    INCLUDEPATH += S:/Qt/Qt5.9.8/5.9.8/mingw53_32/include/QWT
    DEPENDPATH += S:/Qt/Qt5.9.8/5.9.8/mingw53_32/include/QWT
}



greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DebugTool
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwidget.cpp \
    serialport.cpp \
    serialtool.cpp \
    serialthread.cpp \
    realtimewave.cpp \
    rssithread.cpp \
    logcapture.cpp \
    logcapturethread.cpp \
    textinfoinputdlg.cpp \
    tipsdialog.cpp

HEADERS += \
        mainwidget.h \
    serialport.h \
    serialtool.h \
    serialthread.h \
    realtimewave.h \
    rssithread.h \
    logcapture.h \
    logcapturethread.h \
    textinfoinputdlg.h \
    tipsdialog.h

FORMS += \
        mainwidget.ui \
    serialport.ui \
    serialtool.ui \
    realtimewave.ui \
    logcapture.ui \
    textinfoinputdlg.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    srcfile.qrc
