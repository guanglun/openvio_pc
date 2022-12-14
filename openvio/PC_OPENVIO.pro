#-------------------------------------------------
#
# Project created by QtCreator 2019-12-22T11:51:40
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PC_OPENVIO
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    winusbdriver.cpp \
    usbthread.cpp \
    easytool.cpp \
    image.cpp \
    glview.cpp \
    gldrow.cpp \
    imu.cpp \
    setting.cpp \
    findstr.cpp \
    log.cpp \
    formcamconfig.cpp \
    imuso3.cpp

HEADERS += \
        mainwindow.h \
    winusbdriver.h \
    libusb.h \
    usbthread.h \
    workspace.h \
    easytool.h \
    image.h \
    glview.h \
    gldrow.h \
    imu.h \
    setting.h \
    findstr.h \
    log.h \
    formcamconfig.h \
    imuso3.h

FORMS += \
        mainwindow.ui \
    formcamconfig.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


win32{
	LIBS +=		-lopengl32 \
				-lglu32

    LIBS += "$$PWD/lib/libusb-1.0.a"
    
    INCLUDEPATH += "$$PWD/eigen-3.2.10"
    INCLUDEPATH += "$$PWD/glut"
}

unix{
    LIBS +=     -lusb-1.0 \
                -lglut \
                -lGL \
                -lGLU

    INCLUDEPATH += ./eigen-3.2.10
}



