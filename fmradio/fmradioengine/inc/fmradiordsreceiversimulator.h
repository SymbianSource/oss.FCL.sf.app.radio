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
* Description:  The RDS receiver simulator for FM Radio
*
*/


#ifndef CFMRADIORDSRECEIVERSIMULATOR_H
#define CFMRADIORDSRECEIVERSIMULATOR_H

#include "fmradioengineradiosettings.h"
#include "fmradiordsobserver.h"
#include "fmradiordsreceiverbase.h"

NONSHARABLE_CLASS( CFMRadioRdsReceiverSimulator ) : public CFMRadioRdsReceiverBase
    {
public:

    /**
     * The two-phased constructor
     *
     * @param aSettings The radio settings
     */
    static CFMRadioRdsReceiverSimulator* NewL( TRadioSettings& aSettings );

    /**
     * The destructor
     */
    virtual ~CFMRadioRdsReceiverSimulator();

    // from CFMRadioRdsReceiverBase
    void InitL( CRadioUtility& aRadioUtility, CFMRadioPubSub* aPubSub );
    void StartReceiver();
    void StopReceiver();
    
    /**
     * For simulate MrftoFrequencyChange method from MRadioFmTunerObserver
     */
    void SetRadioEngineForRadioFmTunerSimulation( CRadioEngine* aEngine );
private:

    /** The simulated events */
    enum TFMRadioRdsSimulationEvent
        {
        EFMRadioRdsEventSignalOn,
        EFMRadioRdsEventSignalOff,
        EFMRadioRdsEventPS,
        EFMRadioRdsEventRadioText,
        EFMRadioRdsEventAFBegin,
        EFMRadioRdsEventAFEnd,
        EFMRadioRdsEventRadioTextPlus
        };
        
    /**
     * The default constructor
     *
     * @param aSettings The radio settings
     */
    CFMRadioRdsReceiverSimulator( TRadioSettings& aSettings );

    /**
     * Second phase constructor
     */
    void ConstructL();

    /**
     * StaticRdsSimulationCallback Callback for RDS simulation
     */
    static TInt StaticRdsSimulationCallback( TAny* aSelfPtr );
    
private: // data
    
    /** Timer for RDS event simulation */
    CPeriodic* iEventSimulatorTimer;
    
    /** The iterator for RDS events */
    TInt iEventIterator;
    
    /** The iterator for PS names */
    TInt iPsIterator;
    
    /** The iterator for radio texts */
    TInt iRadioTextIterator;
    
    /** The iterator for frequencies */
    TInt iFrequencyIterator;

    /** The iterator for radio text plus'*/
    TInt iRadioTextPlusIterator;
    
    /** Pointer to the engine for simulate MrftoFrequencyChange method from MRadioFmTunerObserver */
    CRadioEngine* iEngine;
    };

#endif // CFMRADIORDSRECEIVERSIMULATOR_H
