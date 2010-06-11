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
TARGET      = fmradiouiengine
CONFIG      += dll
DEFINES     += BUILD_UI_ENGINE_DLL

symbian:TARGET.UID3 = 0x2002EADA

# Wrapper does not depend on QtGui or Orbit
QT          = core gui network sql
CONFIG      -= hb

INCLUDEPATH += inc
INCLUDEPATH += ../radiopresetstorage/inc
INCLUDEPATH += ../radioenginewrapper/inc
INCLUDEPATH += ../../common

LIBS += -lfmradiopresetstorage
LIBS += -lfmradioenginewrapper

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
HEADERS += radiouiengine.h
HEADERS += radiohistoryitem.h
HEADERS += radiohistorymodel.h
HEADERS += radioscannerengine.h
HEADERS += radioservicedef.h
HEADERS += radionotificationdata.h
HEADERS += radiogenrelocalizer.h
HEADERS += radiocontrolservice.h
HEADERS += radiomonitorservice.h

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
SOURCES += radiouiengine.cpp
SOURCES += radiouiengine_p.cpp
SOURCES += radiohistoryitem.cpp
SOURCES += radiohistoryitem_p.cpp
SOURCES += radiohistorymodel.cpp
SOURCES += radiohistorymodel_p.cpp
SOURCES += radioscannerengine.cpp
SOURCES += radioscannerengine_p.cpp
SOURCES += radiocontrolservice.cpp
SOURCES += radiomonitorservice.cpp
