/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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

#ifndef CRADIOACCESSORYOBSERVERSTUB_H
#define CRADIOACCESSORYOBSERVERSTUB_H

// System includes
#include <AccessoryServer.h>
#include <AccessoryConnection.h>
#include <AccPolSubblockNameArray.h>
#include <RadioStubManager.h>

// User includes
#include "cradiopropertyobserver.h"

class MRadioHeadsetEventObserver;

/**
 *  Observer of accessory states.
 *
 *  Instance of this class observes notifications of headset state changes.
 *  In its turn it sends notifications to its observer of those changes.
 *
 */
NONSHARABLE_CLASS( CRadioAccessoryObserver ): public CBase
        , private MRadioPropertyChangeObserver
{
public:

    /**
     * C++ default constructor.
     */
    CRadioAccessoryObserver();

    /**
     * Static constructor.
     */
    static CRadioAccessoryObserver* NewL();

    /**
     * Destructor.
     */
    virtual ~CRadioAccessoryObserver();

    /**
     * Sets observer. The observer will be notified when headset has been
     * pressed or headset is connected/disconnected.
      *
     * @param aObserver Observer
     */
    void SetObserver( MRadioHeadsetEventObserver* aObserver );

    /**
     * Tells if there is a headset connected.
     *
     * @return <code>ETrue </code> if headset accessory is connected,
     * <code>EFalse</code> otherwise
     */
    TBool IsHeadsetConnectedL() const;

protected:

// from base class MRadioPropertyChangeObserver

    void HandlePropertyChangeL( const TUid& aCategory, const TUint aKey, const TInt aValue );
    void HandlePropertyChangeL( const TUid& aCategory, const TUint aKey, const TDesC8& aValue );
    void HandlePropertyChangeL( const TUid& aCategory, const TUint aKey, const TDesC& aValue );
    void HandlePropertyChangeErrorL( const TUid& aCategory, const TUint aKey, TInt aError );

private:

    /**
     * By default Symbian 2nd phase constructor is private.
     *
     * @param aAccessoryServer Existing session for The Accessory Server.
     */
    void ConstructL();


private: // data

    // RadioStubManager pointer points inside RadioStubManagerChunk
    SRadioStubManager* iRadioStubManager;
    
    // RadioStubManagerChunk handle
    RChunk iRadioStubManagerChunk;

    /** handler to Publish & Subscribe interface*/
    CRadioPropertyObserver*     iHeadsetObserver;

};

#endif // CRADIOACCESSORYOBSERVERSTUB_H
