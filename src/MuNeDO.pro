QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


CONFIG += c++11

LIBS     += -lws2_32
LIBS     += -liphlpapi

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
	Basic/QIniFile.cpp \
	Basic/Utils.cpp \
	Basic/ping.cpp \
	Dialogs/DlgEditPingChart.cpp \
	Dialogs/DlgNewProject.cpp \
	ProjectManager.cpp \
    QBasicChartWidget.cpp \
	QMultipleHandleSlider.cpp \
	QPingChartWidget.cpp \
    QTabChartHolder.cpp \
	main.cpp \
    mainwindow.cpp

HEADERS += \
	Basic/QIniFile.h \
	Basic/Utils.h \
	Basic/ping.h \
	Dialogs/DlgEditPingChart.h \
	Dialogs/DlgNewProject.h \
	ProjectManager.h \
    QBasicChartWidget.h \
	QMultipleHandleSlider.h \
	QPingChartWidget.h \
    QTabChartHolder.h \
	mainwindow.h

FORMS += \
	Dialogs/DlgEditPingChart.ui \
	Dialogs/DlgNewProject.ui \
    mainwindow.ui

TRANSLATIONS += \
    MuNeDO_es_ES.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
	resources.qrc

DISTFILES += \
	ToDo.txt
