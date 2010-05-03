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

TEMPLATE    = lib
TARGET      = radiowidgetsplugin

win32: {
    DESTDIR = ../bin
    LIBS    += -L../bin
}

CONFIG      += hb plugin
QT          += core

INCLUDEPATH += ../radiowidgets/inc
INCLUDEPATH += ../commoninc

symbian: {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY       = CAP_GENERAL_DLL
    pluginstub.sources      = radiowidgetsplugin.dll
    pluginstub.path         = /resource/plugins
    DEPLOYMENT              += pluginstub
}

SOURCES     += main.cpp

LIBS        += -lradiowidgets
