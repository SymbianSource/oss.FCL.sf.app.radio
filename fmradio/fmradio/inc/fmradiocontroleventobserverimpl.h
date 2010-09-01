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
* Description:  Declaration of class CFMRadioControlEventObserverImpl. 
*               Provides the implementation for the control event observer.
* 
*/

#ifndef FMRADIOCONTROLEVENTOBSERVERIMPL_H_
#define FMRADIOCONTROLEVENTOBSERVERIMPL_H_

#include <fmradiointernalpskeys.h>

#include "fmradiopropertyobserver.h"
#include "fmradioaudioutils.hrh"
#include "fmradiocontroleventobserver.h"

class MFMRadioControlEventObserver;
class CFMRadioAppUi;

class CFMRadioControlEventObserverImpl : public CBase, public MFMRadioControlEventObserver
    {
    public:
        /**
         * Static constructor.
         */
        static CFMRadioControlEventObserverImpl* NewL( CFMRadioAppUi& aAppUi );
        
        /**
        * Destructor.
        */
        ~CFMRadioControlEventObserverImpl();
        
        void StepToChannelL( TFMRadioTuneDirection aDirection );
        void SetChannelL( TInt aChannelId );
        void SeekL( TFMRadioTuneDirection aDirection );
        void StepToFrequencyL( TFMRadioTuneDirection aDirection );
        void SetFrequencyL( TUint32 aFreq );
        void AdjustVolumeL( TFMRadioVolumeSetDirection aDirection );
        void MuteL( TBool aMute );
        void PlayL( TBool aDownPressed );
        void PauseL(TBool aDownPressed );
        void PausePlayL( TBool aDownPressed );
        void StopL( TBool aDownPressed );
        void ForwardL( TBool aDownPressed );
        void FastForwardL( TBool aDownPressed );
        void BackwardL( TBool aDownPressed );
        void RewindL( TBool aDownPressed );
        void ChannelUpL( TBool aDownPressed );
        void ChannelDownL( TBool aDownPressed );
        void VolumeUpL( TBool aDownPressed );
        void VolumeDownL( TBool aDownPressed );
        void ChangeViewL(TUid aViewId, TBool aForceChange);
        void SetAudioOutputDestinationL(TFMRadioOutputDestination aDestination);
        void AnswerEndCallL();

    private: //Methods
    	CFMRadioControlEventObserverImpl(CFMRadioAppUi& aAppUi);
        void ConstructL();

    private:
    	CFMRadioAppUi& iAppUi;
    };
#endif /*FMRADIOCONTROLEVENTOBSERVERIMPL_H_*/
