#-------------------------------------------------
#
# Project created by QtCreator 2015-12-09T13:38:40
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += C:\Qwt\include
LIBS+= -L"C:\Qwt\lib" -lqwt
LIBS+= -L"C:\Qwt\lib" -lqwtd

TARGET = dielectricData
TEMPLATE = app
//CONFIG += qaxcontainer

SOURCES += main.cpp\
        dataprocessing.cpp \
    datainput.cpp \
    dataanalysis.cpp \
    qtexcel.cpp

HEADERS  += dataprocessing.h \
    datainput.h \
    dataanalysis.h \
    qtexcel.h

FORMS    += dataprocessing.ui \
    datainput.ui \
    dataanalysis.ui

RESOURCES += \
    image.qrc

RC_FILE = myapp.rc
