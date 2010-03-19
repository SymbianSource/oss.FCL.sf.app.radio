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

#ifndef T_CRADIORDSRECEIVERSIMULATOR_H
#define T_CRADIORDSRECEIVERSIMULATOR_H

#include "mradiordsdataobserver.h"
#include "cradiordsreceiverbase.h"

NONSHARABLE_CLASS( CRadioRdsReceiverSimulator ) : public CRadioRdsReceiverBase
    {
public:

    static CRadioRdsReceiverSimulator* NewL( MRadioEngineSettings& aSettings );

    virtual ~CRadioRdsReceiverSimulator();

// from base class MRadioRdsReceiver

    void InitL( CRadioUtility& aRadioUtility, CRadioPubSub* aPubSub );
    void StartReceiver();
    void StopReceiver();

private:

    /** The simulated events */
    enum TRadioRdsSimulationEvent
        {
        ERadioRdsEventSignalOn,
        ERadioRdsEventSignalOff,
        ERadioRdsEventPS,
        ERadioRdsEventAFBegin,
        ERadioRdsEventAFEnd,
        ERadioRdsEventRadioText,
        ERadioRdsEventRadioTextPlus
        };

    CRadioRdsReceiverSimulator( MRadioEngineSettings& aSettings );

    void ConstructL();

    void ParseRdsDataL();

    void startRTPlusTimer();
	
    /**
     * StaticRdsSimulationCallback Callback for RDS simulation
     */
    static TInt StaticRdsSimulationCallback( TAny* aSelfPtr );
	
    /**
     * Callback for RT+ simulation
     */
    static TInt StaticRdsSimulationCallback2( TAny* aSelfPtr );
    
private: // data

    /** Timer for RDS event simulation */
    CPeriodic* iEventSimulatorTimer;
    
    /** Timer for RT+ events */
    CPeriodic* iEventSimulatorTimer2;

    /** The iterator for RDS events */
    TInt iEventIterator;

    /** The iterator for RT+ events */
    TInt iRtPlusIterator;
    
    /** The iterator for RT events */
    TInt iRtIterator;
    
    /** The iterator for PS names */
    TInt iPsIterator;

    /** The iterator for frequencies */
    TInt iFrequencyIterator;

    /** The iterator for RT and RT+ information */
    TInt iRadioTextIterator;

    };

#endif // T_CRADIORDSRECEIVERSIMULATOR_H
