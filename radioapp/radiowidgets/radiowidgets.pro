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
CONFIG      += dll hb
DEFINES     += BUILD_WIDGETS_DLL

INCLUDEPATH += inc
INCLUDEPATH += ../radiouiengine/inc
INCLUDEPATH += ../commoninc

LIBS        += -lradiouiengine

LOGGING_ENABLED:LIBS += -lradioenginewrapper

DEPENDPATH  += $$INCLUDEPATH src

# Input
HEADERS     += radiowidgetsexport.h
HEADERS     += radiowindow.h
HEADERS     += radiouiutilities.h
HEADERS     += radioviewbase.h
HEADERS     += radiouiloader.h
HEADERS     += radiomainview.h
HEADERS     += radiostripbase.h
HEADERS     += radiofrequencystrip.h
HEADERS     += radiofrequencyitem.h
HEADERS     += radiostationsview.h
HEADERS     += radiobannerlabel.h
HEADERS     += radiofrequencyscanner.h
HEADERS     += radiofadinglabel.h
HEADERS     += radiostationcarousel.h
HEADERS     += radiostationitem.h
HEADERS     += radiohistoryview.h

SOURCES     += radiowindow.cpp
SOURCES     += radiouiutilities.cpp
SOURCES     += radioviewbase.cpp
SOURCES     += radiouiloader.cpp
SOURCES     += radiomainview.cpp
SOURCES     += radiostripbase.cpp
SOURCES     += radiofrequencystrip.cpp
SOURCES     += radiofrequencyitem.cpp
SOURCES     += radiostationsview.cpp
SOURCES     += radiobannerlabel.cpp
SOURCES     += radiofrequencyscanner.cpp
SOURCES     += radiofadinglabel.cpp
SOURCES     += radiostationcarousel.cpp
SOURCES     += radiostationitem.cpp
SOURCES     += radiohistoryview.cpp

RESOURCES   += res/fmradioui.qrc
