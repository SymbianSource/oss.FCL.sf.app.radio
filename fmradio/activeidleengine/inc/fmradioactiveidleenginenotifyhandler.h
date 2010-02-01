/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Observer interface for active idle engine
*
*/


#ifndef __FMRADIOACTIVEIDLEENGINENOTIFYHANDLER_H__
#define __FMRADIOACTIVEIDLEENGINENOTIFYHANDLER_H__

#include <fmradiointernalpskeys.h>

/**
* Observer interface for active idle engine.
* The observer is notified about the changes in relevant keys through this interface.
*/
NONSHARABLE_CLASS(MFMRadioActiveIdleEngineNotifyHandler)
    {
    public:
    
        /**
        * Invoked when the radio volume has changed.
        * @param    aVolume     The new radio volume.
        */
        virtual void HandleRadioVolumeChangeL( TInt aVolume ) = 0;
        
        /**
        * Invoked when tuning has taken more than one second, or tuning was previously on, but now completed.
        * @param    aTuningState    The new tuning state.
        */
        virtual void HandleTuningStateChangeL( TFMRadioPSTuningState aTuningState ) = 0;
        
        /**
        * Invoked when a new channel has been tuned into.
        * @param    aIndex      The index of the channel.
        */
        virtual void HandleChannelChangeL( TInt aIndex ) = 0;
        
        /**
        * Invoked when a channel has been modifed.
        * @param    aIndex    The index of the channel that was modified.
        */
        virtual void HandleChannelModifyL( TInt aIndex ) = 0;

        /**
        * Invoked when the frequency has changed.
        * @param    aFrequency      The new frequency.
        */
        virtual void HandleFrequencyChangeL( TInt aFrequency ) = 0;
        
        /**
        * Invoked when the muting state has changed.
        * @param    aMuteState    The new mute state.
        */
        virtual void HandleMuteStateChangeL( TFMRadioPSRadioMuteState aMuteState ) = 0;

        /**
        * Invoked when the application's running state has changed.
        * @param    aRunningState   The new running state.
        */
        virtual void HandleApplicationRunningStateChangeL( TFMRadioPSApplicationRunningState aRunningState ) = 0;

        /**
        * Invoked when the used decimal count is changed.
        * @param    aDecimalCount   The new decimal count.
        */
        virtual void HandleFrequencyDecimalCountChangeL( TFMRadioPSFrequencyDecimalCount aDecimalCount ) = 0;
        
        /**
        * Invoked when the antenna's connectivity status is changed.
        * @param    aAntennaStatus      Antenna's new connectivity status.
        */
        virtual void HandleAntennaStatusChangeL( TFMRadioPSHeadsetStatus aAntennaStatus ) = 0;
        
        /**
        * Invoked when radio's power state changes.
        * @param    aPowerState      The new power state.
        */
        virtual void HandlePowerStateChangeL( TFMRadioPSRadioPowerState aPowerState ) = 0;
        
        /**
        * Invoked when RDS Program Service information changes.
        * @param    aProgramService      New Program Service info.
        */
        virtual void HandleRDSProgramServiceChangeL( const TDesC& aProgramService ) = 0;

        /**
        * Invoked when RDS Text information changes.
        * @param    aRdsText      New Program Service info.
        */        
        virtual void HandleRDSRadioTextChangeL( const TDesC& aRdsText ) = 0;

        /**
        * Invoked when Channel name changes.
        * @param    aName      New channel name.
        */  
        virtual void HandleChannelNameChangeL( const TDesC& aName ) = 0;
        
        virtual void HandlePresetListCountChangeL( TInt aPresetCount ) = 0;
        virtual void HandlePresetListFocusChangeL( TInt aPresetFocus ) = 0;
    };

#endif // __FMRADIOACTIVEIDLEENGINENOTIFYHANDLER_H__
           
