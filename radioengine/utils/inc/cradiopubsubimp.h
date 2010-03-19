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
#ifndef CRADIOPUBSUBIMP_H
#define CRADIOPUBSUBIMP_H

// User includes
#include "cradiopubsub.h"
#include "cradiopropertyobserver.h"

/**
 *  Publish & Subscribe interface of Radio Application.
 */
NONSHARABLE_CLASS( CRadioPubSubImp ) : public CRadioPubSub
                                     , public MRadioPropertyChangeObserver
    {
public:

    static CRadioPubSubImp* NewL();

    ~CRadioPubSubImp();

private:

    CRadioPubSubImp();

    void ConstructL();

// from base class CRadioPubSub

    TBool PublishChannel( TInt aChannelId ) const;
    TBool PublishFrequency( TUint32 aFreq ) const;
    TBool PublishPowerState( TBool aPowerOn ) const;
    TBool PublishVolume( TInt aVol ) const;
    TBool PublishChannelDataChanged( TInt aChannelId ) const;
    TBool PublishTuningState( TRadioPSTuningState aTuningState ) const;
    TBool PublishRadioMuteState( TBool aMuted ) const;
    TBool PublishApplicationRunningState( TRadioPSApplicationRunningState aRunningState ) const;
    TBool PublishHeadsetStatus( TRadioPSHeadsetStatus aHeadsetStatus ) const;
    TBool PublishFrequencyDecimalCount( TRadioPSFrequencyDecimalCount aDecimalCount ) const;
    TBool PublishLoudspeakerStatus( TRadioPSLoudspeakerStatus aLoudspeakerStatus ) const;
    TBool PublishRdsProgramService( const TDesC& aProgramService ) const;
    TBool PublishRdsRadioText( const TDesC& aRadioText ) const;
    TBool PublishChannelName( const TDesC& aName ) const;
    TBool PublishActivePreseListCount( TInt aCount ) const;
    TBool PublishActivePreseListFocus( TInt aIndex ) const;
    TBool PublishAntennaState( TRadioPSRadioAntennaState aState ) const;
    void SetControlEventObserver( MRadioControlEventObserver* aControlEventObserver );

// from base class MVRPropertyChangeObserver

    void HandlePropertyChangeL( const TUid& aCategory, const TUint aKey, const TInt aValue );
    void HandlePropertyChangeL( const TUid& /*aCategory*/, const TUint /*aKey*/, const TDesC8& /*aValue*/ ) {}
    void HandlePropertyChangeL( const TUid& /*aCategory*/, const TUint /*aKey*/, const TDesC& /*aValue*/ ) {}
    void HandlePropertyChangeErrorL( const TUid& /*aCategory*/, const TUint /*aKey*/, TInt /*aError*/ ) {}

// New functions

    /**
     * Defines a property.
     *
     * @param aKey The key of the property.
     * @param aAttr Attributes of the property.
     * @param aDataProperty ETrue if the property is a data property, EFalse
     * if it is a control property
     */
    void DefinePropertyL( TUint aKey, TInt aAttr, TBool aDataProperty ) const;

    /**
     * Deletes a property.
     *
     * @param aKey The key of the property.
     */
    void DeleteProperty( TUint aKey ) const;

    /**
     * Creates a property observer
     *
     * @param aKey Property key
     * @param aPropertyType Property type
     */
    void CreatePropertyObserverL( const TUint aKey, const TInt aPropertyType );

    /**
     * Gets the integer value stored in a property.
     *
     * @param    aKey    The key of the property.
     * @return   The value contained in the property.
     */
    TBool Get( TUint aKey, TInt& aValue ) const;

    /**
     * Sets the value in an integer property.
     *
     * @param    aKey    The key of the property.
     * @param    aValue  The value to set.
     */
    TBool Set( TUint aKey, TInt aValue ) const;

    /**
     * Sets the value in a text property.
     *
     * @param    aKey    The key of the property.
     * @param    aValue  The value to set.
     */
    TBool Set( TUint aKey, const TDesC& aValue ) const;

private: // data

    /**
     * Pointer to observer in UI side, can be NULL. Not owned.
     */
    MRadioControlEventObserver*           iObserver;

    /**
     * An array of handlers to P&S interface.
     */
    RPointerArray<CRadioPropertyObserver> iPropertyArray;

    };

#endif // CRADIOPUBSUBIMP_H
