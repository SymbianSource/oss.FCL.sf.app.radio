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

#include "radiouid.hrh"

#include "cradioenginelogger.h"
#include "radiointernalpskeys.h"

#include "mradiocontroleventobserver.h"
#include "cradiopubsub.h"
#include "cradiopubsubimp.h"
#include "radioengineutils.h"

// This has to be the last include. 
#ifdef STUB_CONSTELLATION
#   include "RadioStubManager.h"
#   define KRadioPSUid KStub_KRadioPSUid
#endif //STUB_CONSTELLATION

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioPubSubImp::CRadioPubSubImp()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioPubSubImp::ConstructL()
    {
    RadioEngineUtils::InitializeL();
    LOG_METHOD_AUTO;

    // Define data properties.
    DefinePropertyL( KRadioPSDataChannel,               RProperty::EInt,    ETrue );
    DefinePropertyL( KRadioPSDataFrequency,             RProperty::EInt,    ETrue );
    DefinePropertyL( KRadioPSDataRadioPowerState,       RProperty::EInt,    ETrue );
    DefinePropertyL( KRadioPSDataVolume,                RProperty::EInt,    ETrue );
    DefinePropertyL( KRadioPSDataChannelDataChanged,    RProperty::EInt,    ETrue );
    DefinePropertyL( KRadioPSDataTuningState,           RProperty::EInt,    ETrue );
    DefinePropertyL( KRadioPSDataRadioMuteState,        RProperty::EInt,    ETrue );
    DefinePropertyL( KRadioPSDataApplicationRunning,    RProperty::EInt,    ETrue );
    DefinePropertyL( KRadioPSDataHeadsetStatus,         RProperty::EInt,    ETrue );
    DefinePropertyL( KRadioPSDataFrequencyDecimalCount, RProperty::EInt,    ETrue );
    DefinePropertyL( KRadioPSDataLoudspeakerStatus,     RProperty::EInt,    ETrue );
    DefinePropertyL( KRadioPSDataRdsProgramService,     RProperty::EText,   ETrue );
    DefinePropertyL( KRadioPSDataRdsRadioText,          RProperty::EText,   ETrue );
    DefinePropertyL( KRadioPSDataChannelName,           RProperty::EText,   ETrue );
    DefinePropertyL( KRadioPSDataActivePresetListCount, RProperty::EInt,    ETrue );
    DefinePropertyL( KRadioPSDataActivePresetListFocus, RProperty::EInt,    ETrue );
    DefinePropertyL( KRadioPSDataRadioAntennaState,     RProperty::EInt,    ETrue );

    // Define control properties.
    DefinePropertyL( KRadioPSControlStepToChannel,      RProperty::EInt, EFalse );
    DefinePropertyL( KRadioPSControlSetChannel,         RProperty::EInt, EFalse );
    DefinePropertyL( KRadioPSControlSeek,               RProperty::EInt, EFalse );
    DefinePropertyL( KRadioPSControlStepToFrequency,    RProperty::EInt, EFalse );
    DefinePropertyL( KRadioPSControlSetFrequency,       RProperty::EInt, EFalse );
    DefinePropertyL( KRadioPSControlAdjustVolume ,      RProperty::EInt, EFalse );
    DefinePropertyL( KRadioPSControlSetRadioMuteState,  RProperty::EInt, EFalse );
    DefinePropertyL( KRadioPSControlSetAudioOutput,     RProperty::EInt, EFalse );

    // Observe control properties.
    CreatePropertyObserverL( KRadioPSControlStepToChannel,      CRadioPropertyObserver::ERadioPropertyInt );
    CreatePropertyObserverL( KRadioPSControlSetChannel,         CRadioPropertyObserver::ERadioPropertyInt );
    CreatePropertyObserverL( KRadioPSControlSeek,               CRadioPropertyObserver::ERadioPropertyInt );
    CreatePropertyObserverL( KRadioPSControlStepToFrequency,    CRadioPropertyObserver::ERadioPropertyInt );
    CreatePropertyObserverL( KRadioPSControlSetFrequency,       CRadioPropertyObserver::ERadioPropertyInt );
    CreatePropertyObserverL( KRadioPSControlAdjustVolume,       CRadioPropertyObserver::ERadioPropertyInt );
    CreatePropertyObserverL( KRadioPSControlSetRadioMuteState,  CRadioPropertyObserver::ERadioPropertyInt );
    CreatePropertyObserverL( KRadioPSControlSetAudioOutput,     CRadioPropertyObserver::ERadioPropertyInt );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioPubSubImp* CRadioPubSubImp::NewL()
    {
    CRadioPubSubImp* self = new ( ELeave ) CRadioPubSubImp();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioPubSubImp::~CRadioPubSubImp()
    {
    iPropertyArray.ResetAndDestroy();

    DeleteProperty( KRadioPSDataChannel );
    DeleteProperty( KRadioPSDataFrequency );
    DeleteProperty( KRadioPSDataRadioPowerState );
    DeleteProperty( KRadioPSDataVolume );
    DeleteProperty( KRadioPSDataChannelDataChanged );
    DeleteProperty( KRadioPSDataTuningState );
    DeleteProperty( KRadioPSDataRadioMuteState );
    DeleteProperty( KRadioPSDataApplicationRunning );
    DeleteProperty( KRadioPSDataHeadsetStatus );
    DeleteProperty( KRadioPSDataFrequencyDecimalCount );
    DeleteProperty( KRadioPSDataLoudspeakerStatus );
    DeleteProperty( KRadioPSDataRdsProgramService );
    DeleteProperty( KRadioPSDataRdsRadioText );
    DeleteProperty( KRadioPSDataChannelName );
    DeleteProperty( KRadioPSDataActivePresetListCount );
    DeleteProperty( KRadioPSDataActivePresetListFocus );
    DeleteProperty( KRadioPSDataRadioAntennaState );

    DeleteProperty( KRadioPSControlStepToChannel );
    DeleteProperty( KRadioPSControlSetChannel );
    DeleteProperty( KRadioPSControlSeek );
    DeleteProperty( KRadioPSControlStepToFrequency );
    DeleteProperty( KRadioPSControlSetFrequency );
    DeleteProperty( KRadioPSControlAdjustVolume );
    DeleteProperty( KRadioPSControlSetRadioMuteState );
    DeleteProperty( KRadioPSControlSetAudioOutput );

    RadioEngineUtils::Release();
    }

// ---------------------------------------------------------------------------
// Defines a property.
// ---------------------------------------------------------------------------
//
void CRadioPubSubImp::DefinePropertyL( TUint aKey, TInt aAttr, TBool aDataProperty ) const
    {
    _LIT_SECURITY_POLICY_PASS( KRadioExternalAccessPolicy );
    _LIT_SECURITY_POLICY_S0( KRadioInternalAccessPolicy, KUidRadioApplication );

    TInt err = KErrNone;
    if ( aDataProperty )
        {
        // For data properties, grant read access for all, but write access only for radio application
        err = RProperty::Define( KRadioPSUid, aKey, aAttr, KRadioExternalAccessPolicy, KRadioInternalAccessPolicy );
        }
    else
        {
        // For control properties, grant read access only for radio application, but write access for all
        err = RProperty::Define( KRadioPSUid, aKey, aAttr, KRadioInternalAccessPolicy, KRadioExternalAccessPolicy );
        }

    if ( err && err != KErrAlreadyExists )
        {
        LOG_FORMAT( "CRadioPubSubImp::DefinePropertyL: Leaving with %d", err );
        User::Leave( err );
        }
    }

// ---------------------------------------------------------------------------
// Deletes a property
// ---------------------------------------------------------------------------
//
void CRadioPubSubImp::DeleteProperty( TUint aKey ) const
    {
    RProperty::Delete( KRadioPSUid, aKey );
    }

// ---------------------------------------------------------------------------
// Creates a property observer
// ---------------------------------------------------------------------------
//
void CRadioPubSubImp::CreatePropertyObserverL( const TUint aKey,
                                               const TInt aPropertyType )
    {
    CRadioPropertyObserver* observer = CRadioPropertyObserver::NewL( *this, KRadioPSUid,
                    aKey, static_cast<CRadioPropertyObserver::TRadioPropertyType>( aPropertyType ) );

    if ( iPropertyArray.Append( observer ) == KErrNone )
        {
        observer->ActivateL();
        }
    else
        {
        delete observer;
        observer = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Gets the integer value stored in a property.
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::Get( TUint aKey, TInt& aValue ) const
    {
    return RProperty::Get( KRadioPSUid, aKey, aValue ) == KErrNone;
    }

// ---------------------------------------------------------------------------
// Sets a property's value.
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::Set( TUint aKey, TInt aValue ) const
    {
    return RProperty::Set( KRadioPSUid, aKey, aValue ) == KErrNone;
    }

// ---------------------------------------------------------------------------
// Sets a text property value
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::Set( TUint aKey, const TDesC& aValue ) const
    {
    return RProperty::Set( KRadioPSUid, aKey, aValue ) == KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioPubSubImp::HandlePropertyChangeL( const TUid& aCategory,
                                             const TUint aKey,
                                             const TInt aValue )
    {
    LOG_FORMAT( "CRadioPubSubImp::HandlePropertyChangeL: Category: %d, Key: %u. Value: %d",
                                                aCategory.iUid, aKey, aValue );

    if ( aCategory != KRadioPSUid )
        {
        LOG( "CRadioPubSubImp::HandlePropertyChangeL: Unknown category. Leaving" );
        User::Leave( KErrArgument );
        }

    if ( !iObserver )
        {
        return;
        }

    if ( aKey == KRadioPSControlStepToChannel )
        {
        if ( aValue == ERadioPSStepToChannelUp )
            {
            iObserver->StepToChannelL( RadioEngine::ERadioUp );
            }
        else if ( aValue == ERadioPSStepToChannelDown )
            {
            iObserver->StepToChannelL( RadioEngine::ERadioDown );
            }
        }
    else if ( aKey == KRadioPSControlSetChannel )
        {
        if ( aValue > 0 )
            {
            iObserver->SetChannelL( aValue );
            }
        }
    else if ( aKey == KRadioPSControlSeek )
        {
        if ( aValue == ERadioPSSeekUp )
            {
            iObserver->SeekL( RadioEngine::ERadioUp );
            }
        else if ( aValue == ERadioPSSeekDown )
            {
            iObserver->SeekL( RadioEngine::ERadioDown );
            }
        }
    else if ( aKey == KRadioPSControlStepToFrequency )
        {
        if ( aValue == ERadioPSStepToFrequencyUp )
            {
            iObserver->StepToFrequencyL( RadioEngine::ERadioUp );
            }
        else if ( aValue == ERadioPSStepToFrequencyDown )
            {
            iObserver->StepToFrequencyL( RadioEngine::ERadioDown );
            }
        }
    else if ( aKey == KRadioPSControlSetFrequency )
        {
        if ( aValue > 0 )
            {
            iObserver->SetFrequencyL( aValue );
            }
        }
    else if ( aKey == KRadioPSControlAdjustVolume )
        {
        if ( aValue == ERadioPSIncreaseVolume )
            {
            iObserver->AdjustVolumeL( RadioEngine::ERadioIncVolume );
            }
        else if ( aValue == ERadioPSDecreaseVolume )
            {
            iObserver->AdjustVolumeL( RadioEngine::ERadioDecVolume );
            }
        }
    else if ( aKey == KRadioPSControlSetRadioMuteState )
        {
        if ( aValue == ERadioPSMuteStateOn )
            {
            iObserver->MuteL( ETrue );
            }
        else if ( aValue == ERadioPSMuteStateOff )
            {
            iObserver->MuteL( EFalse );
            }
        }
    else if ( aKey == KRadioPSControlSetAudioOutput )
        {
        if ( aValue == ERadioPSAudioOutputHeadset )
            {
            iObserver->SetAudioRouteL( RadioEngine::ERadioHeadset );
            }
        else if ( aValue == ERadioPSAudioOutputSpeaker )
            {
            iObserver->SetAudioRouteL( RadioEngine::ERadioSpeaker );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::PublishChannel( TInt aChannelId ) const
    {
    LOG_FORMAT( "CRadioPubSubImp::PublishChannel ( aChannelId = %d )", aChannelId );

    TInt oldChannelId = 0;
    if ( Get( KRadioPSDataChannel, oldChannelId ) && aChannelId != oldChannelId )
        {
        return Set( KRadioPSDataChannel, aChannelId );
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::PublishFrequency( TUint32 aFreq ) const
    {
    LOG_FORMAT( "CRadioPubSubImp::PublishFrequency ( aFreq = %u )", aFreq );

    TInt oldFreq = 0;
    if ( Get( KRadioPSDataFrequency, oldFreq ) && aFreq != oldFreq )
        {
        return Set( KRadioPSDataFrequency, aFreq );
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::PublishPowerState( TBool aPowerOn ) const
    {
    LOG_FORMAT( "CRadioPubSubImp::PublishStateL ( aPowerOn = %d )", aPowerOn );
    // If VR process gets killed, power on state remains published.
    // Must not compare state transition here, because then power on might not be published.
    TRadioPSRadioPowerState newState = aPowerOn ? ERadioPSRadioPowerOn : ERadioPSRadioPowerOff;
    return Set( KRadioPSDataRadioPowerState, newState );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::PublishVolume( TInt aVol ) const
    {
    LOG_FORMAT( "CRadioPubSubImp::PublishVolume ( aVol = %d )", aVol );

    TInt oldVol = 0;
    if ( Get( KRadioPSDataVolume, oldVol ) && aVol != oldVol )
        {
        return Set( KRadioPSDataVolume, aVol );
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::PublishChannelDataChanged( TInt aChannelId ) const
    {
    LOG_FORMAT( "CRadioPubSubImp::PublishChannelDataChangedL ( aChannelId = %d )", aChannelId );

    // Channel data changed notifications are allowed for the same channel multiple times, as the same channel's
    // data can be changed many times in a row.

    return Set( KRadioPSDataChannelDataChanged, aChannelId );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::PublishTuningState( TRadioPSTuningState aTuningState ) const
    {
    LOG_FORMAT( "CRadioPubSubImp::PublishSeekingStateL ( aTuningState = %d )", aTuningState );

    TInt oldState = 0;
    if ( Get( KRadioPSDataTuningState, oldState ) && aTuningState != oldState )
        {
        return Set( KRadioPSDataTuningState, aTuningState );
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::PublishRadioMuteState( TBool aMuted ) const
    {
    LOG_FORMAT( "CRadioPubSubImp::PublishMuteStateL ( aMuted = %d )", aMuted );

    TRadioPSRadioMuteState newState = aMuted ? ERadioPSMuteStateOn : ERadioPSMuteStateOff;

    TInt oldState = 0;
    if ( Get( KRadioPSDataRadioMuteState, oldState ) && oldState != newState )
        {
        return Set( KRadioPSDataRadioMuteState, newState );
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::PublishApplicationRunningState( TRadioPSApplicationRunningState aRunningState ) const
    {
    LOG_FORMAT( "CRadioPubSubImp::PublishApplicationRunningStateL ( aRunningState = %d )", aRunningState );
    // If VR process gets killed, wrong running state remains published.
    // Must not compare state transition here, because then application start might not be published.
    return Set( KRadioPSDataApplicationRunning, aRunningState );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::PublishHeadsetStatus( TRadioPSHeadsetStatus aHeadsetStatus ) const
    {
    LOG_FORMAT( "CRadioPubSubImp::PublishHeadsetStatusL ( aHeadsetStatus = %d )", aHeadsetStatus );

    TInt oldStatus = 0;
    if ( Get( KRadioPSDataHeadsetStatus, oldStatus ) && aHeadsetStatus != oldStatus )
        {
        return Set( KRadioPSDataHeadsetStatus, aHeadsetStatus );
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::PublishFrequencyDecimalCount( TRadioPSFrequencyDecimalCount aDecimalCount ) const
    {
    LOG_FORMAT( "CRadioPubSubImp::PublishFrequencyDecimalCountL ( aDecimalCount = %d )", aDecimalCount );

    TInt oldCount = 0;
    if ( Get( KRadioPSDataFrequencyDecimalCount, oldCount ) && aDecimalCount != oldCount )
        {
        return Set( KRadioPSDataFrequencyDecimalCount, aDecimalCount );
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::PublishLoudspeakerStatus( TRadioPSLoudspeakerStatus aLoudspeakerStatus ) const
    {
    LOG_FORMAT( "CRadioPubSubImp::PublishLoudspeakerStatusL ( aLoudspeakerStatus = %d )", aLoudspeakerStatus );
    TInt oldStatus = 0;
    if ( Get( KRadioPSDataLoudspeakerStatus, oldStatus ) && oldStatus != aLoudspeakerStatus )
        {
        return Set( KRadioPSDataLoudspeakerStatus, aLoudspeakerStatus );
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::PublishRdsProgramService( const TDesC& aProgramService ) const
    {
    return Set( KRadioPSDataRdsProgramService, aProgramService );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::PublishRdsRadioText( const TDesC& aRadioText ) const
    {
    return Set( KRadioPSDataRdsRadioText, aRadioText );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::PublishChannelName( const TDesC& aName ) const
    {
    return Set( KRadioPSDataChannelName, aName );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::PublishActivePreseListCount( TInt aCount ) const
    {
    return Set( KRadioPSDataActivePresetListCount, aCount );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::PublishActivePreseListFocus( TInt aIndex ) const
    {
    return Set( KRadioPSDataActivePresetListFocus, aIndex );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioPubSubImp::PublishAntennaState( TRadioPSRadioAntennaState aState ) const
    {
    return Set( KRadioPSDataRadioAntennaState, aState );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioPubSubImp::SetControlEventObserver( MRadioControlEventObserver* aControlEventObserver )
    {
    iObserver = aControlEventObserver;
    }
