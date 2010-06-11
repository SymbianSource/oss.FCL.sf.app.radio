#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
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

include(../../buildflags.pri)

TEMPLATE    = lib
TARGET      = radioenginewrapper_stub
CONFIG      += dll
DEFINES     += BUILD_WRAPPER_DLL
win32:DESTDIR = ../bin

# Wrapper does not depend on QtGui or Orbit
QT          = core
CONFIG      -= gui hb
USE_DUMMY_RADIO_DATA:QT += xml

symbian: {
    DEFINES += SYMBIAN
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    
    INCLUDEPATH += ../../../radioengine/utils/api
    INCLUDEPATH += ../../../radioengine/settings/api
    INCLUDEPATH += ../../../radioengine/engine/api
}

win32: {
    INCLUDEPATH += inc/win32
}
INCLUDEPATH += ../inc
INCLUDEPATH += ../commoninc


symbian:LIBS *= -lradioengineutils_stub
symbian:LIBS *= -lradioenginesettings_stub
symbian:LIBS *= -lradioengine_stub
symbian:LIBS *= -lflogger

DEPENDPATH += $$INCLUDEPATH ../src

# $$_PRO_FILE_PWD_ points to the directory of the pro file
MOC_DIR = $$_PRO_FILE_PWD_/tmp

# Input
HEADERS += radiowrapperexport.h
HEADERS += radio_global.h
HEADERS += radiologger.h
HEADERS += radioenginewrapper.h
HEADERS += radiosettings.h
HEADERS += radiostationhandlerif.h

INTERNAL_HEADERS += radiosettings_p.h

symbian: {
    HEADERS += cradioenginehandler.h
    HEADERS += mradioenginehandlerobserver.h
    HEADERS += radiofrequencyscanninghandler.h
    HEADERS += radiocontroleventlistener.h
    HEADERS += radiordslistener.h
    
    INTERNAL_HEADERS += radioenginewrapper_p.h
}

#USE_DUMMY_RADIO_DATA:HEADERS += t_radiodataparser.h

win32:HEADERS += radioenginewrapper_win32_p.h
win32:HEADERS += $$INTERNAL_HEADERS

SOURCES += radiosettings.cpp
SOURCES += radiologger.cpp

symbian: {
    SOURCES += radiosettings_p.cpp
    SOURCES += radioenginewrapper.cpp
    SOURCES += radioenginewrapper_p.cpp
    SOURCES += cradioenginehandler.cpp
    SOURCES += radiofrequencyscanninghandler.cpp
    SOURCES += radiocontroleventlistener.cpp
    SOURCES += radiordslistener.cpp
}

#USE_DUMMY_RADIO_DATA:SOURCES += t_radiodataparser.cpp

win32:SOURCES += radiosettings_win32_p.cpp
win32:SOURCES += radioenginewrapper_win32.cpp
win32:SOURCES += radioenginewrapper_win32_p.cpp
