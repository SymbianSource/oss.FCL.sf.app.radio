/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef MRADIOENGINEINITIALIZER_H_
#define MRADIOENGINEINITIALIZER_H_

#include <e32std.h>

// Forward declarations
class CRadioAudioRouter;
class CRadioSystemEventCollector;
class CRadioSettings;
class CRadioEngine;
class CRadioPubSub;

NONSHARABLE_CLASS( MRadioEngineInitializer )
    {
public:

    virtual CRadioAudioRouter* InitAudioRouterL() = 0;
    virtual CRadioSystemEventCollector* InitSystemEventCollectorL() = 0;
    virtual CRadioSettings* InitSettingsL() = 0;
    virtual CRadioPubSub* InitPubSubL() = 0;

    };

#endif // MRADIOENGINEINITIALIZER_H_
