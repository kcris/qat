#-------------------------------------------------
#
# Project created by QtCreator 2014-06-18T23:10:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Qat
TEMPLATE = app

SOURCES += main.cpp\
    ui/mainwindow.cpp \
    ui/OptionsDialog.cpp \
    Catalog.cpp \
    TreeModel.cpp

HEADERS += \
    ui/mainwindow.h \
    ui/OptionsDialog.h \
    Catalog.h \
    TreeModel.h \
    TreeModelItem.h

FORMS += \
    ui/mainwindow.ui \
    ui/OptionsDialog.ui
