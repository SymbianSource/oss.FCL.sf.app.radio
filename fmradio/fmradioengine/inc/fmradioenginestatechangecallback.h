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
* Description:  Declaration of a simple interface to integrate the FM Radio
*                 application engine and Ui.
*                 Ui is derived from this class and Ui implements
*                 the virtual method HandleRadioEngineCallBackL in class
*                 CFMRadioAppUi. Radio engine then calls this virtual function
*                 when it needs to pass information i.e. about its changed
*                 state where Ui must react to. The traffic between Ui and
*                 engine goes like:
*                 Ui::RequestToEngine() -> Engine() -> Ui::DoAndShowItAndUpdateState()
*
*/


#ifndef FMRADIOENGINESTATECHANGECALLBACK_H
#define FMRADIOENGINESTATECHANGECALLBACK_H

// Class declaration

/**
*  It is interface to integrate the FM Radio
*  application engine and Ui.
*  @lib FMRadioEngine.lib
*  @since Series 60 2.7
*/
class MRadioEngineStateChangeCallback
{
    public:
        /**
        * Event codes for identifying the event that
        * caused the change in engine's internal state.
        * Also, the possible error context is determined
        * from the event code. See the TFMRadioErrorCode
        * decalaration below for further information.
        */
        enum TFMRadioNotifyEvent
        {
            // Radio functionality events
            EFMRadioEventNone = 0x00,
            EFMRadioEventTunerReady,
            EFMRadioEventRadioOn,
            EFMRadioEventRadioOff,
            EFMRadioEventTune,
            EFMRadioEventVolumeUpdated,
            EFMRadioEventSetAudioMode,
            EFMRadioEventSetAudioOutput,
            EFMRadioEventHeadsetDisconnected,
            EFMRadioEventHeadsetReconnected,
            EFMRadioEventButtonPressed,
            EFMRadioEventFlightModeEnabled,
            EFMRadioEventFlightModeDisabled,
            EFMRadioEventStandbyMode,
            EFMRadioEventSetMuteState,
            EFMRadioEventAudioResourceAvailable,
            EFMRadioEventAudioResourcePaused,
            EFMRadioEventAudioResourceLost,
            EFMRadioEventCallStarted,
            EFMRadioEventCallEnded,
            EFMRadioEventFMRadioInitialized,
            EFMRadioEventFMTransmitterOn,
            EFMRadioEventFreqRangeChanged,
            EFMRadioEventRDSNotSupported,
            EFMRadioEventScanLocalStationsCanceled

        };
    public:
        /**
        * Interface method to get the callback from the
        * radio engine to radio Ui.
        *
        * @param aEventCode Predefined code of the event which
        *        caused the change in engine's internal state.
        * @param aErrorCode Predefined code of the error if
        *        one was generated. If no errors, value is 0x00 (NULL).
        * @return none
        */
        virtual void HandleRadioEngineCallBack(TFMRadioNotifyEvent aEventCode, TInt aErrorCode ) = 0;
};

#endif  //FMRADIOENGINESTATECHANGECALLBACK_H

// End of file
