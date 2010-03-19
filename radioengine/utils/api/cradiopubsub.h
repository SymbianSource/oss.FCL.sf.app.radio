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

#ifndef CRADIOPUBSUB_H
#define CRADIOPUBSUB_H

#include <e32base.h>

#include "radiointernalpskeys.h"

class MRadioControlEventObserver;

/**
 *  Publish&Subscribe interface of Visual Radio Application.
 */
NONSHARABLE_CLASS( CRadioPubSub ) : public CBase
    {
public:

    IMPORT_C static CRadioPubSub* NewL();

    IMPORT_C virtual ~CRadioPubSub();

    /**
     * Publishes active channel number to P&S.
     *
     * @param aChannelId The channel ID to publish.
     */
    virtual TBool PublishChannel( TInt aChannelId ) const = 0;

    /**
     * Publishes active frequency to P&S.
     *
     * @param aFreq The frequency to publish.
     */
    virtual TBool PublishFrequency( TUint32 aFreq ) const = 0;

    /**
     * Publishes radio power state to P&S.
     *
     * @param aPowerOn The radio power state to publish.
     */
    virtual TBool PublishPowerState( TBool aPowerOn ) const = 0;

    /**
     * Publishes current volume level to P&S.
     *
     * @param aVol The current volume level to publish.
     */
    virtual TBool PublishVolume( TInt aVol ) const = 0;

    /**
     * Publishes changed channel number to P&S.
     *
     * @param aChannelId The channel ID to publish.
     */
    virtual TBool PublishChannelDataChanged( TInt aChannelId ) const = 0;

    /**
     * Publishes the tuning state to P&S.
     * @param aTuningState The tuning state to publish.
     */
    virtual TBool PublishTuningState( TRadioPSTuningState aTuningState ) const = 0;

    /**
     * Publishes radio mute state to P&S.
     *
     * @param aMuted The radio mute state to publish.
     */
    virtual TBool PublishRadioMuteState( TBool aMuted ) const = 0;

    /**
     * Publishes application running state to P&S.
     *
     * @param aRunningState The application running state.
     */
    virtual TBool PublishApplicationRunningState( TRadioPSApplicationRunningState aRunningState ) const = 0;

    /**
     * Publishes the headset status to P&S.
     *
     * @param aHeadsetStatus The headset's status.
     */
    virtual TBool PublishHeadsetStatus( TRadioPSHeadsetStatus aHeadsetStatus ) const = 0;

    /**
     * Publishes the frequency decimal count to P&S.
     * @param aDecimalCount The decimal count.
     */
    virtual TBool PublishFrequencyDecimalCount( TRadioPSFrequencyDecimalCount aDecimalCount ) const = 0;

    /**
     * Publishes speaker status. Before state is set for first time it is in
     * <code>ERadioPSSpeakerUninitialized</code> state.
     *
     * @param aLoudspeakerStatus Status of the speaker
     */
    virtual TBool PublishLoudspeakerStatus( TRadioPSLoudspeakerStatus aLoudspeakerStatus ) const = 0;

    /**
     * Publishes Rds Program Service information.
     *
     * @param aProgramService Rds Program service descriptor
     */
    virtual TBool PublishRdsProgramService( const TDesC& aProgramService ) const = 0;

    /**
     * Publishes Rds Radio Text information.
     *
     * @param aRadioText The Radio Text information to be published.
     */
    virtual TBool PublishRdsRadioText( const TDesC& aRadioText ) const = 0;

    /**
     * Publishes Channel name information.
     *
     * @param aName The name to be published.
     */
    virtual TBool PublishChannelName( const TDesC& aName ) const = 0;

    /**
     * Publishes the number of presets in the active preset list.
     *
     * @param aCount The number of presets.
     */
    virtual TBool PublishActivePreseListCount( TInt aCount ) const = 0;

    /**
     * Publishes the focus in the active preset list.
     *
     * @param aIndex The focus in the active preset list.
     */
    virtual TBool PublishActivePreseListFocus( TInt aIndex ) const = 0;

    /**
     * Publishes the antenna state ( attach/detach ).
     *
     * @param aState The current antenna state.
     */
    virtual TBool PublishAntennaState( TRadioPSRadioAntennaState aState ) const = 0;

    /**
     * Sets the UI side control event observer.
     * @param aControlEventObserver the observer for control events
     */
    virtual void SetControlEventObserver( MRadioControlEventObserver* aControlEventObserver ) = 0;

    };

#endif // CRADIOPUBSUB_H
