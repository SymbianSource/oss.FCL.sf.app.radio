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
* Description: Engine component for FMRadio Active Idle plugin.
*
*/


#ifndef __FMRADIOACTIVEIDLEENGINE_H__
#define __FMRADIOACTIVEIDLEENGINE_H__

#include <e32base.h>
#include <fmradiointernalpskeys.h>

#include "fmradiopropertyobserver.h"

class MFMRadioActiveIdleEngineNotifyHandler;


/**
* Engine component for FMRadio Active Idle plugin.
*
* This class is responsible for listening to relevant P&S keys and notifying the UI code whenever the listened keys change.
* The actual listening is done by multiple CFMRadioPropertyObserver objects.
* 
* @lib fmradioactiveidleengine200.lib
* 
*/
NONSHARABLE_CLASS(CFMRadioActiveIdleEngine) : public CBase, public MFMRadioPropertyChangeObserver
    {
    public:
        
        /**
        * Static constructor.
        * @param    aObserver   The observer to be notified of the changes in the keys.
        * @return   The newly created CFMRadioActiveIdleEngine object.
        */
        IMPORT_C static CFMRadioActiveIdleEngine* NewL( MFMRadioActiveIdleEngineNotifyHandler& aObserver );
        
        /**
        * Destructor.
        */
        IMPORT_C ~CFMRadioActiveIdleEngine();
        
        /**
        * Activates subscription to P&S keys if not already active.
        * Must be called after CFMRadioActiveIdleEngine::NewL, or after Cancel.
        */
        IMPORT_C void ActivateL();
        
        /**
        * Stops subscribing to P&S keys.
        */
        IMPORT_C void Cancel();
                
        /**
        * Returns the current volume level of the radio.
        * @return   The current volume level of the radio.
        */
        IMPORT_C TInt RadioVolume() const;
        
        /**
        * Sets the radio volume.
        * @param    aVolume     The radio volume to set.
        */
        IMPORT_C void AdjustRadioVolume( TFMRadioPSAdjustVolume aVolume ) const;

        /**
        * Returns the current tuning state.
        * @return   The current tuning state.
        */
        IMPORT_C TFMRadioPSTuningState TuningState() const;
        
        /**
        * Returns the current frequency.
        * @return   The current frequency.
        */
        IMPORT_C TInt Frequency() const;
        
        /**
        * Returns the current channel ID.
        * @return   The current channel ID.
        */
        IMPORT_C TInt Channel() const;
        
        /**
        * Returns the current channel name.
        * @return   The current channel name.
         */
        IMPORT_C const TDesC& ChannelName() const;
        
        /**
        * Returns the current mute state.
        * @return   The current mute state.
        */
        IMPORT_C TFMRadioPSRadioMuteState MuteState() const;
        
        /**
        * Sets the mute state.
        * @param    aMuteState  The mute state to set.
        */
        IMPORT_C void SetMuteState( TFMRadioPSRadioMuteState aMuteState ) const;
        
        /**
        * Returns the antenna's current connectivity status. 
        * @return   The antenna's current connectivity status.
        */
        IMPORT_C TFMRadioPSHeadsetStatus AntennaStatus() const;
        
        /**
        * Returns the current frequency decimal count.
        * @return   The current frequency decimal count.
        */
        IMPORT_C TFMRadioPSFrequencyDecimalCount FrequencyDecimalCount() const;
        
        /**
        * Returns FM Radio application's running state.
        * @return Application's running state.
        */
        IMPORT_C TFMRadioPSApplicationRunningState ApplicationRunningState() const;
        
        /**
        * Returns the radio's power state.
        * @return   The power state.
        */
        IMPORT_C TFMRadioPSRadioPowerState PowerState() const;
        
        /**
        * Returns the RDS Program Service information.
        * @return   RDS Program Service.
        */
        IMPORT_C const TDesC& RDSProgramService() const;

    protected: 

		// from base class MFMRadioPropertyChangeObserver
        void HandlePropertyChangeL( const TUid& aCategory, TUint aKey, TInt aValue );
        void HandlePropertyChangeL( const TUid& aCategory, TUint aKey, const TDesC8& aValue );
        void HandlePropertyChangeL( const TUid& aCategory, TUint aKey, const TDesC& aValue );
        void HandlePropertyChangeErrorL( const TUid& aCategory, TUint aKey, TInt aError );
        
    private:
    
        /**
        * Constructor.
        * @param    aObserver   The observer to be notified of the changes in the keys.
        */
        CFMRadioActiveIdleEngine( MFMRadioActiveIdleEngineNotifyHandler& aObserver );
    
        /**
        * Second-phase constructor.
        */
        void ConstructL();        
        
    protected:
    
        /** The observer to be notified of the changes in the listened keys. */
        MFMRadioActiveIdleEngineNotifyHandler& iObserver;
        /** Array of status observers. */
        RPointerArray<CFMRadioPropertyObserver> iPropertyObserverArray;

	private:
        
    };

#endif //__FMRADIOACTIVEIDLEENGINE_H__
