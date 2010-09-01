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
* Description: FMRadio active idle engine
*
*/


#include <e32std.h>

#include "fmradioactiveidleengine.h"
#include "fmradioactiveidleenginenotifyhandler.h"


/**
* Listing of the subscribed P&S values.
* Changing the order of this enumeration requires changes to CFMRadioActiveIdleEngine::ConstructL as well.
*/

enum TFMRadioActiveIdleStatusObserverArray
    {
    //EFMRadioActiveIdleRadioVolumeObserver,       /**< Index of the radio volume observer. */
    EFMRadioActiveIdleTuningStateObserver,       /**< Index of the tuning state observer. */
    EFMRadioActiveIdleFrequencyObserver,         /**< Index of the current frequency observer. */
    EFMRadioActiveIdleChannelChangeObserver,     /**< Index of the current channel observer. */
    EFMRadioActiveIdleChannelModifyObserver,     /**< Index of the channel modified observer. */
    EFMRadioActiveIdleMuteStateObserver,         /**< Index of the muting state observer. */
    EFMRadioActiveIdleApplicationObserver,       /**< Index of the FM Radio application observer used to observer whether the application is running or not. */
    EFMRadioActiveIdleHeadsetStatusObserver,     /**< Index of the antenna status observer. */
    EFMRadioActiveIdleDecimalCountObserver,      /**< Index of the frequency decimal count observer. */
    EFMRadioActiveIdlePowerStateObserver,        /**< Index of the power state observer. */
    EFMRadioActiveIdleRDSProgramServiceObserver,  /**< Index of the RDS Program Service observer. */
    EFMRadioPSDataRDSRadioText,
    EFMRadioPSDataChannelName,
    EFMRadioActiveIdlePresetListCountObserver,   /**< Index of the active preset list count observer */
    EFMRadioActiveIdlePresetListFocusObserver   /**< Index of the active preset list focus observer */
    };

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CFMRadioActiveIdleEngine::CFMRadioActiveIdleEngine( MFMRadioActiveIdleEngineNotifyHandler& aObserver )
    : iObserver( aObserver )
    {
    }

// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CFMRadioActiveIdleEngine::ConstructL()
    {
    //User::LeaveIfError( iPropertyObserverArray.Append( CFMRadioPropertyObserver::NewL( *this, KFMRadioEnginePSUid, KFMRadioPSDataVolume, CFMRadioPropertyObserver::EFMRadioPropertyInt ) ) );
    User::LeaveIfError( iPropertyObserverArray.Append( CFMRadioPropertyObserver::NewL( *this, KFMRadioPSUid, KFMRadioPSDataTuningState, CFMRadioPropertyObserver::EFMRadioPropertyInt ) ) );
    User::LeaveIfError( iPropertyObserverArray.Append( CFMRadioPropertyObserver::NewL( *this, KFMRadioPSUid, KFMRadioPSDataFrequency, CFMRadioPropertyObserver::EFMRadioPropertyInt ) ) );
    User::LeaveIfError( iPropertyObserverArray.Append( CFMRadioPropertyObserver::NewL( *this, KFMRadioPSUid, KFMRadioPSDataChannel, CFMRadioPropertyObserver::EFMRadioPropertyInt ) ) );
    User::LeaveIfError( iPropertyObserverArray.Append( CFMRadioPropertyObserver::NewL( *this, KFMRadioPSUid, KFMRadioPSDataChannelDataChanged, CFMRadioPropertyObserver::EFMRadioPropertyInt ) ) );
    User::LeaveIfError( iPropertyObserverArray.Append( CFMRadioPropertyObserver::NewL( *this, KFMRadioPSUid, KFMRadioPSDataRadioMuteState, CFMRadioPropertyObserver::EFMRadioPropertyInt ) ) );
    User::LeaveIfError( iPropertyObserverArray.Append( CFMRadioPropertyObserver::NewL( *this, KFMRadioPSUid, KFMRadioPSDataApplicationRunning, CFMRadioPropertyObserver::EFMRadioPropertyInt ) ) );
    User::LeaveIfError( iPropertyObserverArray.Append( CFMRadioPropertyObserver::NewL( *this, KFMRadioPSUid, KFMRadioPSDataHeadsetStatus, CFMRadioPropertyObserver::EFMRadioPropertyInt ) ) );
    User::LeaveIfError( iPropertyObserverArray.Append( CFMRadioPropertyObserver::NewL( *this, KFMRadioPSUid, KFMRadioPSDataFrequencyDecimalCount, CFMRadioPropertyObserver::EFMRadioPropertyInt ) ) );
    User::LeaveIfError( iPropertyObserverArray.Append( CFMRadioPropertyObserver::NewL( *this, KFMRadioPSUid, KFMRadioPSDataRadioPowerState, CFMRadioPropertyObserver::EFMRadioPropertyInt ) ) );
    User::LeaveIfError( iPropertyObserverArray.Append( CFMRadioPropertyObserver::NewL( *this, KFMRadioPSUid, KFMRadioPSDataRDSProgramService, CFMRadioPropertyObserver::EFMRadioPropertyText ) ) );
    User::LeaveIfError( iPropertyObserverArray.Append( CFMRadioPropertyObserver::NewL( *this, KFMRadioPSUid, KFMRadioPSDataRDSRadioText, CFMRadioPropertyObserver::EFMRadioPropertyText ) ) );
    User::LeaveIfError( iPropertyObserverArray.Append( CFMRadioPropertyObserver::NewL( *this, KFMRadioPSUid, KFMRadioPSDataChannelName, CFMRadioPropertyObserver::EFMRadioPropertyText ) ) );
    User::LeaveIfError( iPropertyObserverArray.Append( CFMRadioPropertyObserver::NewL( *this, KFMRadioPSUid, KFMRadioActiveIdlePresetListCount, CFMRadioPropertyObserver::EFMRadioPropertyInt ) ) );
    User::LeaveIfError( iPropertyObserverArray.Append( CFMRadioPropertyObserver::NewL( *this, KFMRadioPSUid, KFMRadioActiveIdlePresetListFocus, CFMRadioPropertyObserver::EFMRadioPropertyInt ) ) );
    }

// ---------------------------------------------------------------------------
// Makes all pubsub objects to subscribe.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFMRadioActiveIdleEngine::ActivateL()
	{
	for ( TInt i = 0; i < iPropertyObserverArray.Count(); i++ )
		{
		iPropertyObserverArray[i]->ActivateL();
		}
	}

// ---------------------------------------------------------------------------
// Cancels all pubsub objects.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFMRadioActiveIdleEngine::Cancel()
	{
	for ( TInt i = 0; i < iPropertyObserverArray.Count(); i++ )
		{
		iPropertyObserverArray[i]->Cancel();	
		}
	}

// ---------------------------------------------------------------------------
// Static constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFMRadioActiveIdleEngine* CFMRadioActiveIdleEngine::NewL( MFMRadioActiveIdleEngineNotifyHandler& aObserver )
    {
    CFMRadioActiveIdleEngine* self = new ( ELeave ) CFMRadioActiveIdleEngine( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CFMRadioActiveIdleEngine::~CFMRadioActiveIdleEngine()
    {
    iPropertyObserverArray.ResetAndDestroy();
    iPropertyObserverArray.Close();
	/*
    if ( iServ )
        {
        iServ->Close();
        }
    delete iServ;
    */
    }

// ---------------------------------------------------------------------------
// Invoked when a listened integer P&S key is changed.
// ---------------------------------------------------------------------------
//
void CFMRadioActiveIdleEngine::HandlePropertyChangeL( const TUid& aCategory, TUint aKey, TInt aValue )
    {
    if ( aCategory == KFMRadioPSUid )
        {
        switch ( aKey )
            {/*
            case KFMRadioPSDataVolume:
                iObserver.HandleRadioVolumeChangeL( aValue );
            break;*/
                    
            case KFMRadioPSDataTuningState:
                iObserver.HandleTuningStateChangeL( static_cast<TFMRadioPSTuningState>( aValue ) );
            break;

            case KFMRadioPSDataFrequency:
                iObserver.HandleFrequencyChangeL( aValue );
            break;
            
            case KFMRadioPSDataChannel:
                // Sync the cache values for channel name and index
                iPropertyObserverArray[EFMRadioPSDataChannelName]->ValueDes( ETrue );
                iObserver.HandleChannelChangeL( aValue );
            break;

            case KFMRadioPSDataChannelDataChanged:
                iPropertyObserverArray[EFMRadioPSDataChannelName]->ValueDes( ETrue );
                iObserver.HandleChannelModifyL( aValue );
            break;

            case KFMRadioPSDataRadioMuteState:
                iObserver.HandleMuteStateChangeL( static_cast<TFMRadioPSRadioMuteState>( aValue ) );
            break;

            case KFMRadioPSDataApplicationRunning:
                iObserver.HandleApplicationRunningStateChangeL( static_cast<TFMRadioPSApplicationRunningState>( aValue ) );
            break;
            
            case KFMRadioPSDataHeadsetStatus:
                iObserver.HandleAntennaStatusChangeL( static_cast<TFMRadioPSHeadsetStatus>( aValue ) );
            break;
        
            case KFMRadioPSDataFrequencyDecimalCount:
                iObserver.HandleFrequencyDecimalCountChangeL( static_cast<TFMRadioPSFrequencyDecimalCount>( aValue ) );
            break;

            case KFMRadioPSDataRadioPowerState:
                iObserver.HandlePowerStateChangeL( static_cast<TFMRadioPSRadioPowerState>( aValue ) );
            break;
            
            case KFMRadioActiveIdlePresetListCount:
               iObserver.HandlePresetListCountChangeL( aValue );
            break;
            
            case KFMRadioActiveIdlePresetListFocus:
               //iObserver.HandlePresetListFocusChangeL( aValue );
            break;
            
            default:
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// Invoked when a listened byte array P&S key is changed.
// ---------------------------------------------------------------------------
//
//void CFMRadioActiveIdleEngine::HandlePropertyChangeL( const TUid& /*aCategory*/, TUint /*aKey*/, const TDesC8& /*aValue*/ )
//    {
//    }
    
// ---------------------------------------------------------------------------
// Invoked when a listened text P&S key is changed.
// ---------------------------------------------------------------------------
//

void CFMRadioActiveIdleEngine::HandlePropertyChangeL( const TUid& aCategory, TUint aKey, const TDesC& aValue )
    {
	if ( aCategory == KFMRadioPSUid )
    	{
    	switch ( aKey )
    	    {
            case KFMRadioPSDataRDSProgramService:
    	        iObserver.HandleRDSProgramServiceChangeL( aValue );
    	    break;
            case KFMRadioPSDataRDSRadioText:
                iObserver.HandleRDSRadioTextChangeL( aValue );
                break;
            case KFMRadioPSDataChannelName:
                // Sync the cache valuse for channel name and index
                iPropertyObserverArray[EFMRadioActiveIdleChannelChangeObserver]->ValueInt( ETrue );
                iObserver.HandleChannelNameChangeL( aValue );
                break;
            default:
            break;
    	    }
    	}

    }
// ---------------------------------------------------------------------------
// Invoked when a listened text P&S key is changed.
// ---------------------------------------------------------------------------
//
void CFMRadioActiveIdleEngine::HandlePropertyChangeL( const TUid&, TUint, const TDesC8&)
    {
    }
    
// ---------------------------------------------------------------------------
// Invoked when an error has occured while fetching the new value of any listened P&S key.
// ---------------------------------------------------------------------------
//
void CFMRadioActiveIdleEngine::HandlePropertyChangeErrorL( const TUid& /*aCategory*/, TUint /*aKey*/, TInt aError )
    {
    if (aError < 0)
    	{
    	// If any error should pass to this method, handle it like 'application shutdown'.
    	// This situation will realize whenever the application is killed, by accident/by system/by an app crash
    	// or due to change to offline mode.
		HandlePropertyChangeL(KFMRadioPSUid, KFMRadioPSDataApplicationRunning, EFMRadioPSApplicationClosing);
    	}
    }

// ---------------------------------------------------------------------------
// Returns the current radio volume.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFMRadioActiveIdleEngine::RadioVolume() const
    {
    //return iPropertyObserverArray[EFMRadioActiveIdleRadioVolumeObserver]->ValueInt();
    return 0;
    }

// ---------------------------------------------------------------------------
// Sets the radio volume.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFMRadioActiveIdleEngine::AdjustRadioVolume( TFMRadioPSAdjustVolume aVolume ) const
    {
    RProperty::Set( KFMRadioPSUid, KFMRadioPSControlAdjustVolume, aVolume );
    }


// ---------------------------------------------------------------------------
// Returns the current tuning state.
// ---------------------------------------------------------------------------
//
EXPORT_C TFMRadioPSTuningState CFMRadioActiveIdleEngine::TuningState() const
    {
    return static_cast<TFMRadioPSTuningState>( iPropertyObserverArray[EFMRadioActiveIdleTuningStateObserver]->ValueInt() );
    }

// ---------------------------------------------------------------------------
// Returns the current frequency.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFMRadioActiveIdleEngine::Frequency() const
    {
    return iPropertyObserverArray[EFMRadioActiveIdleFrequencyObserver]->ValueInt();
    }

// ---------------------------------------------------------------------------
// Returns the currently active channel ID.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFMRadioActiveIdleEngine::Channel() const
    {
    return iPropertyObserverArray[EFMRadioActiveIdleChannelChangeObserver]->ValueInt();
    }

// ---------------------------------------------------------------------------
// Returns the currently active channel name.
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CFMRadioActiveIdleEngine::ChannelName() const
    {
    return iPropertyObserverArray[EFMRadioPSDataChannelName]->ValueDes();
    }

// ---------------------------------------------------------------------------
// Returns the current mute state.
// ---------------------------------------------------------------------------
//
EXPORT_C TFMRadioPSRadioMuteState CFMRadioActiveIdleEngine::MuteState() const
    {
    return static_cast<TFMRadioPSRadioMuteState>( iPropertyObserverArray[EFMRadioActiveIdleMuteStateObserver]->ValueInt() );
    }

// ---------------------------------------------------------------------------
// Sets the mute state.
// ---------------------------------------------------------------------------
//
EXPORT_C void CFMRadioActiveIdleEngine::SetMuteState( TFMRadioPSRadioMuteState aMuteState ) const
    {
    RProperty::Set( KFMRadioPSUid, KFMRadioPSControlSetRadioMuteState, aMuteState );
    }

// ---------------------------------------------------------------------------
// Returns the current antenna connectivity status.
// ---------------------------------------------------------------------------
//
EXPORT_C TFMRadioPSHeadsetStatus CFMRadioActiveIdleEngine::AntennaStatus() const
    {
    return static_cast<TFMRadioPSHeadsetStatus>( iPropertyObserverArray[EFMRadioActiveIdleHeadsetStatusObserver]->ValueInt() );
    }

// ---------------------------------------------------------------------------
// Returns the current frequency decimal count.
// ---------------------------------------------------------------------------
//
EXPORT_C TFMRadioPSFrequencyDecimalCount CFMRadioActiveIdleEngine::FrequencyDecimalCount() const
    {
    return static_cast<TFMRadioPSFrequencyDecimalCount>( iPropertyObserverArray[EFMRadioActiveIdleDecimalCountObserver]->ValueInt() );
    }

// ---------------------------------------------------------------------------
// Returns FM Radio application's running state.
// ---------------------------------------------------------------------------
//
EXPORT_C TFMRadioPSApplicationRunningState CFMRadioActiveIdleEngine::ApplicationRunningState() const
    {
    return static_cast<TFMRadioPSApplicationRunningState>( iPropertyObserverArray[EFMRadioActiveIdleApplicationObserver]->ValueInt() );
    }

// ---------------------------------------------------------------------------
// Returns the radio's power state.
// ---------------------------------------------------------------------------
//
EXPORT_C TFMRadioPSRadioPowerState CFMRadioActiveIdleEngine::PowerState() const
    {
    return static_cast<TFMRadioPSRadioPowerState>( iPropertyObserverArray[EFMRadioActiveIdlePowerStateObserver]->ValueInt() );
    }

// ---------------------------------------------------------------------------
// Returns RDS Program Service information.
// ---------------------------------------------------------------------------
//

EXPORT_C const TDesC& CFMRadioActiveIdleEngine::RDSProgramService() const
    {
    return iPropertyObserverArray[EFMRadioActiveIdleRDSProgramServiceObserver]->ValueDes();
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

