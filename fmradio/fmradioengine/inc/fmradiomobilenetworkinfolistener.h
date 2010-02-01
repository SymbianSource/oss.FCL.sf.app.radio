/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for CFMRadioMobileNetworkInfoListener.
*
*/


#ifndef FMRADIOMOBILENETWORKINFOLISTENER_H
#define FMRADIOMOBILENETWORKINFOLISTENER_H

#include <e32base.h>
#include <etelmm.h>


const TInt KFMRadioCREmulatorCountryCodeLength = 3;
namespace CommsDat { class CCDIAPRecord; }

class MFMRadioNetworkChangeObserver;
/**
 * Listens changes in network ID and country code.
 *
 * @ingroup Utils
 */
NONSHARABLE_CLASS(CFMRadioMobileNetworkInfoListener) : public CActive
    {
public:

    /**
     * Creates a new mobile network info listener.
     * @return   The newly created mobile network info listener.
     */
    static CFMRadioMobileNetworkInfoListener* NewL(
                                    MFMRadioNetworkChangeObserver & aObserver);

    /**
     * Destructor.
     */
    ~CFMRadioMobileNetworkInfoListener();

    /**
     * Return current combined network ID and country code as TInt.
     * @return   An integer composed of the current network ID and country code.
     */
    TInt CompoundNetworkId() const;

	/**
     * Returns current country code.
     * @return   The current country code.
     */
    const TDesC& CountryCode() const;
    
    /**
     * SubscriberId
     * @return subscriber id
     */
    const TDesC& SubscriberId() const;
    
private:

	/**
     * Constructor.
     */
    CFMRadioMobileNetworkInfoListener(MFMRadioNetworkChangeObserver & aObserver);
    
    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();
    
    /**
     * Saves fetched network info to local variables.
     */
    void SaveNetworkInfo();

#ifdef __WINS__
    /**
     * Gets network info from Central Repository.
     */
    void GetCenRepNetworkInfoL();
#endif

// from base class CActive
    void RunL();
    void DoCancel();

private:    // Data

    /** interface to report a condition when network id has changed */
    MFMRadioNetworkChangeObserver & iNetworkChangeObserver;
#ifdef __WINS__
    /** Central repository */
    CRepository* iRepository;
    /** Network info from repository */
    HBufC* iEmulatorNetworkInfo;
#else
    /** Telephony server. */
    RTelServer iTelServer;
    /** Mobile phone. */
    RMobilePhone iPhone;
    /** Network info of mobile phone. */
    RMobilePhone::TMobilePhoneNetworkInfoV1 iNetworkInfo;
    /** Location of mobile phone. */
    RMobilePhone::TMobilePhoneLocationAreaV1 iLocation;
    /** Packaged network info object. */
    RMobilePhone::TMobilePhoneNetworkInfoV1Pckg iNetworkInfoPckg;
#endif		
    /** Current network ID. */
    RMobilePhone::TMobilePhoneNetworkIdentity iNetworkId;
    /** Current country code. */
    RMobilePhone::TMobilePhoneNetworkCountryCode iCountryCode;
    /** Combined network ID and country code. */
    TInt iCompoundNetworkId;
    /** Previous known non-null combined country code */
    TInt iPreviousCompoundNetworkId;
    /** Subsriber (IMSI code) id that has been picked up at the startup */
    HBufC* iSubscriberId;
    };

#endif // FMRADIOMOBILENETWORKINFOLISTENER_H   
