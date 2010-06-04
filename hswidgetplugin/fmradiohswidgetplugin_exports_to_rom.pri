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

BLD_INF_RULES.prj_exports += \
"$${LITERAL_HASH}include <platform_paths.hrh>"

#iby exports to core
BLD_INF_RULES.prj_exports += \
"rom/fmradiohswidgetplugin.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(fmradiohswidgetplugin.iby)"

#iby exports to install
BLD_INF_RULES.prj_exports += \
"rom/fmradiohswidgetplugininstaller.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(fmradiohswidgetplugininstaller.iby)"

#iby exports to language
#BLD_INF_RULES.prj_exports += \
#"rom/homescreenapp_resources.iby  LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(homescreenapp_resources.iby)"

BLD_INF_RULES.prj_extensions += \
$$LITERAL_HASH"ifdef MARM" \
  " START EXTENSION app-services/buildstubsis" \
    " OPTION SRCDIR rom" \
    " OPTION SISNAME fmradiohswidgetplugin_stub" \
  " END" \
$$LITERAL_HASH"endif"
