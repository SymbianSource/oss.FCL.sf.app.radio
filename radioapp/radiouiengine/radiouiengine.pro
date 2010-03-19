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
TARGET      = radiouiengine
CONFIG      += dll
DEFINES     += BUILD_UI_ENGINE_DLL
win32:DESTDIR = ../bin

# Wrapper does not depend on QtGui or Orbit
QT          = core gui
CONFIG      -= hb

symbian: {
    DEFINES += SYMBIAN
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
}

INCLUDEPATH += inc
INCLUDEPATH += ../radiopresetstorage/inc
INCLUDEPATH += ../radioenginewrapper/inc
INCLUDEPATH += ../radioenginewrapper/commoninc

win32:LIBS += -L../bin

LIBS += -lradiopresetstorage
LIBS += -lradioenginewrapper

USE_MOBILE_EXTENSIONS_API {
    symbian:LIBS += -lxqprofile
}

DEPENDPATH += $$INCLUDEPATH src

# $$_PRO_FILE_PWD_ points to the directory of the pro file
MOC_DIR = $$_PRO_FILE_PWD_/tmp

# Input
HEADERS += radiostation.h
HEADERS += radiostationmodel.h
HEADERS += radiostationfiltermodel.h
HEADERS += radiouiengine.h
HEADERS += radioplaylogitem.h
HEADERS += radioplaylogmodel.h
HEADERS += radiolocalization.h
HEADERS += radiouiengineexport.h

INTERNAL_HEADERS += radiostation_p.h
INTERNAL_HEADERS += radioplaylogitem_p.h
INTERNAL_HEADERS += radioplaylogmodel_p.h
INTERNAL_HEADERS += radiouiengine_p.h
INTERNAL_HEADERS += radiostationmodel_p.h

win32:HEADERS += $$INTERNAL_HEADERS

SOURCES += radiostation.cpp
SOURCES += radiostation_p.cpp
SOURCES += radiostationmodel.cpp
SOURCES += radiostationmodel_p.cpp
SOURCES += radiostationfiltermodel.cpp
SOURCES += radiouiengine.cpp
SOURCES += radiouiengine_p.cpp
SOURCES += radioplaylogitem.cpp
SOURCES += radioplaylogitem_p.cpp
SOURCES += radioplaylogmodel.cpp
SOURCES += radioplaylogmodel_p.cpp
