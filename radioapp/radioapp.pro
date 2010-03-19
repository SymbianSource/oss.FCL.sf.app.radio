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

TEMPLATE = subdirs

# Process directories in order that they are given
CONFIG += ordered

SUBDIRS += radioenginewrapper
SUBDIRS += radiopresetstorage
SUBDIRS += radiouiengine
SUBDIRS += radiowidgets
SUBDIRS += radioapplication

symbian {
    BLD_INF_RULES.prj_mmpfiles += $$LITERAL_HASH"include \"../rom/bld.inf\""
}
