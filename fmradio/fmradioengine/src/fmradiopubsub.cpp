/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Publish&Subscribe component of FM Radio
*
*/

#include "fmradiocontroleventobserver.h"

#include "fmradiopubsub.h"


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CFMRadioPubSub::CFMRadioPubSub()
    {
    }

// ---------------------------------------------------------------------------
// EPOC default constructor can leave.
// ---------------------------------------------------------------------------
//
void CFMRadioPubSub::ConstructL()
    {    
    // Define data properties.
    DefinePropertyL( KFMRadioPSDataChannel, RProperty::EInt, ETrue );
    DefinePropertyL( KFMRadioPSDataFrequency, RProperty::EInt, ETrue );
    DefinePropertyL( KFMRadioPSDataRadioPowerState, RProperty::EInt, ETrue );
    DefinePropertyL( KFMRadioPSDataVolume, RProperty::EInt, ETrue );
    DefinePropertyL( KFMRadioPSDataChannelDataChanged, RProperty::EInt, ETrue );
    DefinePropertyL( KFMRadioPSDataTuningState, RProperty::EInt, ETrue );
    DefinePropertyL( KFMRadioPSDataRadioMuteState, RProperty::EInt, ETrue );
    DefinePropertyL( KFMRadioPSDataApplicationRunning, RProperty::EInt, ETrue );
    DefinePropertyL( KFMRadioPSDataHeadsetStatus, RProperty::EInt, ETrue );
    DefinePropertyL( KFMRadioPSDataFrequencyDecimalCount, RProperty::EInt, ETrue );
    DefinePropertyL( KFMRadioPSDataLoudspeakerStatus, RProperty::EInt, ETrue );
    DefinePropertyL( KFMRadioPSDataRDSProgramService, RProperty::EText, ETrue );
    DefinePropertyL( KFMRadioPSDataRDSRadioText, RProperty::EText, ETrue );
    DefinePropertyL( KFMRadioPSDataChannelName, RProperty::EText, ETrue );
    
    // Define control properties.
    DefinePropertyL( KFMRadioPSControlStepToChannel, RProperty::EInt, EFalse );
    DefinePropertyL( KFMRadioPSControlSeek, RProperty::EInt, EFalse );
    DefinePropertyL( KFMRadioPSControlSetRadioMuteState , RProperty::EInt, EFalse );
    // Observe control properties.
    DefinePropertyL( KFMRadioActiveIdlePresetListCount, RProperty::EInt, ETrue );    
    DefinePropertyL( KFMRadioActiveIdlePresetListFocus, RProperty::EInt, ETrue );
    
    
    CFMRadioPropertyObserver* observer = CFMRadioPropertyObserver::NewLC( *this,
            KFMRadioPSUid, KFMRadioPSControlStepToChannel, CFMRadioPropertyObserver::EFMRadioPropertyInt);
    iPropertyArray.AppendL( observer );
    CleanupStack::Pop( observer );
    
    observer = CFMRadioPropertyObserver::NewLC( *this,
            KFMRadioPSUid, KFMRadioPSControlSeek, CFMRadioPropertyObserver::EFMRadioPropertyInt );
    iPropertyArray.AppendL( observer );
    CleanupStack::Pop( observer );
    
    observer = CFMRadioPropertyObserver::NewLC( *this,
            KFMRadioPSUid, KFMRadioPSControlSetRadioMuteState, CFMRadioPropertyObserver::EFMRadioPropertyInt );
    iPropertyArray.AppendL( observer );
    CleanupStack::Pop( observer );
    
    observer = CFMRadioPropertyObserver::NewLC( *this,
            KFMRadioPSUid, KFMRadioActiveIdlePresetListCount, CFMRadioPropertyObserver::EFMRadioPropertyInt );
    iPropertyArray.AppendL( observer );
    CleanupStack::Pop( observer );

    for ( TInt i = 0; i < iPropertyArray.Count(); i++ )
		{
		iPropertyArray[i]->ActivateL();	
		}
    }

// -------------------------------------------------------f--------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CFMRadioPubSub* CFMRadioPubSub::NewL()
    {
    CFMRadioPubSub* self = new (ELeave) CFMRadioPubSub();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CFMRadioPubSub::~CFMRadioPubSub()
    {
    iPropertyArray.ResetAndDestroy();
	iPropertyArray.Close();
	
	DeleteProperty( KFMRadioPSDataChannel );
	DeleteProperty( KFMRadioPSDataFrequency );
	DeleteProperty( KFMRadioPSDataRadioPowerState );
	DeleteProperty( KFMRadioPSDataVolume );
	DeleteProperty( KFMRadioPSDataChannelDataChanged );
	DeleteProperty( KFMRadioPSDataTuningState );
	DeleteProperty( KFMRadioPSDataRadioMuteState );
	DeleteProperty( KFMRadioPSDataApplicationRunning );
    DeleteProperty( KFMRadioPSDataHeadsetStatus );
    DeleteProperty( KFMRadioPSDataFrequencyDecimalCount );
    DeleteProperty( KFMRadioPSDataLoudspeakerStatus );
    DeleteProperty( KFMRadioPSDataRDSProgramService );
    DeleteProperty( KFMRadioPSDataRDSRadioText );
    DeleteProperty( KFMRadioPSDataChannelName );
    
	DeleteProperty( KFMRadioPSControlStepToChannel );
	DeleteProperty( KFMRadioPSControlSeek );
	DeleteProperty( KFMRadioPSControlSetRadioMuteState );
    DeleteProperty ( KFMRadioActiveIdlePresetListCount);
	DeleteProperty ( KFMRadioActiveIdlePresetListFocus);
    }

// ---------------------------------------------------------------------------
// Defines a property.
// ---------------------------------------------------------------------------
//
void CFMRadioPubSub::DefinePropertyL( TUint aKey, TInt aAttr, TBool aDataProperty ) const
    {
    /*lint -save -e648*/
    _LIT_SECURITY_POLICY_PASS( KFMRadioExternalAccessPolicy );
    /*lint -restore*/
    _LIT_SECURITY_POLICY_S0( KFMRadioInternalAccessPolicy, KUidFMRadioApplication );
    
    TInt err = KErrNone;
    if ( aDataProperty )
        {   
        // For data properties, grant read access for all, but write access only for FMRadioEngine
        err = RProperty::Define( KFMRadioPSUid, aKey, aAttr, KFMRadioExternalAccessPolicy, KFMRadioInternalAccessPolicy );
        }
    else
        {
        // For control properties, grant read access only for FMRadioEngine , but write access for all
        err = RProperty::Define( KFMRadioPSUid, aKey, aAttr, KFMRadioInternalAccessPolicy, KFMRadioExternalAccessPolicy );
        }
    
    
    if ( err && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }
    }

// ---------------------------------------------------------------------------
// Deletes a property
// ---------------------------------------------------------------------------
//
void CFMRadioPubSub::DeleteProperty( TUint aKey ) const
    {
    RProperty::Delete( KFMRadioPSUid, aKey );
    }

// ---------------------------------------------------------------------------
// Gets the integer value stored in a property.
// ---------------------------------------------------------------------------
//
TInt CFMRadioPubSub::PropertyValueIntL( TUint aKey ) const
    {
    TInt value;
    User::LeaveIfError( RProperty::Get( KFMRadioPSUid, aKey, value ) );
    return value;
    }

// ---------------------------------------------------------------------------
// Sets a property's value.
// ---------------------------------------------------------------------------
//
void CFMRadioPubSub::SetPropertyValueL( TUint aKey, TInt aValue ) const
    {
    User::LeaveIfError( RProperty::Set( KFMRadioPSUid, aKey, aValue ) );
    }

// ---------------------------------------------------------------------------
// Sets a text property value
// ---------------------------------------------------------------------------
//
void CFMRadioPubSub::SetPropertyValueL( TUint aKey, const TDesC& aValue ) const
    {
    User::LeaveIfError( RProperty::Set( KFMRadioPSUid, aKey, aValue ) );
    }

// ---------------------------------------------------------------------------
// CFMRadioPubSub::HandlePropertyChangeL
// ---------------------------------------------------------------------------
//
void CFMRadioPubSub::HandlePropertyChangeL( const TUid& aCategory, const TUint aKey, const TInt aValue )
	{		
	if (aCategory != KFMRadioPSUid)
		{
		User::Leave(KErrArgument);
		}
	
	if (iObserver)
		{
		if (aKey == KFMRadioPSControlStepToChannel)
	    	{
	    	switch (aValue)
	    		{
				case EFMRadioPSStepToChannelUninitialized:
					{
					break;	
					}
				case EFMRadioPSStepToChannelUp:
					{
					iObserver->StepToChannelL(EFMRadioUp);
					break;
					}
				case EFMRadioPSStepToChannelDown:
					{
					iObserver->StepToChannelL(EFMRadioDown);
					break;
					}
	 			default:
	 				{
	 				break; 
	 				}			
	    		}
	    	}
	    else if (aKey == KFMRadioPSControlSeek)
	    	{
	    	switch (aValue)
	    		{
				case EFMRadioPSSeekUninitialized:
					{
					break;	
					}
				case EFMRadioPSSeekUp:
					{
					iObserver->SeekL(EFMRadioUp);
					break;
					}
				case EFMRadioPSSeekDown:
					{
					iObserver->SeekL(EFMRadioDown);
					break;
					}
	 			default:
	 				{
	 				break; 
	 				}			
	    		}
	    	}
	    else if (aKey == KFMRadioPSControlSetRadioMuteState)
	    	{
	    	switch (aValue)
	    		{
				case EFMRadioPSMuteStateUninitialized:
					{
					break;	
					}
				case EFMRadioPSMuteStateOn:
					{
					iObserver->MuteL(ETrue);
					break;
					}
				case EFMRadioPSMuteStateOff:
					{
					iObserver->MuteL(EFalse);
					break;
					}
	 			default:
	 				{
	 				break; 
	 				}			
	    		}
	    	}
		}
	}
	
// ---------------------------------------------------------------------------
// Currently empty implementation because we don't listen byte array type properties.
// ---------------------------------------------------------------------------
//
void CFMRadioPubSub::HandlePropertyChangeL(const TUid& /*aCategory*/, const TUint /*aKey*/, const TDesC8& /*aValue*/)
	{
	}
	
// ---------------------------------------------------------------------------
// Currently empty implementation because we don't listen text type properties.
// ---------------------------------------------------------------------------
//
void CFMRadioPubSub::HandlePropertyChangeL(const TUid& /*aCategory*/, const TUint /*aKey*/, const TDesC& /*aValue*/)
	{
	}
	
// ---------------------------------------------------------------------------
// CFMRadioPubSub::HandlePropertyChangeErrorL
// ---------------------------------------------------------------------------
//
void CFMRadioPubSub::HandlePropertyChangeErrorL (const TUid& /*aCategory*/, const TUint /*aKey*/, TInt /*aError*/ )
	{
	//NOP
	}

// ---------------------------------------------------------------------------
// CFMRadioPubSub::PublishChannelL
// ---------------------------------------------------------------------------
//	
void CFMRadioPubSub::PublishChannelL(TInt aChannelId) const
	{	    
    TInt oldChannelId = PropertyValueIntL( KFMRadioPSDataChannel );

    if ( aChannelId != oldChannelId )
        {
    	SetPropertyValueL( KFMRadioPSDataChannel, aChannelId );
        }
	}
	
// ---------------------------------------------------------------------------
// CFMRadioPubSub::PublishFrequencyL
// ---------------------------------------------------------------------------
//	
EXPORT_C void CFMRadioPubSub::PublishFrequencyL( TUint32 aFreq ) const
	{	
    TInt oldFreq = PropertyValueIntL( KFMRadioPSDataFrequency );
    
    if ( aFreq != oldFreq )
        {
    	SetPropertyValueL( KFMRadioPSDataFrequency, aFreq );
        }
	}
	
// ---------------------------------------------------------------------------
// CFMRadioPubSub::PublishStateL
// ---------------------------------------------------------------------------
//	
EXPORT_C void CFMRadioPubSub::PublishStateL( TBool aPowerOn ) const
	{	
    // If application process gets killed, power on state remains published.
    // Must not compare state transition here, because then power on might not be published.
	TFMRadioPSRadioPowerState newState = aPowerOn ? EFMRadioPSRadioPowerOn : EFMRadioPSRadioPowerOff;
    SetPropertyValueL( KFMRadioPSDataRadioPowerState, newState );
	}
	
// ---------------------------------------------------------------------------
// CFMRadioPubSub::PublishVolumeL
// ---------------------------------------------------------------------------
//	
void CFMRadioPubSub::PublishVolumeL( TInt aVol ) const
	{	
	TInt oldVol = PropertyValueIntL( KFMRadioPSDataVolume );
	
	if ( aVol != oldVol )
	    {
    	SetPropertyValueL( KFMRadioPSDataVolume, aVol );
	    }
	}

// ---------------------------------------------------------------------------
// CFMRadioPubSub::PublishPresetCountL
// ---------------------------------------------------------------------------
//	
EXPORT_C void CFMRadioPubSub::PublishPresetCountL( TInt aVal ) const
	{	
	TInt oldVal = PropertyValueIntL( KFMRadioActiveIdlePresetListCount );
	
	if ( aVal != oldVal )
	    {
    	SetPropertyValueL( KFMRadioActiveIdlePresetListCount, aVal );
	    }
	}

// ---------------------------------------------------------------------------
// CFMRadioPubSub::PublishChannelDataChangedL
// ---------------------------------------------------------------------------
//	
void CFMRadioPubSub::PublishChannelDataChangedL( TInt aChannelId ) const
	{		
	// Channel data changed notifications are allowed for the same channel multiple times, as the same channel's
	// data can be changed many times in a row.
	
	SetPropertyValueL( KFMRadioPSDataChannelDataChanged, aChannelId );
	}

// ---------------------------------------------------------------------------
// CFMRadioPubSub::PublishTuningStateL
// ---------------------------------------------------------------------------
//	
void CFMRadioPubSub::PublishTuningStateL( TFMRadioPSTuningState aTuningState ) const
    {	
    TFMRadioPSTuningState oldState = static_cast<TFMRadioPSTuningState>( PropertyValueIntL( KFMRadioPSDataTuningState ) );
	
	if ( aTuningState != oldState )
	    {
        SetPropertyValueL( KFMRadioPSDataTuningState, aTuningState );
	    }
    }

// ---------------------------------------------------------------------------
// CFMRadioPubSub::PublishMuteStateL
// ---------------------------------------------------------------------------
//
void CFMRadioPubSub::PublishRadioMuteStateL( TBool aMuted ) const
	{
	TFMRadioPSRadioMuteState oldState = static_cast<TFMRadioPSRadioMuteState>( PropertyValueIntL( KFMRadioPSDataRadioMuteState ) );
	TFMRadioPSRadioMuteState newState = aMuted ? EFMRadioPSMuteStateOn : EFMRadioPSMuteStateOff;
	
	if ( newState != oldState )
	    {
        SetPropertyValueL( KFMRadioPSDataRadioMuteState, newState );
	    }
	}
	
// ---------------------------------------------------------------------------
// CFMRadioPubSub::PublishApplicationRunningStateL
// ---------------------------------------------------------------------------
//
EXPORT_C void CFMRadioPubSub::PublishApplicationRunningStateL( TFMRadioPSApplicationRunningState aRunningState ) const
	{	
    // If application process gets killed, wrong running state remains published.
    // Must not compare state transition here, because then application start might not be published.
    SetPropertyValueL( KFMRadioPSDataApplicationRunning, aRunningState );
	}
	
// ---------------------------------------------------------------------------
// CFMRadioPubSub::PublishAntennaStatusL
// ---------------------------------------------------------------------------
//
void CFMRadioPubSub::PublishAntennaStatusL( TFMRadioPSHeadsetStatus aAntennaStatus ) const
	{	
    TFMRadioPSHeadsetStatus oldState = static_cast<TFMRadioPSHeadsetStatus>( PropertyValueIntL( KFMRadioPSDataHeadsetStatus ) );
	
	if ( aAntennaStatus != oldState )
	    {
        SetPropertyValueL( KFMRadioPSDataHeadsetStatus, aAntennaStatus );
	    }
	}
	
// ---------------------------------------------------------------------------
// CFMRadioPubSub::PublishFrequencyDecimalCountL
// ---------------------------------------------------------------------------
//
void CFMRadioPubSub::PublishFrequencyDecimalCountL( TFMRadioPSFrequencyDecimalCount aDecimalCount ) const
	{	
    TFMRadioPSFrequencyDecimalCount oldCount = static_cast<TFMRadioPSFrequencyDecimalCount>( PropertyValueIntL( KFMRadioPSDataFrequencyDecimalCount ) );
	
	if ( aDecimalCount != oldCount )
	    {
        SetPropertyValueL( KFMRadioPSDataFrequencyDecimalCount, aDecimalCount );
	    }
	}

// ---------------------------------------------------------------------------
// CFMRadioPubSub::PublishLoudspeakerStatusL
// ---------------------------------------------------------------------------
//
void CFMRadioPubSub::PublishLoudspeakerStatusL( 
    TFMRadioPSLoudspeakerStatus aLoudspeakerStatus ) const
    {
    TFMRadioPSLoudspeakerStatus oldLoudspeakerStatus 
        = static_cast<TFMRadioPSLoudspeakerStatus>( 
        PropertyValueIntL(KFMRadioPSDataLoudspeakerStatus) );
    
    if(oldLoudspeakerStatus != aLoudspeakerStatus)
        {
        SetPropertyValueL( KFMRadioPSDataLoudspeakerStatus, aLoudspeakerStatus);
        }	
	}

// ---------------------------------------------------------------------------
// CFMRadioPubSub::PublishRDSProgramServiceL
// ---------------------------------------------------------------------------
//
EXPORT_C void CFMRadioPubSub::PublishRDSProgramServiceL( const TDesC& aProgramService ) const
    {
    SetPropertyValueL( KFMRadioPSDataRDSProgramService, aProgramService );
	}

// ---------------------------------------------------------------------------
// CFMRadioPubSub::PublishRDSRadioTextL
// ---------------------------------------------------------------------------
//
EXPORT_C void CFMRadioPubSub::PublishRDSRadioTextL( const TDesC& aRadioText ) const
    {
    SetPropertyValueL( KFMRadioPSDataRDSRadioText, aRadioText );
    }

// ---------------------------------------------------------------------------
// CFMRadioPubSub::PublishChannelNameL
// ---------------------------------------------------------------------------
//
EXPORT_C void CFMRadioPubSub::PublishChannelNameL( const TDesC& aName ) const
    {
    SetPropertyValueL( KFMRadioPSDataChannelName, aName );
    }

// ---------------------------------------------------------------------------
// CFMRadioPubSub::SetControlEventObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void CFMRadioPubSub::SetControlEventObserver( MFMRadioControlEventObserver* aControlEventObserver )
    {
    iObserver = aControlEventObserver;
    }

