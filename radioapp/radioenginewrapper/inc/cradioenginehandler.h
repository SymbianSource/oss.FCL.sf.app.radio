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

#ifndef _CRADIOENGINEHANDLER_H_
#define _CRADIOENGINEHANDLER_H_

// System includes
#include <e32base.h>

#include <mw/ProfileEngineSDKCRKeys.h>

// User includes
#include "radio_global.h"
#include "mradioengineinitializer.h"

// Forward declarations
class CRadioEngine;
class MRadioScanObserver;
class MRadioRdsDataObserver;
class CRadioRepositoryManager;
class MRadioEngineHandlerObserver;
class MRadioApplicationSettings;

// Class declaration
class CRadioEngineHandler : public CBase
                          , public MRadioEngineInitializer
    {
public:

    CRadioEngineHandler( MRadioEngineHandlerObserver& aObserver );

    ~CRadioEngineHandler();

    void ConstructL();

    /**
     * Sets the rds data observer
     */
    void SetRdsObserver( MRadioRdsDataObserver* aObserver );

    /**
     * Returns the radio status
     * @return ETrue = radio is on, EFalse = radio is off
     */
    TBool IsRadioOn();

    /**
     * Tune to the specified frequency
     * @param aFrequency - frequency to lock onto.
     */
    void Tune( TUint aFrequency );

    /**
     * Tune to the specified frequency after a delay
     * @param aFrequency - frequency to lock onto.
     */
    void TuneWithDelay( TUint aFrequency );

    /**
     * Sets the audio mute state
     * @param aMuted - flag to determine whether mute should be turned on or off
     */
    void SetMuted( const TBool aMuted );

    /**
     * Gets the audio mute state
     * @return ETrue or EFalse to indicate whether mute is currently on.
     */
    TBool IsMuted() const;

    /**
     * Sets the volume level of the FM radio
     * @param aVolume - the volume to be used.
     */
    void SetVolume( TInt aVolume );

    /**
     * Gets the volumelevel.
     * @return the current volume
     */
    TInt Volume() const;

    /**
     * Gets the max volumelevel.
     * @return the max volume
     */
    TInt MaxVolume() const;

    /**
     * Increases the volume by one increment
     */
    void IncreaseVolume();

    /**
     * Decreases the volume by one increment
     */
    void DecreaseVolume();

    /**
     * Checks if the antenna is attached
     * @return ETrue or EFalse to indicate whether antenna is currently attached.
     */
    TBool IsAntennaAttached() const;

    /**
     * Retrieves the current frequency.
     * @return the frequency in hertz
     */
    TUint TunedFrequency() const;

    /**
     * Returns the minimum allowed frequency in the current region
     */
    TUint MinFrequency() const;

    /**
     * Returns the maximum allowed frequency in the current region
     */
    TUint MaxFrequency() const;

    /**
     * Checks if the given frequency is valid in the current region
     */
    TBool IsFrequencyValid( TUint aFrequency ) const;

    /**
     * Scan up to the next available frequency.
     */
    void Seek( Seeking::Direction direction );

    /**
     * Cancel previously requested scan.
     */
    void CancelSeek();

    /**
     * Returns the engine seeking state
     */
    Seeking::State SeekingState() const;

    void StartScan( MRadioScanObserver& aObserver );

    void StopScan( TInt aError = KErrCancel );

    /**
     * Rreturn step size for tuning.
     * @return step size
     */
    TUint32 FrequencyStepSize() const;

    /**
     * Determine current region
     * @return fmradio region. In case of error, returns EFMRadioRegionNone.
     */
    RadioRegion::Region Region() const;

    /**
     * Sets whether or not audio should be routed to loudspeaker
     * @param aLoudspeaker ETrue if loudspeaker should be used, EFalse if not
     */
    void SetAudioRouteToLoudspeaker( TBool aLoudspeaker );

    /**
     * Checks if audio is routed to loudspeaker
     * @return ETrue if loudspeaker is used, EFalse if headset is used
     */
    TBool IsAudioRoutedToLoudspeaker() const;

    /**
     * Returns ar reference to the publish & subscribe handler
     */
    CRadioPubSub& PubSub();

    /**
     * Returns the repository manager.
     *
     * @return  The repository manager.
     */
    CRadioRepositoryManager& Repository() const;

    MRadioApplicationSettings& ApplicationSettings() const;

private:

// from base class MRadioEngineInitializer

    CRadioAudioRouter* InitAudioRouterL();
    CRadioSystemEventCollector* InitSystemEventCollectorL();
    CRadioSettings* InitSettingsL();
    CRadioPubSub* InitPubSubL();

// New functions

    /**
     * Static callback function to be used by the tune delay timer
     * @param aSelf Pointer to this
     * @return not used
     */
    static TInt TuneDelayCallback( TAny* aSelf );

private: // data

    /**
     * Radio engine holder
     * Own.
     */
    CRadioEngine*                   iEngine;

    /**
     * Radio handler observer
     * Not own.
     */
    MRadioEngineHandlerObserver&    iObserver;

    /**
     * Frequency used by delayed tuning
     */
    TUint                           iFrequency;

    /**
     * Timer used for delayed tuning
     * Own.
     */
    CPeriodic*                      iDelayTimer;

    /**
     * Selected radio region
     */
    RadioRegion::Region             iRegion;

    };

#endif //_CRADIOENGINEHANDLER_H_
