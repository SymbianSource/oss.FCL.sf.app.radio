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

// System includes
#include <AudioOutput.h>

// User includes
#include "cradioroutableaudio.h"
#include "cradioaudiorouter.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CRadioRoutableAudio::CRadioRoutableAudio( CRadioAudioRouter* aAudioRouter )
    : iAudioRouter( aAudioRouter )
    {
    iAudioRouter->RegisterRoutableAudio( this );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CRadioRoutableAudio::~CRadioRoutableAudio()
    {
    iAudioRouter->UnRegisterRoutableAudio( this );
    delete iAudioOutput;
    delete iAudioRouter;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioRoutableAudio::SetAudioOutput( CAudioOutput* aAudioOutput )
    {
    delete iAudioOutput;
    iAudioOutput = aAudioOutput;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioRoutableAudio::DeleteAudioOutput()
    {
    delete iAudioOutput;
    iAudioOutput = NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioRoutableAudio::SetAudioRouteL( RadioEngine::TRadioAudioRoute aRoute )
    {
    if ( iAudioOutput )
        {
        iAudioOutput->SetAudioOutputL( aRoute == RadioEngine::ERadioSpeaker ? CAudioOutput::EPublic
                                                                            : CAudioOutput::EPrivate );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CRadioRoutableAudio::AudioRoutingSupported() const
    {
#ifdef __WINS__
    return ETrue;
#else
    return iAudioOutput ? ETrue : EFalse;
#endif
    }
