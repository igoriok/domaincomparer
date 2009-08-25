# -------------------------------------------------
# Project created by QtCreator 2009-05-20T14:18:52
# -------------------------------------------------
QT += network \
    sql
TARGET = DomainComparer
TEMPLATE = app
SOURCES += main.cpp \
    MainWindow.cpp \
    WebManager.cpp \
    WebResponse.cpp \
    DomainManager.cpp \
    UrlInfo.cpp \
    DatabaseManager.cpp \
    DomainInfo.cpp \
    DomainHelper.cpp
HEADERS += MainWindow.h \
    WebManager.h \
    WebResponse.h \
    DomainManager.h \
    UrlInfo.h \
    DatabaseManager.h \
    DomainInfo.h \
    DomainHelper.h
FORMS += MainWindow.ui \
    AddRange.ui
OTHER_FILES += db.s3db \
    TODO.txt
