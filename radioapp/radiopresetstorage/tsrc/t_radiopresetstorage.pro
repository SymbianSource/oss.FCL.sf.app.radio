#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

TEMPLATE = app
TARGET = t_radiopresetstorage

symbian: {
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.SID = 0x101FF976 # Old visual radio uid
    MMP_RULES += SMPSAFE
}

DEPENDPATH += . \
    inc \
    src
INCLUDEPATH += . \
    /epoc32/include/domain \
    /epoc32/include/domain/middleware \
    /epoc32/include/domain/applications \
    /epoc32/include/osextensions \
    /epoc32/include/middleware \
    /epoc32/include/osextensions/stdapis/stlport \
    stub/inc \
    ../../../inc \
    ../inc \
    ../../commoninc \
    ../../radioenginewrapper/inc \
    ../../radiopresetstorage/inc \
    ../../radiouiengine/inc \
    ../../../../inc

CONFIG += qtestlib

symbian:LIBS += -lradiouiengine
symbian:LIBS += -lradioenginewrapper
symbian:LIBS += -lradiopresetstorage


HEADERS += inc/t_radiopresetstorage.h
SOURCES += src/t_radiopresetstorage.cpp
