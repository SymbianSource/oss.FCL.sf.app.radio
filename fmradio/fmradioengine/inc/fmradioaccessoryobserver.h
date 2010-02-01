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
* Description:  Observer of accessory states.
*
*/


#ifndef CFMRADIOACCESSORYOBSERVER_H
#define CFMRADIOACCESSORYOBSERVER_H

#include <AccessoryServer.h>
#include <AccessoryMode.h>
#include <AccPolAccessoryMode.h>

class MFMRadioHeadsetEventObserver;

/**
 *  Observer of accessory states.  
 *
 *  Instance of this class observes notifications of headset state changes. 
 *  In its turn it sends notifications to its observer of those changes.
 *
 */
class CFMRadioAccessoryObserver : public CActive
    {
public:  

    /**
     * C++ default constructor.
     */
    CFMRadioAccessoryObserver();

    /**
     * Static constructor.
     */
    static CFMRadioAccessoryObserver* NewL();
    
    /**
     * Destructor.
     */
    virtual ~CFMRadioAccessoryObserver();

    /**
     * Sets observer. The observer will be notified when headset has been 
     * pressed or headset is connected/disconnected.
      *
     * @param aObserver Observer
     */
    void SetObserver( MFMRadioHeadsetEventObserver* aObserver );

    /**
     * Returns accessory connection state.
     *
     * @return <code>ETrue </code> if accessory is connected, 
     * <code>EFalse</code> otherwise
     */
    TBool IsHeadsetAccessoryConnected() const;
    
    // from base class CActive
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
    
protected:  

private: 

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

public:

protected:

    /** 
     * Instance that gets notifications of the headset state changes 
     */
    MFMRadioHeadsetEventObserver* iObserver;

private:    // Data

    /** 
     * Session for The Accessory Server. 
     */
    RAccessoryServer iAccessoryServer;
    
    /** 
     * Accessory Mode sub-session 
     */            
    RAccessoryMode          iAccessoryModeSession;
    
    
    /** 
     * Accessory Mode structure, with active audio information. 
     */
    TAccPolAccessoryMode    iAccPolAccessoryMode;
    TAccPolAccessoryMode    iPrevAccMode;
    };

#endif      // CFMRADIOACCESSORYOBSERVER_H
            

