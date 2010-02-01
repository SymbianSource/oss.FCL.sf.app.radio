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
* Description:  Declaration for the control event observer interface
*
*/

#ifndef MFMRADIOCONTROLEVENTOBSERVER_H
#define MFMRADIOCONTROLEVENTOBSERVER_H

#include "fmradioaudioutils.hrh"
#include "fmradiosystemutils.h"

/**
 *  MFMRadioControlEventObserver
 *
 *  MFMRadioControlEventObserver defines observer interface for FM Radio control events.
 */
class MFMRadioControlEventObserver
    {
    
public:
        
    /**
     * Called when "step to channel" request occurs.
     *
     * @param aDirection EFMRadioUp for stepping up, EFMRadioDown for stepping down
     */
    virtual void StepToChannelL( TFMRadioTuneDirection aDirection ) = 0;
    
    /**
     * Called when "set channel" request occurs.
     *
     * @param aChannelId Channel id to set
     */
    virtual void SetChannelL( TInt aChannelId ) = 0;
    
    /**
     * Called when "seek" request occurs.
     *
     * @param aDirection If EFMRadioUp seeks up, otherwise seeks down
     */
    virtual void SeekL( TFMRadioTuneDirection aDirection ) = 0;

    /**
     * Called when "step to frequency up" request occurs.
     *
     * @param aDirection EFMRadioUp for stepping up, EFMRadioDown for stepping down
     */
    virtual void StepToFrequencyL( TFMRadioTuneDirection aDirection ) = 0;
    
    /**
     * Called when "set frequency" request occurs.
     *
     * @param aFreq Frequency to set
     */
    virtual void SetFrequencyL( TUint32 aFreq ) = 0;
    
    /**
     * Called when "adjust volume" request occurs.
     *
     * @param aDirection If EFMRadioDecVolume decreases, otherwise increases 
     * volume
     */
    virtual void AdjustVolumeL( TFMRadioVolumeSetDirection aDirection ) = 0;
    
    /**
     * Called when "mute" request occurs.
     * 
     * @param aMute if ETrue mutes, else unmutes radio
     */
    virtual void MuteL( TBool aMute ) = 0;

    /**
    * Called when "play" request occurs.
    *
    * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
    */
    virtual void PlayL( TBool aDownPressed ) = 0;

    /**
     * Called when "pause" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void PauseL(TBool aDownPressed ) = 0;

    /**
     * Called when "play", "pause" or "pauseplay" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void PausePlayL( TBool aDownPressed ) = 0;
    
    /**
     * Called when "stop" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void StopL( TBool aDownPressed ) = 0;
    
    /**
     * Called when "forward" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void ForwardL( TBool aDownPressed ) = 0;
    
    /**
     * Called when "fast forward" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void FastForwardL( TBool aDownPressed ) = 0;
    
    /**
     * Called when "backward" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void BackwardL( TBool aDownPressed ) = 0;
    
    /**
     * Called when "rewind" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void RewindL( TBool aDownPressed ) = 0;
    
    /**
     * Called when "channel up" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void ChannelUpL( TBool aDownPressed ) = 0;
    
    /**
     * Called when "channel down" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void ChannelDownL( TBool aDownPressed ) = 0;
    
    /**
     * Called when "volume up" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void VolumeUpL( TBool aDownPressed ) = 0;
    
    /**
     * Called when "volume down" request occurs.
     *
     * @param aDownPressed If ETrue, button is down. If EFalse, button is up.
     */
    virtual void VolumeDownL( TBool aDownPressed ) = 0;
    
    /**
     * Called when "change view" request occurs
     *
     * @param aViewId the id of activated view
     * @param aForceChange if ETrue, view changing is forced and state 
     * of FM Radio is not checked
     */
    virtual void ChangeViewL(TUid aViewId, TBool aForceChange) = 0;
    
    /**
     * Called when output source is changed
     * @param aDestination the current output source
     */
    virtual void SetAudioOutputDestinationL(TFMRadioOutputDestination aDestination) = 0;
    
    /**
     * Called when headset button answer/end call is pressed
     */
    virtual void AnswerEndCallL() = 0;
    };

#endif      // MFMRADIOCONTROLEVENTOBSERVER_H   
            

