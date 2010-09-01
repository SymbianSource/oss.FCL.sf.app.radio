/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The RDS receiver implementation for FM Radio
*
*/


#ifndef CFMRADIORDSRECEIVER_H
#define CFMRADIORDSRECEIVER_H

#include "fmradioengineradiosettings.h"
#include "fmradiordsobserver.h"
#include "fmradiordsreceiverbase.h"

NONSHARABLE_CLASS( CFMRadioRdsReceiver ) : public CFMRadioRdsReceiverBase
    {
public:

    /**
     * The two-phased constructor
     *
     * @param aSettings The radio settings
     */
    static CFMRadioRdsReceiver* NewL( TRadioSettings& aSettings );

    /**
     * The destructor
     */
    virtual ~CFMRadioRdsReceiver();

    // from CFMRadioRdsReceiverBase
    void InitL( CRadioUtility& aRadioUtility, CFMRadioPubSub* aPubSub );
    void SetAutomaticSwitchingL( TBool aEnable );
    void StartReceiver();
    void StopReceiver();

private:

    /**
     * The default constructor
     *
     * @param aSettings The radio settings
     */
    CFMRadioRdsReceiver( TRadioSettings& aSettings );

    /**
     * Second phase constructor
     */
    void ConstructL();

private: // data
    
    /** The RDS utility */
    CRadioRdsUtility* iRdsUtility;

    /** State of the RDS receiver */
    TBool iStarted;
    };

#endif // CFMRADIORDSRECEIVER_H
