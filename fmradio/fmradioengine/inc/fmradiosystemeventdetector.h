/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for CFMRadioSystemEventDetector.
*
*/


#ifndef CFMRADIOSYSTEMEVENTDETECTOR_H
#define CFMRADIOSYSTEMEVENTDETECTOR_H

#include <e32base.h>

#include "fmradiopropertyobserver.h"

class MFMRadioSystemEventDetectorObserver;

/** System properties mappings */
enum TFMRadioSystemEvents
   {
   EFMRadioNetworkCoverageProperty = 0,
   EFMRadioCallProperty,
   EFMRadioAudioResourcesProperty,
   EFMRadioVoiceUiProperty
   };

/**
* Catches notification of the call and network state changes.
*
* Class provides implementation to detect system events and notify UI components of the event,
* Class keeps several instances of CFMRadioPropertyObserver, and those instances send event notifications 
* about the particular system component state change. 
*/
class CFMRadioSystemEventDetector : public CBase, public MFMRadioPropertyChangeObserver
    {
    public:

        /**
        * Two-phased constructor.
        * @param aObserver a reference to the observer interface implementer
        */
        static CFMRadioSystemEventDetector* NewL(MFMRadioSystemEventDetectorObserver&  aObserver);
        
        /**
        * Destructor.
        */
        virtual ~CFMRadioSystemEventDetector();

		/**
		* Returns network coverage state.
		*/
		TBool IsNetworkCoverage() const;

        /** Is Auto resume allowed; based on categories */		
		TBool IsAutoResumePossible() const;
		
		/**
		* Returns current call state.
		*/
		TBool IsCallActive() const;



    private:

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        * @param aObserver a reference to the observer interface implementer     
        */
        CFMRadioSystemEventDetector(MFMRadioSystemEventDetectorObserver&  aObserver);
        /**
        * Callback for auto resume timer
        * @param aSelfPtr pointer to itself
        */        
        static TInt StaticAutoResumeTimerCallback( TAny* aSelfPtr );
        /**
         * Report audio resource availability to observers.  
         */
        void NotifyAudioResourcesAvailability();
    
        // From base class MFMRadioPropertyChangeObserver
        void HandlePropertyChangeL(const TUid& aCategory, const TUint aKey, const TInt aValue);
        void HandlePropertyChangeL(const TUid& aCategory, const TUint aKey, const TDesC8& aValue);
        void HandlePropertyChangeL(const TUid& aCategory, const TUint aKey, const TDesC& aValue);
        void HandlePropertyChangeErrorL(const TUid& aCategory, const TUint aKey, TInt aError);
        
	private:    // Data
		
        /** Array of audio policy categories that radio must not resume after. */
        RArray<TInt> iNoAutoResumeAudioCategories;
    
        /**Observer which will be notified of the call and network state changes*/
		MFMRadioSystemEventDetectorObserver&  iObserver;
	
        /**An array of handlers to Publish & Subscribe interface*/        		
		RPointerArray<CFMRadioPropertyObserver> iPropertyArray;

		/**Indicator for the network status*/
		TBool iIsNetworkCoverage;
		// for tracking currently playing audio clients
		TInt iNumberOfActiveAudioClients;
		/** Indicator for Voice UI status. */
		TBool iIsVoiceUiActive;
		/** flag for call status */
		TBool iIsCallActive;
		/** call status observer */
		CFMRadioPropertyObserver* iCallStatusObserver;
        /* timer for audio auto resume */
        CPeriodic* iAutoResumeTimer;		
		
    };

#endif // CFMRADIOSYSTEMEVENTDETECTOR_H
            
