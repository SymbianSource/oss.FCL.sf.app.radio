/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*  Class which implements handling Side Volume Key events (SVK).
*  This class implements methods of MRemConCoreApiTargetObserver
*
*
*/



// INCLUDE FILES
#include <remconcoreapitarget.h>
#include <remconcoreapitargetobserver.h>
#include <RemConCallHandlingTarget.h>
#include <RemConCallHandlingTargetObserver.h>
#include <remconinterfaceselector.h>
#include <aknconsts.h>                      // KAknStandardKeyboardRepeatRate
#include <aknViewAppUi.h>

#include "fmradiosvkevents.h"


// CONSTANTS
const TInt KVolumeControlExpiryPeriod = 2000000;
const TInt KVolumeFirstChangePeriod = KAknStandardKeyboardRepeatRate;
const TInt KVolumeChangePeriod = KAknStandardKeyboardRepeatRate;
const TInt KVolumeChangeUp = 1;
const TInt KVolumeChangeDown = -1;


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CFMRadioSvkEvents
// ----------------------------------------------------------------------------
//
CFMRadioSvkEvents* CFMRadioSvkEvents::NewL( MFMRadioSvkEventsObserver& aObserver )
    {
#ifdef _DEBUG
	RDebug::Print(_L("CFMRadioSvkEvents::NewL"));
#endif
    CFMRadioSvkEvents* self = new ( ELeave ) CFMRadioSvkEvents( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CFMRadioSvkEvents
// ----------------------------------------------------------------------------
//
CFMRadioSvkEvents::CFMRadioSvkEvents( MFMRadioSvkEventsObserver& aObserver ) :
    iObserver( aObserver )
    {
    }


// ----------------------------------------------------------------------------
// CFMRadioSvkEvents::~CFMRadioSvkEvents
// ----------------------------------------------------------------------------
//
CFMRadioSvkEvents::~CFMRadioSvkEvents()
    {
#ifdef _DEBUG
	RDebug::Print(_L("CFMRadioSvkEvents::~CFMRadioSvkEvents"));
#endif
    delete iRemoveTimer;
    delete iVolumeTimer;
    delete iInterfaceSelector;
    iCoreTarget = NULL; // For LINT. Owned by iInterfaceSelector
    iCallTarget = NULL; // For LINT. Owned by iInterfaceSelector
    }

// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//
void CFMRadioSvkEvents::ConstructL( )
    {
#ifdef _DEBUG
	RDebug::Print(_L("CFMRadioSvkEvents::ConstructL"));
#endif

    iTargetOpen = EFalse;
    iInterfaceSelector = CRemConInterfaceSelector::NewL();

    // owned by iInterfaceSelector
    iCoreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector, *this );
    iCallTarget = CRemConCallHandlingTarget::NewL(*iInterfaceSelector, *this);

    if (!iTargetOpen)
    {
    	iInterfaceSelector->OpenTargetL();
    	iTargetOpen = ETrue ;
    }

    // Remote control server command repeat timer.
    iVolumeTimer = CPeriodic::NewL( EPriorityNormal );
    iRemoveTimer = CPeriodic::NewL( EPriorityNormal );
    }

// ----------------------------------------------------------------------------
// CFMRadioSvkEvents::MrccatoCommand
// Handles side volume key events.
// ----------------------------------------------------------------------------
//
void CFMRadioSvkEvents::MrccatoCommand( TRemConCoreApiOperationId   aOperationId,
                                    TRemConCoreApiButtonAction  aButtonAct )
    {
#ifdef _DEBUG
	RDebug::Print(_L("CFMRadioSvkEvents::MrccatoCommand, Operation ID [%d]"), aOperationId);
#endif
    switch ( aOperationId )
        {
        case ERemConCoreApiVolumeUp: //  Volume Up
        	{
            HandleVolumeButtons( aButtonAct, KVolumeChangeUp ); 
            break;
        	}
        case ERemConCoreApiVolumeDown: //  Volume Down
        	{
            HandleVolumeButtons( aButtonAct, KVolumeChangeDown ); 
            break;
        	}
        case ERemConCoreApiBackward: //  Backward
            {
			iObserver.FMRadioHeadsetEvent( ERewind );
            break;
            }
		case ERemConCoreApiForward: //   Forward
            {
			iObserver.FMRadioHeadsetEvent( EForward );
            break;
            }
        case ERemConCoreApiRewind: //  Tune down
            {
            if ( aButtonAct == ERemConCoreApiButtonPress )
            	{
            	TuneChannel( EFMRadioCmdSeekDown );
            	}
            break;
            }
		case ERemConCoreApiFastForward: //   Tune up
            {
            if ( aButtonAct == ERemConCoreApiButtonPress )
             	{
             	TuneChannel( EFMRadioCmdSeekUp );
             	}
            break;
            }
        case ERemConCoreApiPausePlayFunction:        
        	{
        	iObserver.FMRadioHeadsetEvent( EPausePlay );
        	break;
        	}
        case ERemConCoreApiStop:
        	{
        	iObserver.FMRadioHeadsetEvent( EStop );
        	break;	
        	}            
        default :
            {
            // Don’t Panic or do anything here. Other events such as Stop, Rewind, Forward will fall here.
            break;
            }
        }
    }


// ----------------------------------------------------------------------------
// StartRemoveTimerL
//
// ----------------------------------------------------------------------------
//
void CFMRadioSvkEvents::StartRemoveTimerL( )
    {
#ifdef _DEBUG
	RDebug::Print(_L("CFMRadioSvkEvents::StartRemoveTimerL" ));
#endif
    iRemoveTimer->Cancel();
    iRemoveTimer->Start(    KVolumeControlExpiryPeriod,
                            KVolumeControlExpiryPeriod,
                            TCallBack( RemoveControl, this ) );
    }

// ----------------------------------------------------------------------------
// CancelRemoveTimer
// ----------------------------------------------------------------------------
//
void CFMRadioSvkEvents::CancelRemoveTimer( )
    {
    iRemoveTimer->Cancel();
    }

// ----------------------------------------------------------------------------
// DoChangeVolume
//
// ----------------------------------------------------------------------------
//
void CFMRadioSvkEvents::DoChangeVolume( )
    {
#ifdef _DEBUG
	RDebug::Print(_L("CFMRadioSvkEvents::DoChangeVolume" ));
#endif
    TRAP_IGNORE( iObserver.FMRadioSvkChangeVolumeL( iChange ) );
    iRemoveTimer->Cancel();
    iRemoveTimer->Start(   KVolumeControlExpiryPeriod,
                            KVolumeControlExpiryPeriod,
                            TCallBack( RemoveControl, this ) );
    }


// ----------------------------------------------------------------------------
// TuneChannel
//
// ----------------------------------------------------------------------------
//
void CFMRadioSvkEvents::TuneChannel( TFMRadioCommandIds aDirection )
    {
	CAknViewAppUi* appUi = static_cast<CAknViewAppUi*>( CCoeEnv::Static()->AppUi() ); 
    TRAP_IGNORE( appUi->HandleCommandL( aDirection ) );
    }

// ----------------------------------------------------------------------------
// RemoveControlL
//
// ----------------------------------------------------------------------------
//
void CFMRadioSvkEvents::RemoveControlL( )
    {
    iRemoveTimer->Cancel();
    iObserver.FMRadioSvkRemoveVolumeL( );
    }



// ----------------------------------------------------------------------------
// ChangeVolume
//
// ----------------------------------------------------------------------------
//
TInt CFMRadioSvkEvents::ChangeVolume( TAny* aObject )
    {
    // cast, and call non-static function
    static_cast<CFMRadioSvkEvents*>( aObject )->DoChangeVolume( );
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// RemoveControl
//
// ----------------------------------------------------------------------------
//
TInt CFMRadioSvkEvents::RemoveControl( TAny* aObject )
    {
    // cast, and call non-static function
    TRAP_IGNORE( static_cast<CFMRadioSvkEvents*>( aObject )->RemoveControlL() );
    return KErrNone;
    }


// ---------------------------------------------------------
// CFMRadioSvkEvents::AnswerCall
// ---------------------------------------------------------
//
void CFMRadioSvkEvents::AnswerCall()
    {
    }

// ---------------------------------------------------------
// CFMRadioSvkEvents::AnswerEndCall
// ---------------------------------------------------------
//
void CFMRadioSvkEvents::AnswerEndCall()
    {
	iObserver.FMRadioHeadsetEvent(EForward);
    }

// ---------------------------------------------------------
// CFMRadioSvkEvents::DialCall
// ---------------------------------------------------------
//
void CFMRadioSvkEvents::DialCall( const TDesC8& /*aTelNumber*/ )
    {
    }

// ---------------------------------------------------------
// CFMRadioSvkEvents::EndCall
// ---------------------------------------------------------
//
void CFMRadioSvkEvents::EndCall()
    {
    }

// ---------------------------------------------------------
// CFMRadioSvkEvents::GenerateDTMF
// ---------------------------------------------------------
//
void CFMRadioSvkEvents::GenerateDTMF( const TChar /*aChar*/ )
    {
    }

// ---------------------------------------------------------
// CFMRadioSvkEvents::LastNumberRedial
// ---------------------------------------------------------
//
void CFMRadioSvkEvents::LastNumberRedial()
    {
    }

// ---------------------------------------------------------
// CFMRadioSvkEvents::MultipartyCalling
// ---------------------------------------------------------
//
void CFMRadioSvkEvents::MultipartyCalling( const TDesC8& /*aData*/ )
    {
    }

// ---------------------------------------------------------
// CFMRadioSvkEvents::SpeedDial
// ---------------------------------------------------------
//
void CFMRadioSvkEvents::SpeedDial( const TInt /*aIndex*/ )
    {
    }

// ---------------------------------------------------------
// CFMRadioSvkEvents::VoiceDial
// ---------------------------------------------------------
//
void CFMRadioSvkEvents::VoiceDial( const TBool /*aActivate*/ )
    {
    }
// ---------------------------------------------------------
// CFMRadioSvkEvents::HandleVolumeButtons
// ---------------------------------------------------------
//
void CFMRadioSvkEvents::HandleVolumeButtons( TRemConCoreApiButtonAction aButtonAct, TInt aDirection  )
	{
	switch( aButtonAct )
        {
        case ERemConCoreApiButtonPress:
            {
            // Volume up hold down for 0,6 seconds
            iVolumeTimer->Cancel();
            iVolumeTimer->Start( KVolumeFirstChangePeriod,
                                 KVolumeChangePeriod,
                                 TCallBack( ChangeVolume, this ) );
       	    iChange = aDirection;
       		DoChangeVolume( );
            break;
            }
        case ERemConCoreApiButtonRelease:
            {
            iVolumeTimer->Cancel();
            break;
            }
        case ERemConCoreApiButtonClick:
            {
            iChange = aDirection;
            DoChangeVolume( );
            break;
            }
        default:
            {
            // Never hits this
            break;
            }
        }
	}
// End of File
