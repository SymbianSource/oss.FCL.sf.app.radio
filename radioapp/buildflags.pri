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


# ##########################################################
# Start of Flag definitions
# Add or remove comments to disable or enable the features
# ##########################################################

# Use of some debugging controls that are not part of official UI spec
# RADIOFLAGS += USE_DEBUGGING_CONTROLS

# Reads the layout docml files and images from e:/radiotest/ folder
# RADIOFLAGS += USE_LAYOUT_FROM_E_DRIVE

# Flag to use dummy radio data read from XML file
# RADIOFLAGS += USE_DUMMY_RADIO_DATA

# Functional testing
# RADIOFLAGS += SHOW_CALLSIGN_IN_ANY_REGION

# Build flag to enable usage of the new preset utility
RADIOFLAGS += COMPILE_WITH_NEW_PRESET_UTILITY

# Build flag to add EXPORTUNFROZEN to the pro file
# RADIOFLAGS += USE_UNFROZEN_EXPORTS

# Enables the assert macros
# RADIOFLAGS += ENABLE_ASSERTS

# ##########################################################
# Start of Logging flag definitions
# ##########################################################

# Full logging flag that enables the full logging including also timestamps
# Enabled by default in debug builds
# CONFIG(debug, debug|release) : LOGGING_FLAGS += LOGGING_ENABLED

# Uncomment to enable full logging in release builds
# LOGGING_FLAGS *= LOGGING_ENABLED

# Timestamp logging flag that enables only timestamp logging
# LOGGING_FLAGS += TIMESTAMP_LOGGING_ENABLED

# Marker that is printed in the beginning of each log line
## LOGGING_FLAGS += LOGMARKER=\\\"FMRadio:\\\"

# Combines Ui and Engine logs by feeding UI traces to the engine logger
LOGGING_FLAGS += COMBINE_WITH_ENGINE_LOGGER

contains(LOGGING_FLAGS, TIMESTAMP_LOGGING_ENABLED)|contains(LOGGING_FLAGS, LOGGING_ENABLED) {

    # Writes debug prints to file if enabled
    #LOGGING_FLAGS += TRACE_TO_FILE

    # Settings for the log file name
    symbian:contains(LOGGING_FLAGS, TRACE_TO_FILE) {
        LOGGING_FLAGS += \"TRACE_OUTPUT_FILE=\\\"c:/logs/radio/fmradioui.txt\\\"\"
        LOGGING_FLAGS += FILTER_BY_LOGMARKER=true
    }
}

# Method for testing signal-slot connections
# 1 - Log failed connection
# 2 - Log failed connection and halt debugger
LOGGING_FLAGS += CONNECT_TEST_MODE=2

DEFINES += $$LOGGING_FLAGS

# ##########################################################
# End of Flag definitions
# ##########################################################

# Copy all radio flags to defines to define them as preprocessor macros
DEFINES += $$RADIOFLAGS
CONFIG += $$RADIOFLAGS
CONFIG += $$LOGGING_FLAGS

symbian: {
    DEFINES += SYMBIAN
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.VID              = VID_DEFAULT
    TARGET.CAPABILITY       = CAP_GENERAL_DLL
}

win32: {
    DEFINES     += BUILD_WIN32
    DESTDIR     = ../bin
    LIBS        += -L../bin
    INCLUDEPATH += ../radioenginewrapper/inc
}

USE_UNFROZEN_EXPORTS {
    symbian:MMP_RULES   +=  "exportunfrozen"
    symbian:DEF_FILE    = not_used.def
}

# $$_PRO_FILE_PWD_ points to the directory of the pro file
MOC_DIR         = $$_PRO_FILE_PWD_/tmp
RCC_DIR         = $$_PRO_FILE_PWD_/tmp
OBJECTS_DIR     = $$_PRO_FILE_PWD_/tmp
UI_DIR          = $$_PRO_FILE_PWD_/tmp
UI_HEADERS_DIR  = $$_PRO_FILE_PWD_/tmp
UI_SOURCES_DIR  = $$_PRO_FILE_PWD_/tmp
