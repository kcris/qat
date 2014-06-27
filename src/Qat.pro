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
        mainwindow.cpp \
    OptionsDialog.cpp \
    Catalog.cpp \
    TreeModel.cpp

HEADERS  += mainwindow.h \
    CollectionModel.h \
    OptionsDialog.h \
    Catalog.h \
    TreeModelItem.h \
    TreeModel.h

FORMS    += mainwindow.ui \
    OptionsDialog.ui
