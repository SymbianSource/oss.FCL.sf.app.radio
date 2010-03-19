#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

include(../buildflags.pri)

TEMPLATE    = lib
TARGET      = radiowidgets
CONFIG      += dll
DEFINES     += BUILD_WIDGETS_DLL
win32:DESTDIR = ../bin

# Wrapper does not depend on QtGui or Orbit
QT          = core gui
CONFIG      += hb

symbian: {
    DEFINES += SYMBIAN
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
}

INCLUDEPATH += inc
INCLUDEPATH += ../radiouiengine/inc
INCLUDEPATH += ../radioenginewrapper/commoninc

win32:LIBS += -L../bin

LIBS += -lradiouiengine

LOGGING_ENABLED:LIBS += -lradioenginewrapper

DEPENDPATH += $$INCLUDEPATH src

# $$_PRO_FILE_PWD_ points to the directory of the pro file
MOC_DIR = $$_PRO_FILE_PWD_/tmp

# Input
HEADERS += radiowidgetsexport.h
HEADERS += radiomainwindow.h
HEADERS += radiouiutilities.h
HEADERS += radioviewbase.h
HEADERS += radioxmluiloader.h
HEADERS += radiotuningview.h
HEADERS += radiostripbase.h
HEADERS += radiofrequencystrip.h
HEADERS += radiofrequencyitem.h
HEADERS += radiostationsview.h
HEADERS += radiobannerlabel.h
HEADERS += radiowizardview.h
HEADERS += radiofrequencyscanner.h
HEADERS += radiocontextmenu.h
HEADERS += radiofadinglabel.h
HEADERS += radiostationcarousel.h
HEADERS += radiostationcontrolwidget.h
HEADERS += radioplaylogview.h

SOURCES += radiomainwindow.cpp
SOURCES += radiouiutilities.cpp
SOURCES += radioviewbase.cpp
SOURCES += radioxmluiloader.cpp
SOURCES += radiotuningview.cpp
SOURCES += radiostripbase.cpp
SOURCES += radiofrequencystrip.cpp
SOURCES += radiofrequencyitem.cpp
SOURCES += radiostationsview.cpp
SOURCES += radiobannerlabel.cpp
SOURCES += radiowizardview.cpp
SOURCES += radiofrequencyscanner.cpp
SOURCES += radiocontextmenu.cpp
SOURCES += radiofadinglabel.cpp
SOURCES += radiostationcarousel.cpp
SOURCES += radiostationcontrolwidget.cpp
SOURCES += radioplaylogview.cpp

RESOURCES += res/fmradioui.qrc
