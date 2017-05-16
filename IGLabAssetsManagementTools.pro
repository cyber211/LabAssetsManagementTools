#-------------------------------------------------
#
# Project created by QtCreator 2014-06-30T15:35:52
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IGLabAssetsManagementTools
TEMPLATE = app

include(./xlsx/qtxlsx.pri)


RC_FILE = Icon.rc


SOURCES += main.cpp\
        mainwindow.cpp \
        addnewitem.cpp \
    addnewequipment.cpp \
    updatecalverdatedlg.cpp \
    logindlg.cpp \
    moreinfordlg.cpp \
    globalvarial.cpp \
    smtp/emailaddress.cpp \
    smtp/mimemessage.cpp \
    smtp/mimemultipart.cpp \
    smtp/mimepart.cpp \
    smtp/mimetext.cpp \
    smtp/smtpclient.cpp \
    smtp/quotedprintable.cpp \
    smtp/mimeattachment.cpp \
    smtp/mimecontentformatter.cpp \
    smtp/mimefile.cpp \
    smtp/mimehtml.cpp \
    smtp/mimeinlinefile.cpp

HEADERS  += mainwindow.h \
        addnewitem.h \
    addnewequipment.h \
    typedef.h \
    updatecalverdatedlg.h \
    logindlg.h \
    moreinfordlg.h \
    main.h \
    globalvarial.h \
    smtp/emailaddress.h \
    smtp/mimemessage.h \
    smtp/mimemultipart.h \
    smtp/mimepart.h \
    smtp/mimetext.h \
    smtp/smtpclient.h \
    smtp/SmtpMime.h \
    smtp/quotedprintable.h \
    smtp/mimeattachment.h \
    smtp/mimecontentformatter.h \
    smtp/mimefile.h \
    smtp/mimehtml.h \
    smtp/mimeinlinefile.h

FORMS    += mainwindow.ui \
        addnewitem.ui \
    addnewequipment.ui \
    updatecalverdatedlg.ui \
    logindlg.ui \
    moreinfordlg.ui

RESOURCES += \
    Icon.qrc

OTHER_FILES += \
    Icon.rc \
    main.qss

