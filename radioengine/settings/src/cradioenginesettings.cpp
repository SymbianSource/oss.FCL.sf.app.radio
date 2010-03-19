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
#include <barsread.h>
#include <coemain.h>
#include "radiointernalcrkeys.h"
#include "radioenginesettings.rsg"

#include "radioenginedef.h"
#include "cradioenginesettings.h"
#include "mradiosettingsobserver.h"
#include "cradiorepositorymanager.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineSettings* CRadioEngineSettings::NewL( CRadioRepositoryManager& aRepositoryManager,
                                                  CCoeEnv& aCoeEnv )
    {
    CRadioEngineSettings* self = new ( ELeave ) CRadioEngineSettings( aRepositoryManager, aCoeEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineSettings::ConstructL()
    {
    iRepositoryManager.AddObserverL( this );
    iRepositoryManager.AddEntityL( KRadioCRUid, KRadioCRHeadsetVolume,          CRadioRepositoryManager::ERadioEntityInt );
    iRepositoryManager.AddEntityL( KRadioCRUid, KRadioCRSpeakerVolume,          CRadioRepositoryManager::ERadioEntityInt );
    iRepositoryManager.AddEntityL( KRadioCRUid, KRadioCROutputMode,             CRadioRepositoryManager::ERadioEntityInt );
    iRepositoryManager.AddEntityL( KRadioCRUid, KRadioCRAudioRoute,             CRadioRepositoryManager::ERadioEntityInt );
    iRepositoryManager.AddEntityL( KRadioCRUid, KRadioCRHeadsetMuteState,       CRadioRepositoryManager::ERadioEntityInt );
    iRepositoryManager.AddEntityL( KRadioCRUid, KRadioCRSpeakerMuteState,       CRadioRepositoryManager::ERadioEntityInt );
    iRepositoryManager.AddEntityL( KRadioCRUid, KRadioCRVisualRadioPowerState,  CRadioRepositoryManager::ERadioEntityInt );
    iRepositoryManager.AddEntityL( KRadioCRUid, KRadioCRTunedFrequency,         CRadioRepositoryManager::ERadioEntityInt );
    iRepositoryManager.AddEntityL( KRadioCRUid, KRadioCRDefaultMinVolumeLevel,  CRadioRepositoryManager::ERadioEntityInt );
    iRepositoryManager.AddEntityL( KRadioCRUid, KRadioCRCurrentRegion,          CRadioRepositoryManager::ERadioEntityInt );
    iRepositoryManager.AddEntityL( KRadioCRUid, KRadioCRRdsAfSearch,            CRadioRepositoryManager::ERadioEntityInt );
    iRepositoryManager.AddEntityL( KRadioCRUid, KRadioCRNetworkId,              CRadioRepositoryManager::ERadioEntityDes16 );
    iRepositoryManager.AddEntityL( KRadioCRUid, KRadioCRSubscriberId,           CRadioRepositoryManager::ERadioEntityDes16 );
    iRepositoryManager.AddEntityL( KRadioCRUid, KRadioCRCountryCode,            CRadioRepositoryManager::ERadioEntityDes16 );

    InitializeRegionsL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineSettings::CRadioEngineSettings( CRadioRepositoryManager& aRepositoryManager, CCoeEnv& aCoeEnv )
    : CRadioSettingsBase( aRepositoryManager, aCoeEnv )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineSettings::~CRadioEngineSettings()
    {
    iRepositoryManager.RemoveObserver( this );
    iRegions.ResetAndDestroy();
    iRegions.Close();
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
// ---------------------------------------------------------------------------
//
void CRadioEngineSettings::SetObserver( MRadioSettingsObserver* aObserver )
    {
    iObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetHeadsetVolume( TInt aVolume )
    {
    return iRepositoryManager.SetEntityValue( KRadioCRUid, KRadioCRHeadsetVolume, aVolume );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::HeadsetVolume() const
    {
    return iRepositoryManager.EntityValueInt( KRadioCRUid, KRadioCRHeadsetVolume );
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetSpeakerVolume( TInt aVolume )
    {
    return iRepositoryManager.SetEntityValue( KRadioCRUid, KRadioCRSpeakerVolume, aVolume );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SpeakerVolume() const
    {
    return iRepositoryManager.EntityValueInt( KRadioCRUid, KRadioCRSpeakerVolume );
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetVolume( TInt aVolume )
    {
    if ( AudioRoute() == RadioEngine::ERadioHeadset )
        {
        return SetHeadsetVolume( aVolume );
        }
    else
        {
        return SetSpeakerVolume( aVolume );
        }
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::Volume() const
    {
    if ( AudioRoute() == RadioEngine::ERadioHeadset )
        {
        return HeadsetVolume();
        }
    else
        {
        return SpeakerVolume();
        }
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetOutputMode( TInt aOutputMode )
    {
    return iRepositoryManager.SetEntityValue( KRadioCRUid, KRadioCROutputMode, aOutputMode );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::OutputMode() const
    {
    return iRepositoryManager.EntityValueInt( KRadioCRUid, KRadioCROutputMode );
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
// Stores the radio audio route ( headset/ihf ) to use.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetAudioRoute( TInt aAudioRoute )
    {
    return iRepositoryManager.SetEntityValue( KRadioCRUid, KRadioCRAudioRoute, aAudioRoute );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
// Retrieves the radio audio route in use.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::AudioRoute() const
    {
    return iRepositoryManager.EntityValueInt( KRadioCRUid, KRadioCRAudioRoute );
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetHeadsetVolMuted( TBool aMuted )
    {
    return iRepositoryManager.SetEntityValue( KRadioCRUid, KRadioCRHeadsetMuteState, aMuted );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineSettings::IsHeadsetVolMuted() const
    {
    return iRepositoryManager.EntityValueInt( KRadioCRUid, KRadioCRHeadsetMuteState );
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetSpeakerVolMuted( TBool aMuted )
    {
    return iRepositoryManager.SetEntityValue( KRadioCRUid, KRadioCRSpeakerMuteState, aMuted );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineSettings::IsSpeakerVolMuted() const
    {
    return iRepositoryManager.EntityValueInt( KRadioCRUid, KRadioCRSpeakerMuteState );
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetVolMuted( TBool aMuted )
    {
    if ( AudioRoute() == RadioEngine::ERadioHeadset )
        {
        return SetHeadsetVolMuted( aMuted );
        }
    else
        {
        return SetSpeakerVolMuted( aMuted );
        }
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineSettings::IsVolMuted() const
    {
    if ( AudioRoute() == RadioEngine::ERadioHeadset )
        {
        return IsHeadsetVolMuted();
        }
    else
        {
        return IsSpeakerVolMuted();
        }
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetPowerOn( TBool aPowerState )
    {
    return iRepositoryManager.SetEntityValue( KRadioCRUid, KRadioCRVisualRadioPowerState, aPowerState );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineSettings::IsPowerOn() const
    {
    return iRepositoryManager.EntityValueInt( KRadioCRUid, KRadioCRVisualRadioPowerState );
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetTunedFrequency( TUint32 aFrequency )
    {
    return iRepositoryManager.SetEntityValue( KRadioCRUid,
                    KRadioCRTunedFrequency, static_cast<TInt>( aFrequency ) );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TUint32 CRadioEngineSettings::TunedFrequency() const
    {
    return static_cast<TUint32>( iRepositoryManager.EntityValueInt( KRadioCRUid, KRadioCRTunedFrequency ) );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::DefaultMinVolumeLevel() const
    {
    return iRepositoryManager.EntityValueInt( KRadioCRUid, KRadioCRDefaultMinVolumeLevel );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::CountRegions() const
    {
    return iRegions.Count();
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
CRadioRegion& CRadioEngineSettings::Region( TInt aIndex ) const
    {
    if ( aIndex == KErrNotFound )
        {
        aIndex = RegionIndexForId( DefaultRegion() );
        }

    return *iRegions[aIndex];
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TRadioRegion CRadioEngineSettings::RegionId() const
    {
    return static_cast<TRadioRegion>( iRepositoryManager.EntityValueInt( KRadioCRUid,
                                                                         KRadioCRCurrentRegion ) );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TRadioRegion CRadioEngineSettings::DefaultRegion() const
    {
    TInt region( 0 );

    TRAPD( err, iRepositoryManager.GetRepositoryValueL( KRadioCRUid, KRadioCRDefaultRegion, region ) );

    TRadioRegion regionSetting = static_cast<TRadioRegion>( region );

    if ( err != KErrNone )
        {
        regionSetting = ERadioRegionNone;
        }
    return regionSetting;
    }


// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TPtrC CRadioEngineSettings::NetworkId() const
    {
    return TPtrC( iRepositoryManager.EntityValueDes16( KRadioCRUid, KRadioCRNetworkId ) );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TPtrC CRadioEngineSettings::SubscriberId() const
    {
    return TPtrC( iRepositoryManager.EntityValueDes16( KRadioCRUid, KRadioCRSubscriberId ) );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TPtrC CRadioEngineSettings::CountryCode() const
    {
    return TPtrC( iRepositoryManager.EntityValueDes16( KRadioCRUid, KRadioCRCountryCode ) );
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetRegionId( TInt aRegion )
    {
    __ASSERT_ALWAYS( RegionIndexForId( aRegion ) != KErrNotFound,
                     User::Panic( _L( "CRadioEngineSettings" ), KErrArgument ) );

    TInt err = iRepositoryManager.SetEntityValue( KRadioCRUid, KRadioCRCurrentRegion, aRegion );
    if ( !err )
        {
        UpdateCurrentRegionIdx( aRegion );
        err = SetTunedFrequency( Region( iCurrentRegionIdx ).MinFrequency() );
        }
    return err;
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetRdsAfSearch( TBool aEnabled )
    {
    return iRepositoryManager.SetEntityValue( KRadioCRUid, KRadioCRRdsAfSearch, aEnabled );
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetNetworkId( const TDesC& aNetworkId )
    {
    return iRepositoryManager.SetEntityValue( KRadioCRUid, KRadioCRNetworkId, aNetworkId );
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetSubscriberId( const TDesC& aSubscriberId )
    {
    return iRepositoryManager.SetEntityValue( KRadioCRUid, KRadioCRSubscriberId, aSubscriberId );
    }

// ---------------------------------------------------------------------------
// From class MRadioSettingsSetter.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::SetCountryCode( const TDesC& aCountryCode )
    {
    return iRepositoryManager.SetEntityValue( KRadioCRUid, KRadioCRCountryCode, aCountryCode );
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TUint32 CRadioEngineSettings::FrequencyStepSize() const
    {
    return Region( iCurrentRegionIdx ).StepSize();
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TUint32 CRadioEngineSettings::MaxFrequency() const
    {
    return Region( iCurrentRegionIdx ).MaxFrequency();
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TUint32 CRadioEngineSettings::MinFrequency() const
    {
    return Region( iCurrentRegionIdx ).MinFrequency();
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::DecimalCount() const
    {
    return Region( iCurrentRegionIdx ).DecimalCount();
    }

// ---------------------------------------------------------------------------
// From class MRadioEngineSettings.
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineSettings::RdsAfSearchEnabled() const
    {
    return iRepositoryManager.EntityValueInt( KRadioCRUid, KRadioCRRdsAfSearch );
    }

// ---------------------------------------------------------------------------
// Initializes the regions based on resources.
// ---------------------------------------------------------------------------
//
void CRadioEngineSettings::InitializeRegionsL()
    {
    iRegions.ResetAndDestroy();

    TResourceReader reader;
    iCoeEnv.CreateResourceReaderLC( reader, R_QRAD_REGIONS );

    TInt regionCount = reader.ReadInt16();

    for ( TInt i = 0 ; i < regionCount; i++ )
        {
        TInt resId = reader.ReadInt32(); // The next resource ID to read.
        TResourceReader regionReader;
        iCoeEnv.CreateResourceReaderLC( regionReader, resId );
        CRadioRegion* region = CRadioRegion::NewL( regionReader );
        CleanupStack::PushL( region );

        if ( IsRegionAllowed( region->Id() ))
            {
            User::LeaveIfError( iRegions.Append( region ) );
            CleanupStack::Pop( region );
            }
        else{
            CleanupStack::PopAndDestroy( region );
            }
        CleanupStack::PopAndDestroy();
        }

    CleanupStack::PopAndDestroy();

    if ( CountRegions() <= 0 || !IsRegionAllowed( DefaultRegion() ) )
        {
        User::Leave( KErrCorrupt );
        }

    UpdateCurrentRegionIdx( RegionId() );
    }

// ---------------------------------------------------------------------------
// Converts Region ID to index
// ---------------------------------------------------------------------------
//
TInt CRadioEngineSettings::RegionIndexForId( TInt aRegionId ) const
    {
    TInt idx = KErrNotFound;
    for ( TInt i = 0 ; i < CountRegions(); ++i )
        {
        if ( Region( i ).Id() == static_cast<TRadioRegion>( aRegionId ) )
            {
            idx = i;
            break;
            }
        }
    return idx;
    }

// ---------------------------------------------------------------------------
// Updates the current region
// ---------------------------------------------------------------------------
//
void CRadioEngineSettings::UpdateCurrentRegionIdx( TInt aRegionId )
    {
    iCurrentRegionIdx = RegionIndexForId( aRegionId );
    }

// ---------------------------------------------------------------------------
// From class MRadioRepositoryEntityObserver.
// ---------------------------------------------------------------------------
//
void CRadioEngineSettings::HandleRepositoryValueChangeL( const TUid& aUid,
                                                         TUint32 aKey,
                                                         TInt aValue,
                                                         TInt aError )
    {
    if ( !aError && aUid == KRadioCRUid && iObserver )
        {
        if ( aKey == KRadioCRRdsAfSearch )
            {
            iObserver->RdsAfSearchSettingChangedL( aValue );
            }
        else if ( aKey == KRadioCRCurrentRegion )
            {
            if ( iCurrentRegionIdx != RegionIndexForId( aValue ) )
                {
                UpdateCurrentRegionIdx( aValue );
                __ASSERT_ALWAYS( iCurrentRegionIdx != KErrNotFound,
                     User::Panic( _L( "CRadioEngineSettings" ), KErrArgument ) );
                User::LeaveIfError( SetTunedFrequency( Region( iCurrentRegionIdx ).MinFrequency() ) );
                iObserver->RegionSettingChangedL( aValue );
                }
            }
        else if ( aKey == KRadioCRRdsAfSearch )
            {
            iObserver->RdsAfSearchSettingChangedL( aValue );
            }
        else
            {
            }
        }
    }

// ---------------------------------------------------------------------------
// Determines if region is allowed .
// ---------------------------------------------------------------------------
//
TBool CRadioEngineSettings::IsRegionAllowed( TRadioRegion aRegionId ) const
    {

    TInt regionAllowed( EFalse );
    TInt err( KErrNone );

    switch ( aRegionId )
        {
        case ERadioRegionJapan:
            TRAP( err, iRepositoryManager.GetRepositoryValueL( KRadioCRUid, KRadioCRRegionAllowedJapan, regionAllowed ) );
            break;

        case ERadioRegionAmerica:
            TRAP( err, iRepositoryManager.GetRepositoryValueL( KRadioCRUid, KRadioCRRegionAllowedAmerica, regionAllowed ) );
            break;

        case ERadioRegionDefault:
            TRAP( err, iRepositoryManager.GetRepositoryValueL( KRadioCRUid, KRadioCRRegionAllowedDefault, regionAllowed ) );
            break;

        default:
            break;
        }

    if ( err != KErrNone )
        {
        regionAllowed = EFalse;
        }

    return TBool( regionAllowed );
    }
