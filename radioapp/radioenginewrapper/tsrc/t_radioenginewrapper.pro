# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description: project file for radiouiengine's unit tests
# Description:
TEMPLATE = app
TARGET = t_radioenginewrapper
symbian: { 
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.SID = 0x101FF976  # Tried testUids(0x01000111 and 0x01001005) but failed
    MMP_RULES += SMPSAFE    
}
DEPENDPATH += . \
    inc \
    src
INCLUDEPATH += . \
    ../../../inc \
    ../inc \
    ../commoninc \
    ../../../../inc \
    /sf/app/radio/radioengine/utils/Stub/inc \
    /sf/app/radio/radioengine/utils/api \
    /sf/app/radio/radioengine/utils/inc \
    /sf/app/radio/radioengine/settings/api \
    /epoc32/include/internal \
    /sf/mw/mmmw/mmserv/radioutility/radio_utility/src \
    /sf/mw/mmmw/mmserv/radioutility/inc \
    /sf/mw/mmmw/mmserv/inc \
    /sf/mw/mmmw/inc \
    /sf/mw/mmmw/mmserv/radioutility/radioserver/inc
CONFIG += qtestlib
symbian:LIBS += -lradioenginewrapper_stub
symbian:LIBS += -lRadioSession_Stub
symbian:LIBS += -leuser
symbian:LIBS += -lcentralrepository
symbian:LIBS += -lflogger
HEADERS += inc/t_radioenginewrapper.h
HEADERS += inc\t_schedulerstartandstoptimer.h
SOURCES += src/t_radioenginewrapper.cpp
SOURCES += src/t_schedulerstartandstoptimer.cpp