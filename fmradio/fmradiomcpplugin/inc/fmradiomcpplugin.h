/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration for the class CFMRadioMCPPlugin.
*
*/

#ifndef FMRADIOMCPPLUGIN_H
#define FMRADIOMCPPLUGIN_H

#include <fmradiointernalpskeys.h>
#include <mcpplugin.h>
#include <mcppluginobserver.h>

#include "fmradioactiveidleenginenotifyhandler.h"

class CFMRadioActiveIdleEngine;
class MFMRadioActiveIdleEngineNotifyHandler;

/**
* Music content publisher plugin for FM Radio.
*
* Implements plugin which publishes FM Radio state to affected parties.
* 
* @lib fmradiomcpplugin100.lib
* 
*/
NONSHARABLE_CLASS( CFMRadioMCPPlugin ) : public CMCPPlugin,
                                         public MFMRadioActiveIdleEngineNotifyHandler
    {
public:
    
    static CFMRadioMCPPlugin* NewL( MMCPPluginObserver* aObserver );
    ~CFMRadioMCPPlugin();

// from base class CMCPPlugin
    void Deactivate();

// from base class MFMRadioActiveIdleEngineNotifyHandler
    void HandleRadioVolumeChangeL( TInt aVolume );
    void HandleTuningStateChangeL( TFMRadioPSTuningState aTuningState );
    void HandleFrequencyChangeL( TInt aFrequency );
    void HandleChannelChangeL( TInt aId );
    void HandleChannelModifyL( TInt aId );
    void HandleMuteStateChangeL( TFMRadioPSRadioMuteState aMuteState );
    void HandleApplicationRunningStateChangeL( TFMRadioPSApplicationRunningState aRunningState );
    void HandleFrequencyDecimalCountChangeL( TFMRadioPSFrequencyDecimalCount aDecimalCount );
    void HandleAntennaStatusChangeL( TFMRadioPSHeadsetStatus aAntennaStatus );
    void HandlePowerStateChangeL( TFMRadioPSRadioPowerState aPowerState );
    void HandleRDSProgramServiceChangeL( const TDesC& aProgramService );
    void HandleRDSRadioTextChangeL( const TDesC& aRdsText );
    void HandleChannelNameChangeL( const TDesC& aName );
    void HandlePresetListCountChangeL( TInt aPresetCount );    
    void HandlePresetListFocusChangeL( TInt aPresetFocus );  
    
private:
    void InstallFMRadioCommandActionL( const TDesC& aCommand, TMCPTriggerDestination aDestination );
    void ActivateL();
    void InstallEmptyActionL( TMCPTriggerDestination aDestination );
    void DimmNextAndPrevious();
    void PublishApplicationIconL( TMCPImageDestination aDestination );
    void PublishFrequencyL( TInt aFrequency, TMCPTextDestination aDestination );
    void UpdatePublishedToolBarL(TUint aToolBarState);
    void UpdateToolBarL( TBool aForceApplicationClosing = EFalse );    
    void UpdateMusicWidgetTextL( TBool aForceApplicationClosing = EFalse );
    
    /**
     * Transfers the given frequency number as a descriptor
     * @param aFrequency The frequency to be formatted
     * @return The frequency descriptor. Pushed to Cleanup Stack. Ownership is transfered.
     */
    HBufC* FrequencyStringLC( TInt aFrequency );
    
private:
    CFMRadioMCPPlugin( MMCPPluginObserver* aObserver );
    void ConstructL();
    void InitializeResourceLoadingL();

private:
    MMCPPluginObserver* iObserver;

    CFMRadioActiveIdleEngine* iEngine;
    TBool iActive;
    TInt iResourceOffset;
    RBuf iAntennaNotConnectedText;
    RBuf iTuningText;
    RBuf iSavedStationFormat;
    RBuf iSavedStationFormatNoName;
    RBuf iFrequencyFormat;
    
    TUint iTbPreviousState;
    TUint iTbMuteUnmuteState;
    TUint iTbNextState;
    TInt iPresetCount;

    /**
     * Name of the mif-file that provides the published icons.
     * Owned.
     */
    RBuf iMifFileName;
    };

#endif /* FMRADIOMCPPLUGIN_H */
