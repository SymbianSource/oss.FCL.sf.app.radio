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
* Description:  Header file for CFMRadioPubSub
*
*/


#ifndef CFMRADIOPUBSUB_H
#define CFMRADIOPUBSUB_H

#include <fmradiointernalpskeys.h>

#include "fmradiopropertyobserver.h"

class MFMRadioControlEventObserver;

/**
 *  Publish&Subscribe interface of FMRadioEngine
 */
class CFMRadioPubSub : 	
    public CBase,
	public MFMRadioPropertyChangeObserver
    {
    
public:  // Methods

    /**
     * Static constructor.
     */
    static CFMRadioPubSub* NewL();
    
    /**
     * Destructor.
     */
    virtual ~CFMRadioPubSub();

    /**
     * Publishes active channel number to P&S.
     *
     * @param aChannelId The channel ID to publish.
     */
    void PublishChannelL( TInt aChannelId ) const;
    
    /**
     * Publishes active frequency to P&S.
     *
     * @param aFreq The frequency to publish.
     */
    IMPORT_C void PublishFrequencyL( TUint32 aFreq ) const;

	/**
     * Publishes radio power state to P&S.
     *
     * @param aPowerOn The radio power state to publish.
     */
	IMPORT_C void PublishStateL( TBool aPowerOn ) const;

	/**
     * Publishes current volume level to P&S.
     *
     * @param aVol The current volume level to publish.
     */
	void PublishVolumeL( TInt aVol ) const;

	/**
     * Publishes changed channel number to P&S.
     *
     * @param aChannelId The channel ID to publish.
     */
	void PublishChannelDataChangedL(TInt aChannelId) const;

	/**
     * Publishes the tuning state to P&S.
     * @param aTuningState The tuning state to publish.
     */
	void PublishTuningStateL( TFMRadioPSTuningState aTuningState ) const;

	/**
     * Publishes radio mute state to P&S.
     *
     * @param aMuted The radio mute state to publish.
     */
	void PublishRadioMuteStateL( TBool aMuted ) const;

    /**
     * Publishes application running state to P&S.
     *
     * @param aRunningState The application running state.
     */
    IMPORT_C void PublishApplicationRunningStateL( 
        TFMRadioPSApplicationRunningState aRunningState ) const;

    /**
     * Publishes the antenna status to P&S.
     *
     * @param aAntennaStatus The antenna's status.
     */
    void PublishAntennaStatusL( TFMRadioPSHeadsetStatus aAntennaStatus ) const;
    
    /**
     * Publishes the frequency decimal count to P&S.
     * @param aDecimalCount The decimal count.
     */
    void PublishFrequencyDecimalCountL( 
        TFMRadioPSFrequencyDecimalCount aDecimalCount ) const;

    /**
     * Publishes speaker status. Before state is set for first time it is in
     * <code>EFMRadioPSLoudspeakerUninitialized</code> state.
     *
     * @param aLoudspeakerStatus Status of the speaker
     */
    void PublishLoudspeakerStatusL( TFMRadioPSLoudspeakerStatus aLoudspeakerStatus ) const;

    /**
     * Publishes RDS Program Service information.
     *
     * @param aProgramService Service text
     */
    IMPORT_C void PublishRDSProgramServiceL( const TDesC& aProgramService ) const;
    IMPORT_C void PublishRDSRadioTextL( const TDesC& aRadioText ) const;
    IMPORT_C void PublishChannelNameL( const TDesC& aName ) const;

    IMPORT_C void PublishPresetCountL( TInt aVal ) const;
    
// from base class MFMRadioPropertyChangeObserver
    void HandlePropertyChangeL( const TUid& aCategory, const TUint aKey, const TInt aValue );
    void HandlePropertyChangeL( const TUid& aCategory, const TUint aKey, const TDesC8& aValue );
    void HandlePropertyChangeL( const TUid& aCategory, const TUint aKey, const TDesC& aValue );
    void HandlePropertyChangeErrorL( const TUid& aCategory, const TUint aKey, TInt aError );
    
	/**
	 * Sets the UI side control event observer.
	 * @param aControlEventObserver the observer for control events
	 */
	IMPORT_C void SetControlEventObserver( MFMRadioControlEventObserver* aControlEventObserver );

private: //Methods

	/**
     * C++ default constructor.
     */
    CFMRadioPubSub();
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Defines a property.
     *
     * @param aKey The key of the property.
     * @param aAttr Attributes of the property.
     * @param aDataProperty ETrue if the property is a data property, EFalse 
     * if it is a control property
     */
    void DefinePropertyL( 
        TUint aKey, 
        TInt aAttr, 
        TBool aDataProperty ) const;

    /**
     * Deletes a property.
     *
     * @param aKey The key of the property.
     */
    void DeleteProperty( TUint aKey ) const;        

    /**
     * Gets the integer value stored in a property.
     *
     * @param    aKey    The key of the property.
     * @return   The value contained in the property.
     */
    TInt PropertyValueIntL( TUint aKey ) const;
    
    /**
     * Sets the value in an integer property.
     *
     * @param    aKey    The key of the property.
     * @param    aValue  The value to set.
     */
    void SetPropertyValueL( TUint aKey, TInt aValue ) const;

    /**
     * Sets the value in a text property.
     *
     * @param    aKey    The key of the property.
     * @param    aValue  The value to set.
     */
    void SetPropertyValueL( TUint aKey, const TDesC& aValue ) const;

private:    // Data

    /** 
     * Pointer to observer in UI side, can be NULL. Not owned. 
     */
	MFMRadioControlEventObserver*           iObserver;

	/** 
	 * An array of handlers to P&S interface. 
	 */ 
	RPointerArray<CFMRadioPropertyObserver> iPropertyArray;

    };

#endif      // CFMRADIOPUBSUB_H   
