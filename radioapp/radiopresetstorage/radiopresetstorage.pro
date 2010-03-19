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
TARGET      = radiopresetstorage
CONFIG      += dll
DEFINES     += BUILD_PRESET_STORAGE_DLL
win32:DESTDIR = ../bin

# Wrapper does not depend on QtGui or Orbit
QT          = core
CONFIG      -= hb

symbian: {
    DEFINES += SYMBIAN
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
}

INCLUDEPATH += inc

COMPILE_WITH_NEW_PRESET_UTILITY {
    INCLUDEPATH += /epoc32/include/newfmpresetutility
    symbian:LIBS += -lpresetutility
} else {
    symbian:LIBS += -lfmpresetutility
}

DEPENDPATH += $$INCLUDEPATH src

# $$_PRO_FILE_PWD_ points to the directory of the pro file
MOC_DIR = $$_PRO_FILE_PWD_/tmp

# Input
HEADERS += radiopresetstorage.h
HEADERS += radiostationif.h
HEADERS += radiopresetstorageexport.h

symbian:INTERNAL_HEADERS += radiopresetstorage_p.h
symbian:SOURCES += radiopresetstorage.cpp

win32:HEADERS += radiopresetstorage_win32_p.h
win32:SOURCES += radiopresetstorage_win32.cpp
