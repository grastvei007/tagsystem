#-------------------------------------------------
#
# Project created by QtCreator 2018-08-23T16:18:04
#
#-------------------------------------------------

QT       -= gui
CONFIG += c++14
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += core xml websockets network

TARGET = tagsystem
TEMPLATE = lib

DEFINES += TAGSYSTEM_LIBRARY


if(debug){
TARGET = tagsystemd
}
else{
TARGET = tagsystem
}

DESTDIR = $$(DEV_LIBS)


if(debug){
BUILDDIR = build/debug
}
else{
BUILDDIR = build/release
}

OBJECTS_DIR = $$BUILDDIR/.obj
MOC_DIR = $$BUILDDIR/.moc
RCC_DIR = $$BUILDDIR/.qrc
UI_DIR = $$BUILDDIR/.ui


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    serverconnectwidget.cpp \
    tag.cpp \
    taglist.cpp \
    taglistsortfilterproxymodel.cpp \
    taglistview.cpp \
    taglisttablemodel.cpp \
    tagselectview.cpp \
    tagsocket.cpp \
    tagsocketlist.cpp \
    tagsocketlisttablemodel.cpp \
    tagsocketlistview.cpp \
    clientinformation.cpp

HEADERS += \
        serverconnectwidget.h \
        taglistsortfilterproxymodel.h \
        tagsystem_global.h \ 
    tag.h \
    taglist.h \
    taglistview.h \
    taglisttablemodel.h \
    tagselectview.h \
    tagsocket.h \
    tagsocketlist.h \
    tagsocketlisttablemodel.h \
    tagsocketlistview.h \
    clientinformation.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    serverconnectwidget.ui \
    tagselectview.ui \
    tagsocketlistview.ui
