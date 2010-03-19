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

#ifndef C_RADIOENGINESETTINGS_H
#define C_RADIOENGINESETTINGS_H

#include "mradioenginesettings.h"
#include "mradiosettingssetter.h"
#include "cradioregion.h"
#include "mradiorepositoryentityobserver.h"
#include "cradiosettingsbase.h"

class MRadioSettingsObserver;

/**
 * Concrete implementation of radio settings.
 */
NONSHARABLE_CLASS( CRadioEngineSettings ) : public CRadioSettingsBase
                                          , public MRadioSettingsSetter
                                          , public MRadioRepositoryEntityObserver
    {

public:

    static CRadioEngineSettings* NewL( CRadioRepositoryManager& aRepositoryManager, CCoeEnv& aCoeEnv );

    ~CRadioEngineSettings();

    /**
     * Determines if region is allowed.
     */
    TBool IsRegionAllowed( TRadioRegion aRegionId ) const;

private:

    CRadioEngineSettings( CRadioRepositoryManager& aRepositoryManager, CCoeEnv& aCoeEnv );

    void ConstructL();

// from base class MRadioEngineSettings

    TInt HeadsetVolume() const;
    TInt SpeakerVolume() const;
    TInt Volume() const;
    TInt OutputMode() const;
    TInt AudioRoute() const;
    TBool IsHeadsetVolMuted() const;
    TBool IsSpeakerVolMuted() const;
    TBool IsVolMuted() const;
    TBool IsPowerOn() const;
    TUint32 TunedFrequency() const;
    TInt DefaultMinVolumeLevel() const;
    TInt CountRegions() const;
    CRadioRegion& Region( TInt aIndex ) const;
    TRadioRegion RegionId() const;
    TUint32 FrequencyStepSize() const;
    TUint32 MaxFrequency() const;
    TUint32 MinFrequency() const;
    TInt DecimalCount() const;
    TBool RdsAfSearchEnabled() const;
    TRadioRegion DefaultRegion() const;
    TPtrC NetworkId() const;
    TPtrC SubscriberId() const;
    TPtrC CountryCode() const;

// from base class MRadioSettingsSetter

    void SetObserver( MRadioSettingsObserver* aObserver );
    TInt SetHeadsetVolume( TInt aVolume );
    TInt SetSpeakerVolume( TInt aVolume );
    TInt SetVolume( TInt aVolume );
    TInt SetOutputMode( TInt aOutputMode );
    TInt SetAudioRoute( TInt aAudioRoute );
    TInt SetHeadsetVolMuted( TBool aMuted );
    TInt SetSpeakerVolMuted( TBool aMuted );
    TInt SetVolMuted( TBool aMuted );
    TInt SetPowerOn( TBool aPowerState );
    TInt SetTunedFrequency( TUint32 aFrequency );
    TInt SetRegionId( TInt aRegion );
    TInt SetRdsAfSearch( TBool aEnabled );
    TInt SetNetworkId( const TDesC& aNetworkId );
    TInt SetSubscriberId( const TDesC& aSubscriberId );
    TInt SetCountryCode( const TDesC& aCountryCode );

// from base class MRadioRepositoryEntityObserver

    void HandleRepositoryValueChangeL( const TUid& aUid, TUint32 aKey, TInt aValue, TInt aError );
    void HandleRepositoryValueChangeL( const TUid& /*aUid*/, TUint32 /*aKey*/, const TReal& /*aValue*/, TInt /*aError*/ ) {}
    void HandleRepositoryValueChangeL( const TUid& /*aUid*/, TUint32 /*aKey*/, const TDesC8& /*aValue*/, TInt /*aError*/ ) {}
    void HandleRepositoryValueChangeL( const TUid& /*aUid*/, TUint32 /*aKey*/, const TDesC16& /*aValue*/, TInt /*aError*/ ) {}

// New functions

    /**
     * Initializes the regions as defined in the resource file.
     */
    void InitializeRegionsL();

    /**
     * Converts Region ID to index
     *
     * @param aRegionId The region ID that is wanted to convert
     * @return The region index, KErrNotFound if no match found
     */
    TInt RegionIndexForId( TInt aRegionId ) const;

    /**
     * Updates the current region index
     *
     * @param aRegionId The new region ID
     */
     void UpdateCurrentRegionIdx( TInt aRegionId );

private: // data

    /**
     * The observer for the setting changes.
     * Not own.
     */
    MRadioSettingsObserver* iObserver;

    /**
     * The available regions.
     */
    RRadioRegionArray       iRegions;

    /**
     * The current region.
     */
    TInt                    iCurrentRegionIdx;

    };

#endif // C_RADIOENGINESETTINGS_H
