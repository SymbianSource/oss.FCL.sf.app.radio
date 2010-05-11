/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer of accessory connection.
*
*/

#ifndef CFMRADIOACCESSORYCONNECTION_H
#define CFMRADIOACCESSORYCONNECTION_H

class TAccPolGenericIDArray;
class MFMRadioHeadsetEventObserver;

#include <e32base.h> // For CActive, link against: euser.lib
#include <AccessoryServer.h> //For RAccessoryServer session
#include <AccessoryConnection.h> //For RAccessoryConnection subsession

NONSHARABLE_CLASS( CFMRadioAccessoryConnection ) : public CActive
    {
public:
    // Cancel and destroy
    ~CFMRadioAccessoryConnection();
    
    // Two-phased constructor.
    static CFMRadioAccessoryConnection* NewL();
    /**
     * Sets observer. The observer will be notified when headset has been 
     * pressed or headset is connected/disconnected.
     *
     * @param aObserver Observer
     */
    void SetObserver( MFMRadioHeadsetEventObserver* aObserver );
    
    /**
     * Public boolean getter, return ETrue if 
     * a wired headset is connected
     */
    TBool WiredHeadsetConnected();
private:
    /**
     * C++ default constructor.
     */
    CFMRadioAccessoryConnection();
    
    /**
     * Static constructor.
     */
    void ConstructL();
    
    /**
     * Private boolean getter, return ETrue if 
     * a wired headset is found in array of connected accessories
     */
    TBool WiredHeadsetInArrayL();
    
    /**
     * Private boolean getter, return ETrue if 
     * a wired headset is found in first index of
     * array of connected accessories.
     */
    TBool WiredHeadsetFirstInArrayL();
    
    /**
     * From CActive
     *
     * @see CActive::RunL()
     */
    void RunL();
    
    /**
     * From CActive
     *
     * @see CActive::DoCancel()
     */        
    void DoCancel();
    
private:
    /** 
     * Instance that gets notifications of the headset state changes 
     */
    MFMRadioHeadsetEventObserver* iObserver;
    TBool iWiredHeadsetConnected;
    TInt iAccessoryCount;
    RAccessoryServer iAccessoryServer;
    RAccessoryConnection iAccessoryConnection;
    TAccPolGenericIDArray iAccessoryArray;
    };
#endif // CFMRADIOACCESSORYCONNECTION_H

