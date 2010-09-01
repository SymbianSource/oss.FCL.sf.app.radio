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


//  INCLUDES
#include "fmradioengine.h"
#include "fmradioengineradiosettings.h"


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------
// TRadioSettings::TRadioSettings
// Default class constructor.
// ----------------------------------------------------
//
TRadioSettings::TRadioSettings():
                    iFMRadioHeadsetVolume( KDefaultRadioVolume ),
                    iFMRadioSpeakerVolume( KDefaultRadioVolume ),
                    iCurrentPreset( 0 ),
                    iFrequency( KDefaultRadioFrequency ),
                    iMaxFrequency( 0 ),
                    iMinFrequency( 0 ),
                    iFrequencyStepSize( 0 ),
                    iDecimalCount( 0 ),
                    iAudioMode( CRadioEngine::EFMRadioStereo ),
                    iAudioOutput(CRadioEngine::EFMRadioOutputHeadset ),
                    iMuteOn(EFalse),
                    iRadioOn( EFalse),
                    iRadioMode( CRadioEngine::ERadioPresetMode ),
                    iIsHeadsetConnected( EFalse ),
                    iFlightModeEnabled( EFalse ),
                    iStartupCount( 0 ),
                    iRdsAfSearchEnabled( EFalse ),
                    iJapanRegionAllowed( ETrue )
    {
    }

// ----------------------------------------------------
// TRadioSettings::Volume
// retrieve current headset volume level
// Returns: iFMRadioHeadsetVolume: headset volume
// ----------------------------------------------------
//
TInt TRadioSettings::HeadsetVolume() const
    {
    return iFMRadioHeadsetVolume;
    }

// ----------------------------------------------------
// TRadioSettings::SpeakerVolume
// retrieve current speaker volume level
// Returns: iFMRadioSpeakerVolume: speaker volume
// ----------------------------------------------------
//
TInt TRadioSettings::SpeakerVolume() const
    {
    return iFMRadioSpeakerVolume;
    }

// ----------------------------------------------------
// TRadioSettings::CurrentPreset
// retrieve currently selected channel
// Returns: iCurrentPreset: current preset index number
// ----------------------------------------------------
//
TInt TRadioSettings::CurrentPreset() const
    {
    	return iCurrentPreset;
    }

// ----------------------------------------------------
// TRadioSettings::Frequency
// retrieve current frequency
// Returns: iFrequency: current frequency
// ----------------------------------------------------
//
TInt TRadioSettings::Frequency() const
    {
    return iFrequency;
    }

// ----------------------------------------------------
// TRadioSettings::MaxFrequency
// retrieve max frequency
// ----------------------------------------------------
//
TUint32 TRadioSettings::MaxFrequency() const
    {
    return iMaxFrequency;
    }

// ----------------------------------------------------
// TRadioSettings::MinFrequency
// retrieve min frequency
// ----------------------------------------------------
//
TUint32 TRadioSettings::MinFrequency() const
    {
    return iMinFrequency;
    }
 
// ----------------------------------------------------
// TRadioSettings::FrequencyStepSize
// return step size for tuning
// ----------------------------------------------------
//
TUint32 TRadioSettings::FrequencyStepSize() const
    {
    return iFrequencyStepSize;
    }

// ----------------------------------------------------
// TRadioSettings::DecimalCount
// Returns decimal count for current region MHz information 
// ----------------------------------------------------
//
TInt TRadioSettings::DecimalCount() const
    {
    return iDecimalCount;
    }	
 
// ----------------------------------------------------
// TRadioSettings::AudioMode
// retrieve current audio mode (stereo or mono)
// Returns: iAudioMode: current mode
// ----------------------------------------------------
//
CRadioEngine::TFMRadioAudioMode TRadioSettings::AudioMode() const
    {
    return iAudioMode;
    }

// ----------------------------------------------------
// TRadioSettings::AudioOutput
// retrieve current audio output (headset/ihf)
// Returns: iAudioOutput: current audio output
// ----------------------------------------------------
//
CRadioEngine::TFMRadioAudioOutput TRadioSettings::AudioOutput() const
    {
    return iAudioOutput;
    }

// ----------------------------------------------------
// TRadioSettings::IsMuteOn
// retrieve current mute state (on/off)
// Returns: iMuteOn: mute flag
// ----------------------------------------------------
//
TBool TRadioSettings::IsMuteOn() const
    {
    return iMuteOn;
    }

// ----------------------------------------------------
// TRadioSettings::IsRadioOn
// retrieve current radio on state (on/off)
// Returns: iRadioOn: a flag indicating radio on/off
// ----------------------------------------------------
//
TBool TRadioSettings::IsRadioOn() const
    {
    return iRadioOn;
    }

// ----------------------------------------------------
// TRadioSettings::RadioMode
// retrieve the current radio mode (preset or direct frequency)
// Returns: iRadioMode: a flag indicating radio mode
// ----------------------------------------------------
//
CRadioEngine::TRadioMode TRadioSettings::RadioMode() const
    {
    return iRadioMode;
    }

// ----------------------------------------------------
// TRadioSettings::IsHeadsetConnected
// retrieve the current headset mode (connected or disconnected)
// Returns: iIsHeadsetConnected: a flag indicating if headset is connected
// ----------------------------------------------------
//
TBool TRadioSettings::IsHeadsetConnected() const
    {
    return iIsHeadsetConnected;
    }

// ----------------------------------------------------
// TRadioSettings::IsFlightModeEnabled
// retrieve the current flight mode status (on/off)
// Returns: iFlightModeEnabled: a flag indicating if flight mode is enabled
// ----------------------------------------------------
//
TBool TRadioSettings::IsFlightModeEnabled() const
    {
    return iFlightModeEnabled;
    }

// ----------------------------------------------------
// TRadioSettings::IsRdsAfSearchEnabled
// ----------------------------------------------------
//
EXPORT_C TBool TRadioSettings::IsRdsAfSearchEnabled()
	{
	return iRdsAfSearchEnabled;
	}

// ----------------------------------------------------
// TRadioSettings::SetHeadsetVolume
// set the current fm radio headset volume
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetHeadsetVolume(
    const TInt aVolume) // the new volume
    {
    iFMRadioHeadsetVolume = aVolume;
    }

// ----------------------------------------------------
// TRadioSettings::SetSpeakerVolume
// set the current fm radio speaker volume
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetSpeakerVolume(
    const TInt aVolume)  // new volume
	  {
	  iFMRadioSpeakerVolume = aVolume;
	  }


// ----------------------------------------------------
// TRadioSettings::SetCurrentPreset
// set the current preset
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetCurrentPreset(
    const TInt aChannel) //new channel to be set
    {
    	iCurrentPreset = aChannel;
    }

// ----------------------------------------------------
// TRadioSettings::SetFrequency
// set the current frequency
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetFrequency(
    const TInt aFrequency) // new frequency
    {
    	iFrequency = aFrequency;
    //	if (iFrequency < 87500000 || iFrequency > 108000000)
    //		iFrequency = 87500000;
    }

// ----------------------------------------------------
// TRadioSettings::SetMaxFrequency
// set the max frequency
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetMaxFrequency( const TUint32 aMaxFrequency )
	{
	iMaxFrequency = aMaxFrequency;
	}

// ----------------------------------------------------
// TRadioSettings::SetMinFrequency
// set min frequency
// Returns: none
// ----------------------------------------------------
//
 void TRadioSettings::SetMinFrequency( const TUint32 aMinFrequency )
	{
	iMinFrequency = aMinFrequency;
	}

 // ----------------------------------------------------
 // TRadioSettings::SetFrequencyStepSize
 // set step size for manual tuning.
 // Returns: none
 // ----------------------------------------------------
 //
 void TRadioSettings::SetFrequencyStepSize( const TUint32 aFrequencyStepSize )
	{
	iFrequencyStepSize = aFrequencyStepSize;
	}

 // ----------------------------------------------------
 // TRadioSettings::SetDecimalCount
 // Set decimal count for current region MHz information 
 // Returns: none
 // ----------------------------------------------------
 //
 void TRadioSettings::SetDecimalCount( const TInt aDecimalCount )
	{
	iDecimalCount = aDecimalCount;
	}	
 
// ----------------------------------------------------
// TRadioSettings::SetAudioMode
// set the current audio mode
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetAudioMode(
    const CRadioEngine::TFMRadioAudioMode aAudioMode) // new audio mode
    {
    iAudioMode = aAudioMode;
    }

// ----------------------------------------------------
// TRadioSettings::SetAudioOutput
// set the current audio output
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetAudioOutput(
    const CRadioEngine::TFMRadioAudioOutput aAudioOutput) // new audio output
    {
    iAudioOutput = aAudioOutput;
    }

// ----------------------------------------------------
// TRadioSettings::SetMuteOn
// set mute state to on
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetMuteOn()
    {
    iMuteOn = ETrue;
    }

// ----------------------------------------------------
// TRadioSettings::SetMuteOff
// set mute state to off
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetMuteOff()
    {
    iMuteOn = EFalse;
    }

// ----------------------------------------------------
// TRadioSettings::SetRadioOn
// set radio state to "on"
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetRadioOn()
    {
    iRadioOn = ETrue;
    }

// ----------------------------------------------------
// TRadioSettings::SetRadioOff
// set radio state to "off"
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetRadioOff()
    {
    iRadioOn = EFalse;
    }

// ----------------------------------------------------
// TRadioSettings::SetRadioMode
// set radio mode
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetRadioMode(
    const CRadioEngine::TRadioMode aRadioMode) // new radio mode
    {
    iRadioMode = aRadioMode;
    }

// ----------------------------------------------------
// TRadioSettings::SetHeadsetConnected
// set headset connected
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetHeadsetConnected()
    {
    iIsHeadsetConnected = ETrue;
    }

// ----------------------------------------------------
// TRadioSettings::SetHeadsetDisconnected
// set headset disconnected
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetHeadsetDisconnected()
    {
    iIsHeadsetConnected = EFalse;
    }

// ----------------------------------------------------
// TRadioSettings::SetFlightMode
// set flightmode status
// Returns: none
// ----------------------------------------------------
//
void TRadioSettings::SetFlightMode( 
    TBool aEnabled) // indicating enable/disable
    {
    iFlightModeEnabled = aEnabled;
    }

// ----------------------------------------------------
// TRadioSettings::SetStartupCount
// set startup count
// ----------------------------------------------------
//	
void TRadioSettings::SetStartupCount( const TInt aStartupCount )
	{
	iStartupCount = aStartupCount;	
	}
	
// ----------------------------------------------------
// TRadioSettings::StartupCount
// return startup count
// ----------------------------------------------------
//	
TInt TRadioSettings::StartupCount() const
	{
	return iStartupCount;
	}

// ----------------------------------------------------
// TRadioSettings::SetRdsAfSearchEnabled
// ----------------------------------------------------
//
void  TRadioSettings::SetRdsAfSearchEnabled()
	{
	iRdsAfSearchEnabled = ETrue;
	}

// ----------------------------------------------------
// TRadioSettings::SetRdsAfSearchDisabled
// ----------------------------------------------------
//
void  TRadioSettings::SetRdsAfSearchDisabled()
	{
	iRdsAfSearchEnabled = EFalse;
	}
		
// ----------------------------------------------------
// TRadioSettings::SetJapanRegionAllowed
// ----------------------------------------------------
//
void  TRadioSettings::SetJapanRegionAllowed( const TBool aAllowed )
	{
	iJapanRegionAllowed = aAllowed;
	}	

// ----------------------------------------------------
// TRadioSettings::IsJapanRegionAllowed
// ----------------------------------------------------
//
TBool TRadioSettings::IsJapanRegionAllowed() const
	{
	return iJapanRegionAllowed;	
	}	
	
// end of file

