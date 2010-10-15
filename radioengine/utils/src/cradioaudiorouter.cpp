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
#include "cradioenginelogger.h"
#include "cradioaudiorouter.h"
#include "mradioaudioroutingobserver.h"
#include "cradioroutableaudio.h"
#include "radioengineutils.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioAudioRouter::CRadioAudioRouter( MRadioAudioRoutingObserver& aAudioRoutingObserver )
    : iAudioRoutingObserver( aAudioRoutingObserver )
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CRadioAudioRouter* CRadioAudioRouter::NewL( MRadioAudioRoutingObserver& aAudioRoutingObserver )
    {
    LEVEL3( LOG_METHOD_AUTO );
    CRadioAudioRouter* self = new( ELeave ) CRadioAudioRouter( aAudioRoutingObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CRadioAudioRouter::~CRadioAudioRouter()
    {
    LEVEL3( LOG_METHOD_AUTO );
    RadioEngineUtils::Release();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioAudioRouter::ConstructL()
    {
    RadioEngineUtils::InitializeL();
    LEVEL3( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
// Sets audio route
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioAudioRouter::SetAudioRouteL( RadioEngine::TRadioAudioRoute aAudioRoute )
    {
    LEVEL3( LOG_METHOD_AUTO );
    if ( NULL == iRoutableAudio )
        {
        User::Leave( KErrNotFound );
        }
    iRoutableAudio->SetAudioRouteL( aAudioRoute );
    iAudioRoutingObserver.AudioRouteChangedL( aAudioRoute );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioAudioRouter::RegisterRoutableAudio( CRadioRoutableAudio* aRoutableAudio )
    {
    LEVEL3( LOG_METHOD_AUTO );
    if (iRoutableAudio)
        {
        LOG_FORMAT( "Reregistration, only one instance expected! Ptr1 = %p, ptr2 = %p", iRoutableAudio, aRoutableAudio );
        __ASSERT_DEBUG( iRoutableAudio, User::Panic( _L("FMRadio" ), KErrAbort ) );
        }
    iRoutableAudio = aRoutableAudio;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioAudioRouter::UnRegisterRoutableAudio( CRadioRoutableAudio* DEBUGVAR( aRoutableAudio ) )
    {
    LEVEL3( LOG_METHOD_AUTO );
    __ASSERT_DEBUG( aRoutableAudio == iRoutableAudio, User::Panic( _L("FMRadio" ), KErrAbort ) );

    iRoutableAudio = NULL;
    }

