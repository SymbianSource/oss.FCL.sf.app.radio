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

TEMPLATE        = app
TARGET          = qtfmradio
TRANSLATIONS    += fmradio.ts 
win32:DESTDIR   = ../bin

symbian: {
    DEFINES += SYMBIAN
    TARGET.CAPABILITY = CAP_APPLICATION MultimediaDD
    TARGET.VID = VID_DEFAULT
    TARGET.UID3 = 0x101FF976 # Old visual radio uid
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x2400000
}

QT          = core
CONFIG      += hb service
CONFIG      -= gui network svg

# Service provider specific configuration.
SERVICE.FILE = resources/service_conf.xml
#SERVICE.OPTIONS = embeddable

INCLUDEPATH += . inc
INCLUDEPATH += ../radioenginewrapper/commoninc
INCLUDEPATH += ../radiouiengine/inc
INCLUDEPATH += ../radiowidgets/inc

win32:INCLUDEPATH += ../radioenginewrapper/inc
win32:LIBS += -L../bin

LIBS += -lradiouiengine
LIBS += -lradiowidgets

# UI does not normally depend on the wrapper, but it does on two special cases
# 1. Logging is enabled. The logger is in the wrapper module
# 2. Radio is built for win32 target. The test window needs to access wrapper
LOGGING_ENABLED:LIBS += -lradioenginewrapper
win32:LIBS += -lradioenginewrapper

DEPENDPATH  += $$INCLUDEPATH src

# Sources
HEADERS += radioapplication.h

win32:HEADERS += testwindow_win32.h

SOURCES += main.cpp
SOURCES += radioapplication.cpp

win32:SOURCES += testwindow_win32.cpp
