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

// User includes
#include "cradioengine.h"
#include "cradioengineimp.h"
#include "cradioenginelogger.h"
#include "mradioengineinitializer.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// Starts the radio engine initialization.
// ---------------------------------------------------------------------------
//
EXPORT_C CRadioEngine* CRadioEngine::NewL( MRadioEngineInitializer& aInitializer )
    {
    CRadioAudioRouter* audioRouter = aInitializer.InitAudioRouterL();
    CleanupStack::PushL( audioRouter );

    CRadioEngineImp* self = new (ELeave) CRadioEngineImp( audioRouter );
    CleanupStack::Pop( audioRouter );
    CleanupStack::PushL( self );

    self->SetSystemEventCollector( aInitializer.InitSystemEventCollectorL() );
    self->SetRadioSettings( aInitializer.InitSettingsL() );
    self->SetRadioPubSub( aInitializer.InitPubSubL() );
    self->ConstructL();

    self->InitRadioL( self->DetermineRegion(), self->PubSub() );
    self->EnableAudio( ETrue );

    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngine::CRadioEngine( CRadioAudioRouter* aAudioRouter )
    : CRadioRoutableAudio( aAudioRouter )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CRadioEngine::~CRadioEngine()
    {
    }