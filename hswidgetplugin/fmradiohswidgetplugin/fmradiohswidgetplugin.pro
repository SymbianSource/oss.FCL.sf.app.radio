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

LIBS += -lxqsettingsmanager \
		-lxqservice \
		-lxqserviceutil

HEADERS += ./inc/*.h
SOURCES += ./src/*.cpp

INCLUDEPATH += ./inc
INCLUDEPATH += ../../radioapp/commoninc

UID = 2002E6D6

RESOURCES = resources.qrc
#exportResources(./resource/$${TARGET}_icon*.png, $$PLUGIN_SUBDIR)

symbian: {

    DESTDIR = /private/20022F35/import/widgetregistry/$${UID}
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

    TARGET.UID3 = 0x$${UID}
    TARGET.EPOCALLOWDLLDATA=1
    TARGET.CAPABILITY = ALL -TCB
    
    plugins.path = $${DESTDIR}
    plugins.sources = $${TARGET}.dll 
    
    widgetResources.path = $${DESTDIR}
# commented out
#    widgetResources.sources += resource/$${TARGET}.s60xml
# Use this in emulator instead of xml in resources directory.    
#    widgetResources.sources += $${TARGET}.xml
    widgetResources.sources += resource/$${TARGET}.xml
    widgetResources.sources += resource/$${TARGET}.manifest
# icon is now removed to be able to unistall the widget more easily.
#    widgetResources.sources += resource/$${TARGET}_icon*.png
        
    DEPLOYMENT += plugins \
                  widgetResources
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
    manifest.files = ./resource/*.manifest ./resource/*.xml #./resource/*.png
    
    INSTALLS += manifest    
    
}
