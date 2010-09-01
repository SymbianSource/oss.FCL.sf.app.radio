/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains implementation of the TRadioSettings class, which holds 
*                       the RadioEngine settings.
*
*/


#ifndef FMRADIOSETTINGS_H
#define FMRADIOSETTINGS_H

#include "fmradioengine.h"

// CONSTANTS
const TInt KDefaultRadioVolume = 3;

// CLASS DECLARATION
//class CRadioEngine;


/**
*  TRadioSettings class.
*  Maintain the radio setting data.
* 
*/
class TRadioSettings
    {
    public:
        /**  
        * default constructor.
        */
       TRadioSettings();
   
        /**
        * return current headset volume.
        * @since Series 60 2.7
        * @return current headset volume
        */
       TInt HeadsetVolume() const; 
        /**
        * return current speaker volume.
        * @since Series 60 2.7
        * @return current speaker volume
        */
   	    TInt SpeakerVolume() const;
        /**
        * return current preset number.
        * @since Series 60 2.7
        * @return current preset number
        */
        TInt CurrentPreset() const; 
        /**
        * return current frequency.
        * @since Series 60 2.7
        * @return current frequency
        */
        TInt Frequency() const; 
        /**
        * return max frequency.
        * @return max frequency
        */
        TUint32 MaxFrequency() const;
	    /**
        * return min frequency.
        * @return min frequency
        */
        TUint32 MinFrequency() const;
	    /**
        * return step size for tuning.
        * @return step size
        */
        TUint32 FrequencyStepSize() const;
	    /**
	    * Returns decimal count for current region MHz information  
	    * @return decimal count for current region
	    */
	    TInt DecimalCount() const;	
        /**
        * return current audio mode.
        * @since Series 60 2.7
        * @return stereo/mono
        */
        CRadioEngine::TFMRadioAudioMode AudioMode() const; 
        /**
        * return current audio output destination.
        * @since Series 60 2.7
        * @return headset/IHF
        */
        CRadioEngine::TFMRadioAudioOutput AudioOutput() const;
        /**
        * check if radio is mute.
        * @since Series 60 2.7
        * @return true/false
        */
        TBool IsMuteOn() const;
        /**
        * check if radio is on.
        * @since Series 60 2.7
        * @return true/false
        */
        TBool IsRadioOn() const;
        /**
        * return radio mode.
        * @since Series 60 2.7
        * @return tune/preset mode
        */
        CRadioEngine::TRadioMode RadioMode() const; 
        /**
        * check if headset is connected.
        * @since Series 60 2.7
        * @return true/false
        */
        TBool IsHeadsetConnected() const; 
        /**
        * check if flight mode is enabled.
        * @since Series 60 2.7
        * @return true/false
        */
       TBool IsFlightModeEnabled() const;
       /**
        * Retrieves the RDS alternate frequency search setting
        *
        * @return ETrue if AF search is enabled, otherwise EFalse
        */
       IMPORT_C TBool IsRdsAfSearchEnabled();
        /**
        * set speaker volume.
        * @since Series 60 2.7
        * @param aVolume the new volume
        * @return none
        */
       void SetSpeakerVolume(const TInt aVolume);
        /**
        * set headset volume.
        * @since Series 60 2.7
        * @param aVolume the new volume
        * @return none
        */
       void SetHeadsetVolume(const TInt aVolume);
        /**
        * Set current preset number.
        * @since Series 60 2.7
        * @return current preset number
        */
       void SetCurrentPreset(const TInt aChannel);
        /**
        * set current frequency .
        * @since Series 60 2.7
        * @param aFrequency the frequency to be set
        * @return none
        */
       void SetFrequency(const TInt aFrequency);
        /**
        * set max frequency.
        * @param aMaxFrequency the frequency to be set
        * @return none
        */
	   void SetMaxFrequency( const TUint32 aMaxFrequency );
	    /**
        * set min frequency.
        * @param aMinFrequency the frequency to be set
        * @return none
        */
	    void SetMinFrequency( const TUint32 aMinFrequency );
	    /**
        * set step size for manual tuning.
        * @param aFrequencyStepSize frequency step
        * @return none
        */
	    void SetFrequencyStepSize( const TUint32 aFrequencyStepSize );
	    /**
	    * Set decimal count for current region MHz information  
	    * @param aDecimalCount decimals used
	    * @return none
	    */
	    void SetDecimalCount( const TInt aDecimalCount );	
        /**
        * set audio mode.
        * @since Series 60 2.7
        * @param aAudioMode stereo/mono
        * @return none
        */
       void SetAudioMode(CRadioEngine::TFMRadioAudioMode aAudioMode);
        /**
        * set audio output.
        * @since Series 60 2.7
        * @param aAudioOutput headset/IHF
        */
       void SetAudioOutput(const CRadioEngine::TFMRadioAudioOutput aAudioOutput );
        /**
        * set mute on.
        * @since Series 60 2.7
        * @return none
        */
       void SetMuteOn();
        /**
        * set mute on.
        * @since Series 60 2.7
        * @return none
        */
       void SetMuteOff();
        /**
        * set radio on.
        * @since Series 60 2.7
        * @return none
        */
       void SetRadioOn();
        /**
        * set radio off.
        * @since Series 60 2.7
        * @return none
        */
       void SetRadioOff();
        /**
        * set radio mode.
        * @since Series 60 2.7
        * @param aRadioMode preset/tune mode
        * @return none
        */
       void SetRadioMode(const CRadioEngine::TRadioMode aRadioMode);
        /**
        * set headset connected.
        * @since Series 60 2.7
        * @return none
        */
       void SetHeadsetConnected();
        /**
        * set headset disconnected.
        * @since Series 60 2.7
        * @return none
        */
       void SetHeadsetDisconnected();
        /**
        * set flight mode.
        * @since Series 60 2.7
        * @param aEnabled true/false
        * @return none
        */
       void SetFlightMode( TBool aEnabled );
       /**
       * set startup count
       * @param aStartupCount number of app startups
       */
       void SetStartupCount( const TInt aStartupCount );
       /**
       * return startup count
       * @return number of app startups
       */
	   TInt StartupCount() const;

       /**
        * Set rds af search enabled.
        * @return none
        */
       void SetRdsAfSearchEnabled();
       /**
        * Set rds af search disabled.
        * @return none
        */
       void SetRdsAfSearchDisabled();
       /**
        * Set Japan region support on/off.
        * @param aAllowed true/false
        * @return none
        */
       void SetJapanRegionAllowed( const TBool aAllowed );
       /**
        * Return status of the Japan region support
        * @return ETrue if supported
        */       
       TBool IsJapanRegionAllowed() const;
              
    private: 
        /// current headset volume level
        TInt iFMRadioHeadsetVolume;
        /// current speaker volume level
        TInt iFMRadioSpeakerVolume;
        /// currently selected channel.
        TInt iCurrentPreset; 
        /// current frequency
        TInt iFrequency; 
        // max frequency 
        TUint32 iMaxFrequency;
        //  min frequency
        TUint32 iMinFrequency;
        // step size
        TUint32 iFrequencyStepSize;
        // decimal count
        TInt iDecimalCount;
        /// stereo or mono
        CRadioEngine::TFMRadioAudioMode iAudioMode; 
        /// audio output
        CRadioEngine::TFMRadioAudioOutput iAudioOutput;
        /// current mute state
        TBool iMuteOn;
        //  radio on/off flag
        TBool iRadioOn;
        /// preset or direct frequency flag
        CRadioEngine::TRadioMode iRadioMode; 
        /// headset connected flag
        TBool iIsHeadsetConnected; 
        /// flight mode enabled flag
        TBool iFlightModeEnabled;
        // first startup case flag
        TInt iStartupCount;        
        //  rds af search enabled flag
        TBool iRdsAfSearchEnabled;
        //  Japan region support flag
        TBool iJapanRegionAllowed;

    };

#endif  // FMRADIOSETTINGS_H

// End of File
