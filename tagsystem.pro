#-------------------------------------------------
#
# Project created by QtCreator 2018-08-23T16:18:04
#
#-------------------------------------------------

QT       -= gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += xml websockets

TARGET = tagsystem
TEMPLATE = lib

DEFINES += TAGSYSTEM_LIBRARY


release: TARGET = tagsystem
debug: TARGET = tagsystemd

DESTDIR = $$(DEV_LIBS)

release: BUILDDIR = build/release
debug:   BUILDDIR = build/debug

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
        tagsystem.cpp \
    tag.cpp \
    taglist.cpp \
    taglistview.cpp \
    taglisttablemodel.cpp \
    tagselectview.cpp \
    tagsocket.cpp \
    tagsocketlist.cpp

HEADERS += \
        tagsystem.h \
        tagsystem_global.h \ 
    tag.h \
    taglist.h \
    taglistview.h \
    taglisttablemodel.h \
    tagselectview.h \
    tagsocket.h \
    tagsocketlist.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    tagselectview.ui
