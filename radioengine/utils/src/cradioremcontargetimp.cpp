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
#include <RemConCallHandlingTarget.h>
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>

// User includes
#include "cradioenginelogger.h"
#include "cradioremcontargetimp.h"
#include "mradiocontroleventobserver.h"
#include "radioengineutils.h"

// Constants
const TInt KVRVolumeTimerInitialDelay = 1000000; // Initial timer for headset volume up/down press event should expire immediately

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRemConTargetImp::CRadioRemConTargetImp()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::ConstructL()
    {
    RadioEngineUtils::InitializeL();
    // Create interface selector.
    iInterfaceSelector = CRemConInterfaceSelector::NewL();
    // Create a new CRemConCoreApiTarget, owned by the interface selector.
    iCoreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector, *this );
    // Create a new CRemConCallHandlingTarget, owned by the interface selector.
    iCallTarget = CRemConCallHandlingTarget::NewL( *iInterfaceSelector, *this );
    // Start being a target.
    iInterfaceSelector->OpenTargetL();
    // Create repeat timer.
    iRepeatTimer = CPeriodic::NewL( CActive::EPriorityStandard );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRemConTargetImp* CRadioRemConTargetImp::NewL()
    {
    CRadioRemConTargetImp* self = new ( ELeave ) CRadioRemConTargetImp();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRemConTargetImp::~CRadioRemConTargetImp()
    {
    if ( iRepeatTimer )
        {
        iRepeatTimer->Cancel();
        }
    delete iRepeatTimer;
    delete iInterfaceSelector; //deletes also iCallTarget and iCoreTarget
    iCoreTarget = NULL;
    iCallTarget = NULL;
    RadioEngineUtils::Release();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::SetControlEventObserver( MRadioControlEventObserver* aControlEventObserver )
    {
    iObserver = aControlEventObserver;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::MrccatoCommand( TRemConCoreApiOperationId aOperationId,
                                         TRemConCoreApiButtonAction aButtonAct )
    {
    LOG_FORMAT( "CRadioRemConTargetImp::MrccatoCommand ( aOperationId = %d, aButtonAct = %d )", aOperationId, aButtonAct );
    //TODO: Refactor
    if ( iObserver )
        {
        switch ( aOperationId )
            {
            case ERemConCoreApiChannelUp:
                {
                if ( aButtonAct == ERemConCoreApiButtonClick )
                    {
                    TRAP_IGNORE( iObserver->ChannelUpL( ETrue ); iObserver->ChannelUpL( EFalse ));
                    }
                else if ( aButtonAct == ERemConCoreApiButtonPress )
                    {
                    TRAP_IGNORE( iObserver->ChannelUpL( ETrue ));
                    }
                else if ( aButtonAct == ERemConCoreApiButtonRelease )
                    {
                    TRAP_IGNORE( iObserver->ChannelUpL( EFalse ));
                    }
                else {}
                break;
                }
            case ERemConCoreApiChannelDown:
                {
                if ( aButtonAct == ERemConCoreApiButtonClick )
                    {
                    TRAP_IGNORE( iObserver->ChannelDownL( ETrue ); iObserver->ChannelDownL( EFalse ));
                    }
                else if ( aButtonAct == ERemConCoreApiButtonPress )
                    {
                    TRAP_IGNORE( iObserver->ChannelDownL( ETrue ));
                    }
                else if ( aButtonAct == ERemConCoreApiButtonRelease )
                    {
                    TRAP_IGNORE( iObserver->ChannelDownL( EFalse ));
                    }
                else {}
                break;
                }
            case ERemConCoreApiVolumeUp:
                {
                if ( aButtonAct == ERemConCoreApiButtonClick )
                    {
                    TRAP_IGNORE( iObserver->VolumeUpL( ETrue ); iObserver->VolumeUpL( EFalse ));
                    }
                else if ( aButtonAct == ERemConCoreApiButtonPress )
                    {
                    iRepeatTimer->Cancel();
                    iRepeatId = ERemConCoreApiVolumeUp;
                    iRepeatTimer->Start( KVRVolumeTimerInitialDelay, KVRVolumeTimerInitialDelay, TCallBack( RepeatTimerCallback, this ));
                    TRAP_IGNORE( iObserver->VolumeUpL( ETrue ); iObserver->VolumeUpL( EFalse ));
                    }
                else if ( aButtonAct == ERemConCoreApiButtonRelease )
                    {
                    iRepeatTimer->Cancel();
                    }
                else {}
                break;
                }
            case ERemConCoreApiVolumeDown:
                {
                if ( aButtonAct == ERemConCoreApiButtonClick )
                    {
                    TRAP_IGNORE( iObserver->VolumeDownL( ETrue ); iObserver->VolumeDownL( EFalse ));
                    }
                else if ( aButtonAct == ERemConCoreApiButtonPress )
                    {
                    iRepeatTimer->Cancel();
                    iRepeatId = ERemConCoreApiVolumeDown;
                    iRepeatTimer->Start( KVRVolumeTimerInitialDelay, KVRVolumeTimerInitialDelay, TCallBack( RepeatTimerCallback, this ));
                    TRAP_IGNORE( iObserver->VolumeDownL( ETrue ); iObserver->VolumeDownL( EFalse ));
                    }
                else if ( aButtonAct == ERemConCoreApiButtonRelease )
                    {
                    iRepeatTimer->Cancel();
                    }
                else {}
                break;
                }
            case ERemConCoreApiStop:
                {
                if ( aButtonAct == ERemConCoreApiButtonClick )
                    {
                    TRAP_IGNORE( iObserver->StopL( ETrue ); iObserver->StopL( EFalse ));
                    }
                break;
                }
            case ERemConCoreApiBackward:
                {
                if( aButtonAct == ERemConCoreApiButtonClick )
                    {
                    TRAP_IGNORE( iObserver->BackwardL( ETrue ); iObserver->BackwardL( EFalse ));
                    }
                break;
                }
            case ERemConCoreApiRewind:
                {
                if ( aButtonAct == ERemConCoreApiButtonPress )
                    {
                    LOG( "ERemConCoreApiRewind" );
                    TRAP_IGNORE( iObserver->RewindL( ETrue ));
                    }
                else if ( aButtonAct == ERemConCoreApiButtonRelease )
                    {
                    TRAP_IGNORE( iObserver->RewindL( EFalse ));
                    }
                else {}
                break;
                }
            case ERemConCoreApiForward:
                {
                if( aButtonAct == ERemConCoreApiButtonClick )
                    {
                    TRAP_IGNORE( iObserver->ForwardL( ETrue ); iObserver->ForwardL( EFalse ));
                    }
                break;
                }
            case ERemConCoreApiFastForward:
                {
                if ( aButtonAct == ERemConCoreApiButtonPress )
                    {
                    LOG( "ERemConCoreApiFastForward" );
                    TRAP_IGNORE( iObserver->FastForwardL( ETrue ));
                    }
                else if ( aButtonAct == ERemConCoreApiButtonRelease )
                    {
                    TRAP_IGNORE( iObserver->FastForwardL( EFalse ));
                    }
                else {}
                break;
                }
            case ERemConCoreApiPlay:
            case ERemConCoreApiPause:
            case ERemConCoreApiPausePlayFunction:
                {
                if ( aButtonAct == ERemConCoreApiButtonClick )
                    {
                    TRAP_IGNORE( iObserver->PausePlayL( ETrue ); iObserver->PausePlayL( EFalse ));
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::MrccatoPlay( TRemConCoreApiPlaybackSpeed /*aSpeed*/,
                                      TRemConCoreApiButtonAction aButtonAct )
    {
    LOG( "CRadioRemConTargetImp::MrccatoPlay" );
    if ( iObserver )
        {
        if ( aButtonAct == ERemConCoreApiButtonClick )
            {
            TRAP_IGNORE( iObserver->PlayL( ETrue ); iObserver->PlayL( EFalse ))
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::MrccatoTuneFunction( TBool /*aTwoPart*/,
                                              TUint /*aMajorChannel*/,
                                              TUint /*aMinorChannel*/,
                                              TRemConCoreApiButtonAction /*aButtonAct*/)
    {
    LOG( "CRadioRemConTargetImp::MrccatoTuneFunction ( currently unsupported in Visual Radio )" );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::MrccatoSelectDiskFunction( TUint /*aDisk*/,
                                                    TRemConCoreApiButtonAction /*aButtonAct*/)
    {
    LOG( "CRadioRemConTargetImp::MrccatoSelectDiskFunction ( currently unsupported in Visual Radio )" );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::MrccatoSelectAvInputFunction( TUint8 /*aAvInputSignalNumber*/,
                                                       TRemConCoreApiButtonAction /*aButtonAct*/)
    {
    LOG( "CRadioRemConTargetImp::MrccatoSelectAvInputFunction ( currently unsupported in Visual Radio )" );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::MrccatoSelectAudioInputFunction( TUint8 /*aAudioInputSignalNumber*/,
                                                          TRemConCoreApiButtonAction /*aButtonAct*/)
    {
    LOG( "CRadioRemConTargetImp::MrccatoSelectAudioInputFunction ( currently unsupported in Visual Radio )" );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CRadioRemConTargetImp::RepeatTimerCallback( TAny* aPtr )
    {
    LOG( "CRadioRemConTargetImp::RepeatTimerCallback" );

    CRadioRemConTargetImp* self = reinterpret_cast<CRadioRemConTargetImp*>( aPtr );

    if ( self )
        {
        if ( self->iObserver )
            {
            switch ( self->iRepeatId )
                {
                case ERemConCoreApiVolumeUp:
                    {
                    TRAP_IGNORE( self->iObserver->VolumeUpL( ETrue ); self->iObserver->VolumeUpL( EFalse ))
                    break;
                    }
                case ERemConCoreApiVolumeDown:
                    {
                    TRAP_IGNORE( self->iObserver->VolumeDownL( ETrue ); self->iObserver->VolumeDownL( EFalse ))
                    break;
                    }
                default:
                    {
                    break;
                    }
                }
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::AnswerCall()
    {
    LOG( "CRadioRemConTargetImp::AnswerCall() ( currently unsupported in Visual Radio )" );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::AnswerEndCall()
    {
    LOG( "CRadioRemConTargetImp::AnswerEndCall() " );
    if ( iObserver )
        {
        TRAP_IGNORE( iObserver->AnswerEndCallL())
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::DialCall( const TDesC8& /*aTelNumber*/ )
    {
    LOG( "CRadioRemConTargetImp::DialCall ( currently unsupported in Visual Radio )" );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::EndCall()
    {
    LOG( "CRadioRemConTargetImp::EndCall() ( currently unsupported in Visual Radio )" );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::GenerateDTMF( const TChar /*aChar*/ )
    {
    LOG( "CRadioRemConTargetImp::GenerateDTMF ( currently unsupported in Visual Radio )" );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::LastNumberRedial()
    {
    LOG( "CRadioRemConTargetImp::LastNumberRedial ( currently unsupported in Visual Radio )" );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::MultipartyCalling( const TDesC8& /*aData*/ )
    {
    LOG( "CRadioRemConTargetImp::MultipartyCalling ( currently unsupported in Visual Radio )" );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::SpeedDial( const TInt /*aIndex*/ )
    {
    LOG( "CRadioRemConTargetImp::SpeedDial ( currently unsupported in Visual Radio )" );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRemConTargetImp::VoiceDial( const TBool /*aActivate*/ )
    {
    LOG( "CRadioRemConTargetImp::VoiceDial ( currently unsupported in Visual Radio )" );
    }
