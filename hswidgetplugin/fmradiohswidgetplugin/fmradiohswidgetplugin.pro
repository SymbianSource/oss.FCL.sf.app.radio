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
# Description: Example of home screen widget
#

include(../buildflags.pri)

TEMPLATE = lib
CONFIG += plugin mobility hb
MOBILITY = serviceframework
# Traslations is waiting for the widget specific ts
#TRANSLATIONS += $${TARGET}.ts

LIBS += -lxqsettingsmanager \
		-lxqservice \
		-lxqserviceutil

HEADERS += ./inc/*.h
SOURCES += ./src/*.cpp
SOURCES += ../../common/radiologger.cpp

INCLUDEPATH += ./inc
INCLUDEPATH += ../../common

UID = 2002E6D6

RESOURCES = resources.qrc
#exportResources(./resource/$${TARGET}_icon*.png, $$PLUGIN_SUBDIR)

symbian: {

    DESTDIR = /private/20022F35/import/widgetregistry/$${UID}
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    TARGET.UID3 = 0x$${UID}
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.VID = VID_DEFAULT
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    
    plugins.path = $${DESTDIR}
    plugins.sources = $${TARGET}.dll 
    
    widgetResources.path = $${DESTDIR}
    widgetResources.sources += resource/$${TARGET}.xml
    widgetResources.sources += resource/$${TARGET}.manifest
    # icon is now removed to be able to unistall the widget more easily.
    #widgetResources.sources += resource/$${TARGET}_icon*.png
	
    localisedFiles.path = /resource/qt/translations
    localisedFiles.sources += ./locales/*.qm
        
    DEPLOYMENT += plugins \
                  widgetResources \
                  localisedFiles
}

win32: {

    CONFIG(debug, debug|release) {
      SUBDIRPART = debug
    } else {
      SUBDIRPART = release
    }    
    
    PLUGIN_SUBDIR = /hsresources/import/widgetregistry/$${UID}
    
    DESTDIR = $$PWD/../../../../../bin/$${SUBDIRPART}/$${PLUGIN_SUBDIR}

    manifest.path = $${DESTDIR}
    manifest.files = ./resource/*.manifest ./resource/*.xml ./resource/*.css #./resource/*.png
    
    widgetLocalisation.path = $$PWD/../../../../../bin/$${SUBDIRPART}/resource/qt/translations
    widgetLocalisation.files += ./locales/*.qm
    
    INSTALLS += manifest    
}

