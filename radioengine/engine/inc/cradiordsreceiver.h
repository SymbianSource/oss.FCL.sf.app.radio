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

#ifndef CRADIORDSRECEIVER_H
#define CRADIORDSRECEIVER_H

#include "cradiordsreceiverbase.h"

class MRadioEngineSettings;

NONSHARABLE_CLASS( CRadioRdsReceiver ) : public CRadioRdsReceiverBase
    {
public:

    static CRadioRdsReceiver* NewL( MRadioEngineSettings& aSettings );

    virtual ~CRadioRdsReceiver();

private:

    CRadioRdsReceiver( MRadioEngineSettings& aSettings );

    void ConstructL();

// from base class MRadioRdsReceiver

    void InitL( CRadioUtility& aRadioUtility, CRadioPubSub* aPubSub );
    void SetAutomaticSwitchingL( TBool aEnable );
    void StartReceiver();
    void StopReceiver();

// New functions

    /**
     * Logs the RDS receiver capabilities if logging is enabled
     */
    void LogReceiverCapabilities();

private: // data

    /** The RDS utility */
    CRadioRdsUtility*   iRdsUtility;

    /** State of the RDS receiver */
    TBool               iStarted;

    };

#endif // CRADIORDSRECEIVER_H
