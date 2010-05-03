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
QT          = core gui network
CONFIG      -= hb
CONFIG      += mobility
MOBILITY    = systeminfo

symbian: {
    DEFINES += SYMBIAN
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
}

INCLUDEPATH += inc
INCLUDEPATH += ../radiopresetstorage/inc
INCLUDEPATH += ../radioenginewrapper/inc
INCLUDEPATH += ../commoninc

win32:LIBS += -L../bin

LIBS += -lradiopresetstorage
LIBS += -lradioenginewrapper

symbian:LIBS += -lxqservice
symbian:LIBS += -lxqserviceutil
symbian:LIBS += -lxqsettingsmanager

DEPENDPATH += $$INCLUDEPATH src

# $$_PRO_FILE_PWD_ points to the directory of the pro file
MOC_DIR = $$_PRO_FILE_PWD_/tmp

# Input
HEADERS += radiouiengineexport.h
HEADERS += radiostation.h
HEADERS += radiostationmodel.h
HEADERS += radiostationfiltermodel.h
HEADERS += radiouiengine.h
HEADERS += radiohistoryitem.h
HEADERS += radiohistorymodel.h
HEADERS += radioscannerengine.h
HEADERS += radioservicedef.h
HEADERS += radionotificationdata.h

symbian:HEADERS += radiocontrolservice.h
symbian:HEADERS += radiomonitorservice.h
win32:HEADERS += radiomonitorservice_win32.h

INTERNAL_HEADERS += radiostation_p.h
INTERNAL_HEADERS += radiohistoryitem_p.h
INTERNAL_HEADERS += radiohistorymodel_p.h
INTERNAL_HEADERS += radiouiengine_p.h
INTERNAL_HEADERS += radiostationmodel_p.h
INTERNAL_HEADERS += radioscannerengine_p.h

win32:HEADERS += $$INTERNAL_HEADERS

SOURCES += radiostation.cpp
SOURCES += radiostation_p.cpp
SOURCES += radiostationmodel.cpp
SOURCES += radiostationmodel_p.cpp
SOURCES += radiostationfiltermodel.cpp
SOURCES += radiouiengine.cpp
SOURCES += radiouiengine_p.cpp
SOURCES += radiohistoryitem.cpp
SOURCES += radiohistoryitem_p.cpp
SOURCES += radiohistorymodel.cpp
SOURCES += radiohistorymodel_p.cpp
SOURCES += radioscannerengine.cpp
SOURCES += radioscannerengine_p.cpp

symbian:SOURCES += radiocontrolservice.cpp
symbian:SOURCES += radiomonitorservice.cpp
win32:SOURCES += radiomonitorservice_win32.cpp
