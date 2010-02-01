/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for the control event observer.
*
*/

#include <aknViewAppUi.h>
#include "fmradiocontroleventobserverimpl.h"

#include "fmradio.hrh"
// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CFMRadioControlEventObserverImpl::CFMRadioControlEventObserverImpl( CFMRadioAppUi& aAppUi )
        : iAppUi( aAppUi )
    {
    
    }


// ---------------------------------------------------------------------------
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFMRadioControlEventObserverImpl* CFMRadioControlEventObserverImpl::NewL( 
        CFMRadioAppUi& aAppUi )
    {
    CFMRadioControlEventObserverImpl* self = 
        new( ELeave ) CFMRadioControlEventObserverImpl( aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CFMRadioControlEventObserverImpl::~CFMRadioControlEventObserverImpl()
    {  
    }

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::StepToChannelL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::StepToChannelL( TFMRadioTuneDirection aDirection )
	{
	CAknViewAppUi* appUi = static_cast<CAknViewAppUi*>( CCoeEnv::Static()->AppUi() );
	if( aDirection == EFMRadioDown )
	    {
	    appUi->HandleCommandL( EFMRadioCmdPrevChannel );
	    }
	else if( aDirection == EFMRadioUp )
	    {
	    appUi->HandleCommandL( EFMRadioCmdNextChannel );
	    }
	else
	    {
	    }
	}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::SetChannelL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::SetChannelL( TInt /*aChannelId*/ )
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::SeekL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::SeekL( TFMRadioTuneDirection /*aDirection*/ )
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::StepToFrequencyL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::StepToFrequencyL( TFMRadioTuneDirection /*aDirection*/ )
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::SetFrequencyL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::SetFrequencyL( TUint32 /*aFreq*/ )
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::AdjustVolumeL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::AdjustVolumeL( TFMRadioVolumeSetDirection /*aDirection*/ )
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::MuteL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::MuteL( TBool /*aMute*/ )
	{
	CAknViewAppUi* appUi = static_cast<CAknViewAppUi*>( CCoeEnv::Static()->AppUi() );
	TRAP_IGNORE( appUi->HandleCommandL( EFMRadioCmdMute ) );
	}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::PlayL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::PlayL( TBool /*aDownPressed*/ )
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::PauseL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::PauseL(TBool /*aDownPressed*/ )
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::PausePlayL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::PausePlayL( TBool /*aDownPressed*/ )
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::StopL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::StopL( TBool /*aDownPressed*/ )
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::ForwardL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::ForwardL( TBool /*aDownPressed*/ )
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::FastForwardL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::FastForwardL( TBool /*aDownPressed*/ )
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::BackwardL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::BackwardL( TBool /*aDownPressed*/ )
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::RewindL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::RewindL( TBool /*aDownPressed*/ )
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::ChannelUpL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::ChannelUpL( TBool /*aDownPressed*/ )
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::ChannelDownL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::ChannelDownL( TBool /*aDownPressed*/ )
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::VolumeUpL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::VolumeUpL( TBool /*aDownPressed*/ )
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::VolumeDownL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::VolumeDownL( TBool /*aDownPressed*/ )
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::ChangeViewL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::ChangeViewL(TUid /*aViewId*/, TBool /*aForceChange*/)
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::SetAudioOutputDestinationL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::SetAudioOutputDestinationL(TFMRadioOutputDestination /*aDestination*/)
{
}

// ---------------------------------------------------------------------------
// CFMRadioControlEventObserverImpl::AnswerEndCallL
// ---------------------------------------------------------------------------
//
void CFMRadioControlEventObserverImpl::AnswerEndCallL()    
{
}
