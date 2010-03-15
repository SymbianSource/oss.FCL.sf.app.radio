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
* Description:  The class implements the core functionality of the Radio Engine
*
*/


//  INCLUDE FILES
#include <AudioOutput.h>
#include <AccMonitor.h>
#include <bautils.h>
#include <coemain.h>
#include <ctsydomainpskeys.h>
#include <data_caging_path_literals.hrh>
#include <e32property.h>

#include "fmradioengine.h"
#include "fmradioenginestatechangecallback.h"
#include "debug.h"
#include "fmradioengineradiosettings.h"
#include "fmradioengineradiostatehandler.h"
#include "fmradioenginecentralrepositoryhandler.h"
#include "fmradioenginecrkeys.h"
#include "fmradiosystemeventdetector.h"
#include "fmradiomobilenetworkinfolistener.h"
#include "fmradiopubsub.h"
#include "fmradiordsreceiver.h"
#include "fmradiordsreceiversimulator.h"
#include "fmradioaccessoryobserver.h"
#ifndef __ACCESSORY_FW
#include "fmradioenginedosserverobserver.h"
#endif

// CONSTANTS

const TInt KFMRadioNumberOfVolumeSteps = 20;

// ============================ MEMBER FUNCTIONS ===============================
// ----------------------------------------------------
// CRadioEngine::CRadioEngine
// Default class constructor.
// ----------------------------------------------------
//
CRadioEngine::CRadioEngine(
    MRadioEngineStateChangeCallback& aCallback)
    :
    iAutoResume(EFalse),
    iCurrentRadioState(EStateRadioOff),
    iCallback(aCallback),
    iTempFrequency(KDefaultRadioFrequency),
    iTunePresetRequested(EFalse),
    iInitializeRadioRequestExists(EFalse),
    ilineConstructed( EFalse ),
    iHFOptionActivated ( EFalse )
    {
    }

// ----------------------------------------------------
// CRadioEngine::ConstructL
// Second phase class constructor.
// ----------------------------------------------------
//
void CRadioEngine::ConstructL()
    {
    FTRACE(FPrint(_L("CRadioEngine::ConstructL()")));
    
    TRAP_IGNORE( ConnectLineL() );
    
    InitializeResourceLoadingL(); 

    iRadioSettings = new ( ELeave ) TRadioSettings;
    
    iCentralRepositoryHandler = CCentralRepositoryHandler::NewL( *iRadioSettings );
        
#ifdef __WINS__
    // NetworkInfoListener takes care of listening to the network id and country code.
    iNetworkInfoListener = CFMRadioMobileNetworkInfoListener::NewL( *this );
#endif
    
    // for monitoring network availability
	iSystemEventDetector = CFMRadioSystemEventDetector::NewL( *this );
	
    // create an instance of Radio Utility factory and indicate
    // FM Radio is a primary client
    iRadioUtility = CRadioUtility::NewL( ETrue );

    // Get a tuner utility
    iFmTunerUtility = &iRadioUtility->RadioFmTunerUtilityL( *this );
    
    // Get a player utility
    iPlayerUtility = &iRadioUtility->RadioPlayerUtilityL( *this );

    TInt maxVolume = 0;
    iPlayerUtility->GetMaxVolume( maxVolume );
    FTRACE(FPrint(_L("CRadioEngine::ConstructL()  maxVolume %d"), maxVolume ) );
    // calculate multiplier for scaling the UI volume to the value used by DevSound
    iUiVolumeFactor = ( static_cast<TReal> ( maxVolume ) / KFMRadioNumberOfVolumeSteps );
    
    
    // Get a preset utility
    iPresetUtility = CRadioFmPresetUtility::NewL( *this );
    
#ifdef __WINS__
    iRdsReceiver = CFMRadioRdsReceiverSimulator::NewL( *iRadioSettings );
    static_cast<CFMRadioRdsReceiverSimulator*> ( iRdsReceiver )->SetRadioEngineForRadioFmTunerSimulation( this );
#else
    iRdsReceiver = CFMRadioRdsReceiver::NewL( *iRadioSettings );
#endif
    
    iRdsReceiver->AddObserver( this );
    
    iStateHandler = CRadioStateHandler::NewL( this );

#ifndef __ACCESSORY_FW
    iDosServerObserver = CDosServerObserver::NewL( this, iRadioSettings );
#else
#ifndef __WINS
    TRAP_IGNORE( iAudioOutput = CAudioOutput::NewL( *iPlayerUtility ) );
#endif
#endif
    
    iTopFrequency = 0;
    iBottomFrequency = 0;

    TInt callState = KErrUnknown;
    RMobileCall::TMobileCallStatus linestatus;
   	iLine.GetMobileLineStatus( linestatus );
    RProperty::Get(KPSUidCtsyCallInformation, KCTsyCallState, callState);
                
    // check status from line
    if ( linestatus != RMobileCall::EStatusIdle &&
         linestatus != RMobileCall::EStatusUnknown )
        {
        // Pre-empted due to phone call, start Call State Observer
        iInCall = ETrue;
        }
    // accessory observer
    iHeadsetObserver = CFMRadioAccessoryObserver::NewL();
    iHeadsetObserver->SetObserver( this );    
    // Set audio output to current setting
    if ( iHeadsetObserver->IsHeadsetAccessoryConnected() )
        {
        SetAudioOutput( EFMRadioOutputHeadset );
        }
    else
        {
        SetAudioOutput( EFMRadioOutputIHF );
        }
    // Assume headset is connected, we'll be notified later if it's not.
    iRadioSettings->SetHeadsetConnected();
    

    // Create P&S interface.
    iPubSub = CFMRadioPubSub::NewL();
    
    // Publish antenna status as connected, we'll be notified later if it's not.
    iPubSub->PublishAntennaStatusL( EFMRadioPSHeadsetConnected );
    
    iPubSub->PublishFrequencyDecimalCountL( 
            TFMRadioPSFrequencyDecimalCount( iRadioSettings->DecimalCount() ) );

    FTRACE(FPrint(_L("CRadioEngine::ConstructL() End ")));
    }
    
// ----------------------------------------------------
// CRadioEngine::ConnectLineL
// Connects etel server
// ----------------------------------------------------
//
void CRadioEngine::ConnectLineL()
	{
	User::LeaveIfError( iTelServer.Connect() );

	iTelServer.GetTsyName( 0, iTsyName );

	User::LeaveIfError( iTelServer.LoadPhoneModule( iTsyName ) );
	TInt numberOfPhones( 0 );
	User::LeaveIfError( iTelServer.EnumeratePhones( numberOfPhones ) );

	RTelServer::TPhoneInfo phoneInfo;

	// Only possible thing to do is leave. We definately need phone.
	User::LeaveIfError( iTelServer.GetPhoneInfo( 0, phoneInfo ) );
	    
	// Only possible thing to do is leave. We definately need phone.
	User::LeaveIfError( iPhone.Open( iTelServer, phoneInfo.iName ));
	

	RPhone::TLineInfo lineInfo;
	
	User::LeaveIfError( iPhone.GetLineInfo( 0, lineInfo ) );
	
	User::LeaveIfError( iLine.Open( iPhone, lineInfo.iName ) );
	
	ilineConstructed = ETrue;
	}

// ----------------------------------------------------
// CRadioEngine::NewL
// Two-phased class constructor.
// ----------------------------------------------------
//
EXPORT_C CRadioEngine* CRadioEngine::NewL(
    MRadioEngineStateChangeCallback& aCallback )
    {
    CRadioEngine* self = new (ELeave) CRadioEngine( aCallback );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CRadioEngine::~CRadioEngine
// Destructor of CRadioEngine class.
// ----------------------------------------------------
////
EXPORT_C CRadioEngine::~CRadioEngine()
    {
    FTRACE(FPrint(_L("CRadioEngine::~CRadioEngine()")));
    if ( iCentralRepositoryHandler) 
        {
        iCentralRepositoryHandler->SaveEngineSettings();
        delete iCentralRepositoryHandler;
        }
        
    delete iPubSub;
    delete iHeadsetObserver;
	
	RadioOff();
	        
	delete iNetworkInfoListener;     
	delete iSystemEventDetector;      
    delete iPresetUtility;
    delete iRadioSettings;
    delete iStateHandler;

    if ( iAudioOutput )
        {
        TRAP_IGNORE(iAudioOutput->SetAudioOutputL(CAudioOutput::EPrivate));
        delete iAudioOutput;
        }

    if ( iRdsReceiver )
        {
        iRdsReceiver->RemoveObserver( this );
        }

    delete iRdsReceiver;
    
    if ( iFmTunerUtility )
        {
        iFmTunerUtility->Close();
        }

    if ( iPlayerUtility )
        {
        iPlayerUtility->Close();
        }

    delete iRadioUtility;

    #ifndef __ACCESSORY_FW
    if (iDosServerObserver)
        {
        iDosServerObserver->SetAudioRouting(EFMRadioOutputHeadset);
        delete iDosServerObserver;
        }
    #endif
        
	if ( iFMRadioEngineResourceOffset > 0 )
        {
        CCoeEnv::Static()->DeleteResourceFile( iFMRadioEngineResourceOffset );
        }
         
	if ( ilineConstructed )
		{	        
		iLine.Close();
	    iPhone.Close();
	    iTelServer.UnloadPhoneModule( iTsyName );    
	    iTelServer.Close();
	    }
    }

// ----------------------------------------------------
// CRadioEngine::InitializeRadio
// Set radio settings to their default values.
// Returns: None
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::InitializeRadio()
      {
      FTRACE(FPrint(_L("CRadioEngine::InitializeRadio()")));
      iInitializeRadioRequestExists = ETrue;
      Tune( iRadioSettings->Frequency() );
#ifdef __WINS__
      // MrftoRequestTunerControlComplete never completes in WINS, so initialize RdsReceiver here
      TRAP_IGNORE( iRdsReceiver->InitL( *iRadioUtility, iPubSub ) );
#endif
      }

// ----------------------------------------------------
// CRadioEngine::RadioOn
// Start the radio.
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::RadioOn()
    {
    FTRACE(FPrint(_L("CRadioEngine::RadioOn()")));
    if ( !iRadioSettings->IsRadioOn() )
        {
        FTRACE(FPrint(_L("CRadioEngine::RadioOn() - Turning Radio on, calling play")));
        iRadioSettings->SetRadioOn();
        iPlayerUtility->Play();
        
        if ( iPubSub )
            {
            TRAP_IGNORE( iPubSub->PublishStateL( ETrue ) );
            }
       }
    }

// ----------------------------------------------------
// CRadioEngine::RadioOff
// Stops the radio.
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::RadioOff()
    {
    FTRACE(FPrint(_L("CRadioEngine::RadioOff()")));
    if ( iStateHandler )
        {
        iStateHandler->Cancel();
        }

    iCurrentRadioState = EStateRadioOff;

    if( iFmTunerUtility &&
        iRadioSettings &&
        iRadioSettings->RadioMode() == ERadioTunerMode )
        {
        CancelScan();
        }

    if ( iPlayerUtility &&
         iPlayerUtility->PlayerState() == ERadioPlayerPlaying )
        {
        iPlayerUtility->Stop();
        }
    
    if ( iPubSub )
        {
        TRAP_IGNORE( iPubSub->PublishStateL( EFalse ) );
        }
    }

// ----------------------------------------------------
// CRadioEngine::IsRadioOn
// Returns the radio status.
// ----------------------------------------------------
//
EXPORT_C TBool CRadioEngine::IsRadioOn()
	{
	return iRadioSettings->IsRadioOn();
	}

// ----------------------------------------------------
// CRadioEngine::IsOfflineProfileL
// Checks if offine profile is current active profile
// Returns: true/false 
// ----------------------------------------------------
//
EXPORT_C TBool CRadioEngine::IsOfflineProfileL()
	{
	return iCentralRepositoryHandler->IsOfflineProfileActiveL();
	}

// ----------------------------------------------------
// CRadioEngine::Tune
// Tune to the specified frequency
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::Tune( TInt aFrequency, TRadioMode aRadioMode )
    {
    FTRACE(FPrint(_L("CRadioEngine::Tune()")));
    iTempFrequency = aFrequency;

    iRadioSettings->SetRadioMode( aRadioMode );
    if ( iTunerControl && iRadioSettings->IsHeadsetConnected() )
        {
        iFmTunerUtility->SetFrequency( aFrequency );
        }
    else
        {
        StoreAndPublishFrequency( aFrequency );
        }
    if ( iPubSub )
    	{
        TRAP_IGNORE
            (
            iPubSub->PublishFrequencyL( aFrequency );
            if( aRadioMode == ERadioTunerMode )
                {
                iPubSub->PublishChannelNameL( KNullDesC );
                iPubSub->PublishChannelL( KErrNotFound );
                }
            )
    	}
    }

// ----------------------------------------------------
// CRadioEngine::SetTunerModeOn
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::SetTunerModeOn()
    {
    FTRACE(FPrint(_L("CRadioEngine::SetTunerModeOn()")));
    
    iRadioSettings->SetRadioMode( ERadioTunerMode );
    
    if ( iPubSub )
        {
        TRAP_IGNORE
            (
            iPubSub->PublishChannelNameL( KNullDesC );
            iPubSub->PublishChannelL( KErrNotFound );
            )
        }
    }

// ----------------------------------------------------
// CRadioEngine::ScanUp
// Scan up to the next available frequency
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::ScanUp()
    {
    iRadioSettings->SetRadioMode(ERadioTunerMode);
    iFmTunerUtility->StationSeek( ETrue );
    TRAP_IGNORE
        (
        iTuningState = EFMRadioPSTuningStarted;
        iPubSub->PublishTuningStateL( iTuningState );
        iPubSub->PublishChannelNameL( KNullDesC );
        iPubSub->PublishChannelL( KErrNotFound );
        )
    }

// ----------------------------------------------------
// CRadioEngine::ScanDown
// Scan down to the last available frequency
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::ScanDown()
    {
    iRadioSettings->SetRadioMode(ERadioTunerMode);
    iFmTunerUtility->StationSeek( EFalse );
    TRAP_IGNORE
        (
        iTuningState = EFMRadioPSTuningStarted;
        iPubSub->PublishTuningStateL( iTuningState );
        iPubSub->PublishChannelNameL( KNullDesC );
        iPubSub->PublishChannelL( KErrNotFound );
        )
    }

// ----------------------------------------------------
// CRadioEngine::CancelScan
// Cancel previously requested scan, and return to the
// already tuned frequency
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::CancelScan()
    {
    if( iFmTunerUtility )
        {
        iFmTunerUtility->CancelStationSeek();
        iTuningState = EFMRadioPSTuningUninitialized;
        TRAP_IGNORE(iPubSub->PublishTuningStateL( iTuningState ));
        }
    }

// ----------------------------------------------------
// CRadioEngine::IsHeadsetConnected
// Check whether the headset is currently connected
// Returns: true/false
// ----------------------------------------------------
//
EXPORT_C TBool CRadioEngine::IsHeadsetConnected() const
    {
    return iRadioSettings->IsHeadsetConnected();
    }

// ----------------------------------------------------
// CRadioEngine::IsFlightmodeEnabled
// Check whether flight mode is currently enabled
// Returns: true/false
// ----------------------------------------------------
//
EXPORT_C TBool CRadioEngine::IsFlightModeEnabled() const
    {
    return iRadioSettings->IsFlightModeEnabled();
    }

// ----------------------------------------------------
// CRadioEngine::SetMuteOn
// Sets the audio mute state
// Returns: None
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::SetMuteOn(
    TBool aMuteOn)  // a flag indicating if set mute on
    {
    FTRACE(FPrint(_L("CRadioEngine::SetMuteOn()")));
    TInt error = KErrNone;
    if ( IsMuteOn()  && (!aMuteOn) )
        {
        error = iPlayerUtility->Mute( EFalse );
        if( !error )
            {
            FTRACE(FPrint(_L("CRadioEngine::SetMuteOn() setting mute off")));
            iRadioSettings->SetMuteOff();
            TRAP_IGNORE(iPubSub->PublishRadioMuteStateL(EFalse));
            }
        }
    else if ( !IsMuteOn() && aMuteOn )
        {
        error = iPlayerUtility->Mute( ETrue );
        if( !error )
            {
            FTRACE(FPrint(_L("CRadioEngine::SetMuteOn() setting mute on")));
            iRadioSettings->SetMuteOn();
            TRAP_IGNORE(iPubSub->PublishRadioMuteStateL(ETrue));
            }
        }
    iStateHandler->Callback( MRadioEngineStateChangeCallback::EFMRadioEventSetMuteState, error );
    }


// ----------------------------------------------------
// CRadioEngine::IsMuteOn
// Gets the audio mute state
// Returns: true/false
// ----------------------------------------------------
//
EXPORT_C TBool CRadioEngine::IsMuteOn() const
    {
    return iRadioSettings->IsMuteOn();
    }

// ----------------------------------------------------
// CRadioEngine::GetPresetIndex
// Retrieve the currently selected channel
// Returns: the preset channel index
// ----------------------------------------------------
//
EXPORT_C TInt CRadioEngine::GetPresetIndex() const
    {
    return iRadioSettings->CurrentPreset();
    }


// ----------------------------------------------------
// CRadioEngine::SetPresetFrequency
// Update the channel located at the index to use the frequency specified.
// Returns: None
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::SetPresetNameFrequencyL(
    TInt aIndex, // preset channel index
    const TStationName& aStationName,
    TInt aFrequency) // preset channel frequency
    {
    iPresetUtility->SetPresetL(aIndex, aStationName, aFrequency);
    if( aIndex == iRadioSettings->CurrentPreset() )
        {
        iPubSub->PublishChannelNameL( aStationName );
        if( aFrequency == 0 )
            {
            // Current preset was deleted
            iPubSub->PublishChannelL( KErrNotFound );
            }
        }
    iPubSub->PublishChannelDataChangedL( aIndex );
    }

// ----------------------------------------------------
// CRadioEngine::GetPresetFrequency
// Retrieve the channel frequency at the index specified
// Returns: tmpFrequency: preset frequency
// ----------------------------------------------------
//
EXPORT_C TInt CRadioEngine::GetPresetFrequencyL(
    TInt aIndex) const  // preset channel index
    {
    TInt tmpFrequency = KDefaultRadioFrequency;
    TStationName tmpStationName;

    iPresetUtility->GetPresetL( aIndex, tmpStationName, tmpFrequency );

    FTRACE(FPrint(_L("CRadioEngine::GetPresetFrequencyL()  aIndex %d"), aIndex));
    FTRACE(FPrint(_L("CRadioEngine::GetPresetFrequencyL()  Freq %d"), tmpFrequency));
    FTRACE(FPrint(_L("CRadioEngine::GetPresetFrequencyL()  Name %S"), &tmpStationName));
    return tmpFrequency;
    }

// ----------------------------------------------------
// CRadioEngine::GetPresetName
// Retrieve the channel name at the index specified
// Returns: tmpStationName: the channel name
// ----------------------------------------------------
//
EXPORT_C CRadioEngine::TStationName& CRadioEngine::GetPresetNameL( TInt aIndex )
	{		
    TInt tmpFrequency;

    iPresetUtility->GetPresetL(aIndex, iStationName, tmpFrequency);
    FTRACE(FPrint(_L("CRadioEngine::GetPresetFrequencyL()  Freq %d"),tmpFrequency));
    FTRACE(FPrint(_L("CRadioEngine::GetPresetFrequencyL()  Name %S"),&iStationName));
    return iStationName; 
    }

// ----------------------------------------------------
// CRadioEngine::GetPresetNameAndFrequencyL
// get preset name frequency from the preset utility
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::GetPresetNameAndFrequencyL( TInt aIndex, TDes& aName, TInt& aFrequency )
    {
    TInt tmpFrequency;
    TStationName tmpStationName;
    
    iPresetUtility->GetPresetL( aIndex, tmpStationName, tmpFrequency );

    aFrequency = tmpFrequency;
    aName = tmpStationName;
    }

// ----------------------------------------------------
// CRadioEngine::GetTunedFrequency
// Retrieves the current frequency.
// Returns: channel frequency
// ----------------------------------------------------
//
EXPORT_C TInt CRadioEngine::GetTunedFrequency() const
    {
    return iRadioSettings->Frequency();
    }

// ----------------------------------------------------
// CRadioEngine::SetVolume
// Sets the volume level of the FM radio
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::SetVolume(
    TInt aVolume )
    {
    FTRACE(FPrint(_L("CRadioEngine::SetVolume( %d )"), aVolume ));
    
    if ( aVolume < 0 )
        {
        aVolume = 0;
        }
    else if ( aVolume > KFMRadioNumberOfVolumeSteps )
        {
        aVolume = KFMRadioNumberOfVolumeSteps;
        }
    
    TInt playerVolume = ( iUiVolumeFactor * aVolume );
    
    FTRACE(FPrint(_L("CRadioEngine::SetVolume() playerVolume %d"), playerVolume ));
    
    TInt error = iPlayerUtility->SetVolume( playerVolume );
    
    if ( !error )
        {
        if ( iRadioSettings->AudioOutput() == EFMRadioOutputHeadset )
            {
            iRadioSettings->SetHeadsetVolume( aVolume );
            iCentralRepositoryHandler->SaveHeadsetVolumeSetting();
            }
        else
            {
             iRadioSettings->SetSpeakerVolume( aVolume );
             iCentralRepositoryHandler->SaveSpeakerVolumeSetting();
            }
        }
    else
        {
        iStateHandler->Callback( MRadioEngineStateChangeCallback::EFMRadioEventVolumeUpdated, error );
        }
    }

// ----------------------------------------------------
// CRadioEngine::GetRadioMode
// Retrieves the current radio mode (tuner or preset).
// Returns: radio mode
// ----------------------------------------------------
//
EXPORT_C CRadioEngine::TRadioMode CRadioEngine::GetRadioMode() const
    {
    return iRadioSettings->RadioMode();
    }

// ----------------------------------------------------
// CRadioEngine::GetVolume
// Gets the volumelevel.
// Returns: volume
// ----------------------------------------------------
//
EXPORT_C TInt CRadioEngine::GetVolume() const
    {
    if ( iRadioSettings->AudioOutput() == EFMRadioOutputHeadset )
        {
        return iRadioSettings->HeadsetVolume();
        }
    else
        {
        return iRadioSettings->SpeakerVolume();
        }
    }

// ----------------------------------------------------
// CRadioEngine::GetMaxVolume
// Gets the max volumelevel.
// Returns: max volume
// ----------------------------------------------------
//
EXPORT_C TInt CRadioEngine::GetMaxVolume() const
    {
    TInt maxVolume;
    iPlayerUtility->GetMaxVolume( maxVolume );
    return maxVolume;
    }

// ----------------------------------------------------
// CRadioEngine::GetAudioMode
// Gets the current audio mode.
// ----------------------------------------------------
//
EXPORT_C TInt CRadioEngine::GetAudioMode() const
    {
    return iRadioSettings->AudioMode();
    }

// ----------------------------------------------------
// CRadioEngine::SetAudioMode
// Set the radio's audio mode (stereo or mono)
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::SetAudioMode( const TFMRadioAudioMode aAudioMode )
    {
    FTRACE(FPrint(_L("CRadioEngine::SetAudioMode")));
    TBool forcedMono = EFalse;
    TInt res = KErrNone;
    TFMRadioAudioMode audioMode = iRadioSettings->AudioMode();
    if (audioMode != aAudioMode)
        {
        if ( aAudioMode == EFMRadioMono )
            {
            forcedMono = ETrue;
            }
        res = iFmTunerUtility->ForceMonoReception( forcedMono );
        if ( res )
            {
            iRadioSettings->SetAudioMode( aAudioMode );
            }
        }
    iStateHandler->Callback( MRadioEngineStateChangeCallback::EFMRadioEventSetAudioMode, res );
    }

// ----------------------------------------------------
// CRadioEngine::GetAudioOutput
// Retrieve the current audio output
// Returns: audio output
// ----------------------------------------------------
//
EXPORT_C CRadioEngine::TFMRadioAudioOutput CRadioEngine::GetAudioOutput() const
    {
    return iRadioSettings->AudioOutput();
    }

// ----------------------------------------------------
// CRadioEngine::SetAudioOutput
// Sets the audio Output
// Returns: None
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::SetAudioOutput( const TFMRadioAudioOutput aAudioOutput )
    {
    FTRACE(FPrint(_L("CRadioEngine::SetAudioOutput() Output = %d"), aAudioOutput));
    TInt tempError = KErrNone;
    // use mute here to avoid any audio peaks during output change
    SetMuteOn( ETrue );

#ifndef __ACCESSORY_FW

    tempError = iDosServerObserver->SetAudioRouting(aAudioOutput);
    FTRACE(FPrint(_L("CRadioEngine::SetAudioOutput() tempError = %d"), tempError));

#else

    CAudioOutput::TAudioOutputPreference outputPreference = CAudioOutput::EPrivate;

    if ( EFMRadioOutputHeadset == aAudioOutput )
        {
        FTRACE(FPrint(_L("CRadioEngine::SetAudioOutput() Headset is set to Output")));
        outputPreference = CAudioOutput::EPrivate;
        }
    else if ( EFMRadioOutputIHF == aAudioOutput )
        {
        FTRACE(FPrint(_L("CRadioEngine::SetAudioOutput() IHF is set to Output")));
        
        outputPreference = CAudioOutput::EPublic;
        }

#ifndef __WINS
    if ( iAudioOutput )
        {
        TRAP( tempError, iAudioOutput->SetAudioOutputL( outputPreference ) );
        }
#endif

#endif

    if ( KErrNone == tempError )
        {        
        if ( !iHeadsetObserver->IsHeadsetAccessoryConnected() )
        	{
        	iRadioSettings->SetAudioOutput( EFMRadioOutputIHF );
        	}
        else
        	{
        	iRadioSettings->SetAudioOutput( aAudioOutput );
        	}
        }

    iStateHandler->Callback( MRadioEngineStateChangeCallback::EFMRadioEventSetAudioOutput, tempError );
    }

// ----------------------------------------------------
// CRadioEngine::SetRdsAfSearchEnable
// Sets the rds af search enabled
// Returns: None
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::SetRdsAfSearchEnable( TBool aEnabled )
	{
	TRAP_IGNORE( iRdsReceiver->SetAutomaticSwitchingL( aEnabled ) );	
	aEnabled ? iRadioSettings->SetRdsAfSearchEnabled() : iRadioSettings->SetRdsAfSearchDisabled();
	}

// ----------------------------------------------------
// CRadioEngine::RadioSettings
// ----------------------------------------------------
//
EXPORT_C TRadioSettings& CRadioEngine::RadioSettings( )
	{
	return *iRadioSettings;
	}

// ----------------------------------------------------
// CRadioEngine::TunePreset
// Tunes to the chosen channel.
// Returns: None
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::TunePresetL( TInt aIndex )
    {
    FTRACE(FPrint(_L("CRadioEngine::TunePresetL()")));
    iRadioSettings->SetCurrentPreset( aIndex );
    TInt tmpFrequency = 0;// changed back to zero
    TStationName name;

    iPresetUtility->GetPresetL( aIndex, name, tmpFrequency );
    // Should not tune if the radio is not on.
    if ( iRadioSettings->IsRadioOn())
        {
        FTRACE(FPrint(_L("CRadioEngine::TunePresetL() inside if")));
        iTunePresetRequested = ETrue;
        iTempFrequency = tmpFrequency;
        Tune(tmpFrequency, ERadioPresetMode);
        }
    else
        {
        StoreAndPublishFrequency( tmpFrequency );
        }
    iPubSub->PublishChannelNameL( name );
    iPubSub->PublishChannelL( aIndex );
    iRadioSettings->SetRadioMode(ERadioPresetMode);
    FTRACE(FPrint(_L("CRadioEngine::TunePresetL() end %d"),tmpFrequency));
    }

// ----------------------------------------------------
// CRadioEngine::5
// Return min and max freq for the specified range (band)
// ----------------------------------------------------
//
EXPORT_C TInt CRadioEngine::GetFrequencyBandRange(
    TInt& aBottomFrequency,
    TInt& aTopFrequency)
    {
    TBool err = KErrNone;
    
    aBottomFrequency = iCentralRepositoryHandler->MinFrequency() * KFMRadioFreqMultiplier;
	aTopFrequency = iCentralRepositoryHandler->MaxFrequency() * KFMRadioFreqMultiplier;

    return err;
    }

// ----------------------------------------------------
// CRadioEngine::IsInCall
// ----------------------------------------------------
//
EXPORT_C TBool CRadioEngine::IsInCall() const
    {
    return iInCall;
    }

// ---------------------------------------------------------
// CRadioEngine::FrequencySetByRdsAf
// ---------------------------------------------------------
//
EXPORT_C TBool CRadioEngine::FrequencySetByRdsAf() const
    {
    return iFrequencySetByRdsAf;
    }

// ---------------------------------------------------------
// CRadioEngine::RdsReceiver
// ---------------------------------------------------------
//
EXPORT_C CFMRadioRdsReceiverBase& CRadioEngine::RdsReceiver()
    {
    return *iRdsReceiver;
    }

// ----------------------------------------------------
// CRadioEngine::HandleFlightModeEnabled
// Switches the engine into flight mode, and notifies the UI of
// the change in status.
// ----------------------------------------------------
//
void CRadioEngine::HandleFlightModeEnabled()
    {
    FTRACE(FPrint(_L("CRadioEngine::HandleFlightModeEnabled()")));
    iRadioSettings->SetFlightMode(ETrue);
    HandleCallback(MRadioEngineStateChangeCallback::EFMRadioEventFlightModeEnabled, KErrNone);
    }

// ----------------------------------------------------
// CRadioEngine::HandleFlightModeDisabled
// Switches the engine out of flight mode, and notifies the UI of
// the change in status
// ----------------------------------------------------
//
void CRadioEngine::HandleFlightModeDisabled()
    {
    FTRACE(FPrint(_L("CRadioEngine::HandleFlightModeDisabled()")));
    iRadioSettings->SetFlightMode(EFalse);
    HandleCallback(MRadioEngineStateChangeCallback::EFMRadioEventFlightModeDisabled, KErrNone);
    }


// ----------------------------------------------------
// CRadioEngine::HandleCallback
// Calls the User Interface callback function, indicating the event code and
// error code.
// ----------------------------------------------------
//
void CRadioEngine::HandleCallback(TInt aEventCode, TInt aCode )
    {
    FTRACE(FPrint(_L("CRadioEngine::HandleCallback(aEventCode %d, aCode %d "), aEventCode, aCode ) );
    iCallback.HandleRadioEngineCallBack((MRadioEngineStateChangeCallback::TFMRadioNotifyEvent)aEventCode, aCode);
    }

// ----------------------------------------------------
// CRadioEngine::HandleHeadsetButtonPress
// Notifies UI that the headset button has been pressed
// ----------------------------------------------------
//
void CRadioEngine::HandleHeadsetButtonPress()
    {
    FTRACE(FPrint(_L("CRadioEngine::HandleHeadsetButtonPress()")));
    iStateHandler->Callback(MRadioEngineStateChangeCallback::EFMRadioEventButtonPressed, KErrNone);
    }

// ----------------------------------------------------
// CRadioEngine::HeadsetAccessoryConnectedCallbackL
// ----------------------------------------------------
//
void CRadioEngine::HeadsetAccessoryConnectedCallbackL()
    {
    FTRACE(FPrint(_L("CRadioEngine::HeadsetAccessoryConnectedCallbackL")));
    // forward volume changes to headset
    SetAudioOutput( EFMRadioOutputHeadset );
    }

// ----------------------------------------------------
// CRadioEngine::HeadsetAccessoryDisconnectedCallbackL
// ----------------------------------------------------
//
void CRadioEngine::HeadsetAccessoryDisconnectedCallbackL()
    {
    FTRACE(FPrint(_L("CRadioEngine::HeadsetAccessoryDisconnectedCallbackL")));
    // forward volume changes to speakers
    SetAudioOutput( EFMRadioOutputIHF );
    }

// ----------------------------------------------------
// CRadioEngine::MrftoRequestTunerControlComplete
// ----------------------------------------------------
//
void CRadioEngine::MrftoRequestTunerControlComplete(
    TInt aError )
    {
    FTRACE(FPrint(_L("CRadioEngine::MrftoRequestTunerControlComplete() - Error = %d"), aError));

    if ( aError == KErrNone )
        {
        iTunerControl = ETrue;
        // Headset must be connected
        iRadioSettings->SetHeadsetConnected();
        iFmTunerUtility->GetCapabilities( iTunerCaps );

        if( iTunerCaps.iTunerFunctions & TFmTunerCapabilities::ETunerRdsSupport )
	        {
	        TRAP_IGNORE( iRdsReceiver->InitL( *iRadioUtility, iPubSub ) )
	        }
        
		TFmRadioFrequencyRange japanFreqRange = EFmRangeJapan;
		
		if ( TunerFrequencyRangeForRegionId( RegionId() ) == japanFreqRange )
			{
			// region is Japan so we need to change the freq range from the default
			iFmTunerUtility->SetFrequencyRange( japanFreqRange );		
			}
		else
			{
			// allready on correct freq range
			HandleCallback(MRadioEngineStateChangeCallback::EFMRadioEventTunerReady, KErrNone );
			}		        
        }
    else if ( aError == KFmRadioErrAntennaNotConnected )
        {
        FTRACE(FPrint(_L("CRadioEngine::MrftoRequestTunerControlComplete() - KFmRadioErrAntennaNotConnected")));
        iRadioSettings->SetHeadsetDisconnected();
        HandleCallback(MRadioEngineStateChangeCallback::EFMRadioEventHeadsetDisconnected, KErrNone );
        }
    else if ( aError == KErrAlreadyExists )
        {
        iTunerControl = ETrue;
        // Headset must be connected
        iRadioSettings->SetHeadsetConnected();
        iFmTunerUtility->GetCapabilities( iTunerCaps );
        TInt frequency;
        iFmTunerUtility->GetFrequency( frequency );
        StoreAndPublishFrequency( frequency );
        HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventTune, KErrNone );
        }
    else if ( aError == KFmRadioErrFmTransmitterActive )
        {
        iTunerControl = EFalse;
        HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventFMTransmitterOn, KErrNone );
        }
    else if ( aError == KFmRadioErrOfflineMode )
        {
        iTunerControl = EFalse;
        HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventFlightModeEnabled, KErrNone );
        }
    else
        {
        //pass
        }
    }

// ----------------------------------------------------
// CRadioEngine::MrftoSetFrequencyRangeComplete
//
// ----------------------------------------------------
//
void CRadioEngine::MrftoSetFrequencyRangeComplete(
    TInt aError )
    {
    if ( aError == KErrNone )
    	{
    	HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventTunerReady, KErrNone );
    	}
    	  	
    FTRACE(FPrint(_L("CRadioEngine::MrftoSetFrequencyRangeComplete() - error = %d"), aError));
    }

// ----------------------------------------------------
// CRadioEngine::MrftoSetFrequencyComplete
// ----------------------------------------------------
//
void CRadioEngine::MrftoSetFrequencyComplete(
    TInt aError )
    {
    FTRACE(FPrint(_L("CRadioEngine::MrftoSetFrequencyComplete() - aError = %d"), aError));


    if ( !aError )
        {
        iRdsReceiver->ClearRdsInformation();
        StoreAndPublishFrequency( iTempFrequency );
        if ( iTunePresetRequested )
            {
            iCentralRepositoryHandler->SaveEngineSettings();
            iTunePresetRequested = EFalse;
            }
        HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventTune, aError );

        if ( iInitializeRadioRequestExists )
            {
            iInitializeRadioRequestExists = EFalse;
            HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventFMRadioInitialized,  KErrNone );
            }
        }
    else
        {
        iInitializeRadioRequestExists = EFalse;

        FTRACE(FPrint(_L("CRadioEngine::MrftoSetFrequencyComplete() - Sending event to UI")));
        HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventTune, aError );
        }

    }

// ----------------------------------------------------
// CRadioEngine::MrftoStationSeekComplete
//
// ----------------------------------------------------
//
void CRadioEngine::MrftoStationSeekComplete(
    TInt aError,
    TInt aFrequency )
    {
    FTRACE(FPrint(_L("CRadioEngine::MrftoStationSeekComplete() - Frequency = %d"), aFrequency));
    FTRACE(FPrint(_L("CRadioEngine::MrftoStationSeekComplete() - error = %d"), aError));

    if ( !aError )
        {
        StoreAndPublishFrequency( aFrequency );

        iRdsReceiver->ClearRdsInformation();
        StoreAndPublishFrequency( aFrequency );
        if ( iTunePresetRequested )
            {
            iCentralRepositoryHandler->SaveEngineSettings();
            iTunePresetRequested = EFalse;
            }
        iTuningState = EFMRadioPSTuningUninitialized;
        // Process the callback first because it might change the tuning state and we must avoid switching it back and forth
        HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventTune, aError );
        TRAP_IGNORE(iPubSub->PublishTuningStateL( iTuningState ));

        if ( iInitializeRadioRequestExists )
            {
            iInitializeRadioRequestExists = EFalse;
            HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventFMRadioInitialized,  KErrNone );
            }
        }
    else
        {
        iInitializeRadioRequestExists = EFalse;
        if ( aError == KFmRadioErrAntennaNotConnected )
            {            
            iRadioSettings->SetHeadsetDisconnected();
            }
        FTRACE(FPrint(_L("CRadioEngine::MrftoStationSeekComplete() - Sending event to UI")));
        iTuningState = EFMRadioPSTuningUninitialized;
        HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventTune, aError );
        TRAP_IGNORE(iPubSub->PublishTuningStateL( iTuningState ));
        }
    }


// ----------------------------------------------------
// CRadioEngine::MrftoFmTransmitterStatusChange
// The FM tuner is turned off when FM transmitter is on.
// ----------------------------------------------------
//
void CRadioEngine::MrftoFmTransmitterStatusChange(
    TBool aActive )
    {
    FTRACE(FPrint(_L("CRadioEngine::MrftoFmTransmitterStatusChange() - Active = %d"), aActive));
    if ( aActive )
        {
        // loses tuner control
        iTunerControl = EFalse;
        }
    }


// ----------------------------------------------------
// CRadioEngine::MrftoAntennaStatusChange
// Called when antenna status changes.
// ----------------------------------------------------
//
void CRadioEngine::MrftoAntennaStatusChange(
    TBool aAttached )
    {
    FTRACE(FPrint(_L("CRadioEngine::MrftoAntennaStatusChange() - Antenna Status = %d"), aAttached));
    if ( aAttached )
        {
        if ( !iTunerControl )
            {
            iFmTunerUtility->RequestTunerControl();
            }
        iRadioSettings->SetHeadsetConnected();
        TRAP_IGNORE( iPubSub->PublishAntennaStatusL( EFMRadioPSHeadsetConnected ) );
        HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventHeadsetReconnected, KErrNone );
        }
    else
        {
        iRadioSettings->SetHeadsetDisconnected();
        TRAP_IGNORE( iPubSub->PublishAntennaStatusL( EFMRadioPSHeadsetDisconnected ) );
        HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventHeadsetDisconnected, KErrNone );
        }
    }


// ----------------------------------------------------
// CRadioEngine::FlightModeChanged
// Called when offline mode status changes
// ----------------------------------------------------
//
void CRadioEngine::MrftoOfflineModeStatusChange(
    TBool aOfflineMode )
    {
    FTRACE(FPrint(_L("CRadioEngine::MrftoOfflineModeStatusChange() - Offline Mode = %d"), aOfflineMode));

    if( aOfflineMode )
        {
        HandleFlightModeEnabled();
        }
    else
        {
        if ( !iTunerControl )
            {
            iFmTunerUtility->RequestTunerControl();
            }
        HandleFlightModeDisabled();
        }
    }

// ----------------------------------------------------
// CRadioEngine::MrftoFrequencyRangeChange
//
// ----------------------------------------------------
//
void CRadioEngine::MrftoFrequencyRangeChange(
    TFmRadioFrequencyRange FDEBUGVAR( aNewRange ) )
    {
    FTRACE( FPrint(_L("CRadioEngine::MrftoFrequencyRangeChange() - Band = %d"), aNewRange ) );    
    }

 // ----------------------------------------------------
// CRadioEngine::MrftoFrequencyChange
// Called when the tuned frequency changes
// ----------------------------------------------------
//
void CRadioEngine::MrftoFrequencyChange(
    TInt aNewFrequency )
    {
    FTRACE(FPrint(_L("CRadioEngine::MrftoFrequencyChange() - New Frequency = %d"), aNewFrequency));
	
    if ( iRadioSettings->Frequency() != aNewFrequency )
        {
        iRdsReceiver->ClearRdsInformation();
        StoreAndPublishFrequency( aNewFrequency );
        HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventTune, KErrNone );
        }
    }

// ----------------------------------------------------
// CRadioEngine::MTsoForcedMonoChanged
// Called when a client enables/disabled forced mono reception
// ----------------------------------------------------
//
void CRadioEngine::MrftoForcedMonoChange(
    TBool aForcedMono )
    {
    FTRACE(FPrint(_L("CRadioEngine::MrftoForcedMonoChange() - Forced Mono = %d"), aForcedMono));
    if ( aForcedMono )
        {
        iRadioSettings->SetAudioMode( EFMRadioMono );
        }
    else
        {
        iRadioSettings->SetAudioMode( EFMRadioStereo );
        }
    HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventSetAudioMode, KErrNone );
    }

// ----------------------------------------------------
// CRadioEngine::MrftoSquelchChange
//
// error code.
// ----------------------------------------------------
//
void CRadioEngine::MrftoSquelchChange(
    TBool /*aSquelch*/ )
    {
    }


// ----------------------------------------------------
// CRadioEngine::MrpoStateChange This is Tuner State
// Called when the state of the tuner changes
// ----------------------------------------------------
//
void CRadioEngine::MrpoStateChange(
    TPlayerState aState,
    TInt aError )
    {
    FTRACE(FPrint(_L("CRadioEngine::MrpoStateChange() - State Change = %d, Error = %d"), aState, aError));

    if ( !aError )
        {
        if ( aState == ERadioPlayerPlaying )
            {
            iRadioSettings->SetRadioOn();
            iCurrentRadioState = EStateRadioOn;
            FTRACE(FPrint(_L("CRadioEngine::MrpoStateChange() Resource Available Error = %d"), aError ));
            iRdsReceiver->StartReceiver();
            HandleCallback(MRadioEngineStateChangeCallback::EFMRadioEventRadioOn, KErrNone);
            }
        else
            {
            iRdsReceiver->StopReceiver();
            iRadioSettings->SetRadioOff();
            iCurrentRadioState = EStateRadioOff;
            HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventRadioOff, KErrNone );
            }
        }
    else
        {
        if ( aState == ERadioPlayerIdle && iRadioSettings->IsRadioOn() )
            {
            MRadioEngineStateChangeCallback::TFMRadioNotifyEvent ret = 
            	MRadioEngineStateChangeCallback::EFMRadioEventAudioResourcePaused;
            iCurrentRadioState = EStateRadioOff;
            iRdsReceiver->StopReceiver();
            iRadioSettings->SetRadioOff();
            if ( ( aError == KErrDied ) ||
                    ( aError == KErrAccessDenied ) ||
                    ( aError == KErrNotReady ) ||
                    ( aError == KErrInUse ) )
                {
                TInt callState = KErrUnknown;
                
                RMobileCall::TMobileCallStatus linestatus;
                
                if ( ilineConstructed )
					{
                	iLine.GetMobileLineStatus( linestatus );
                	}
                FTRACE(FPrint(_L("CRadioEngine::MrpoStateChange() linestatus = %d"), linestatus));
                
                TInt err = RProperty::Get(KPSUidCtsyCallInformation, KCTsyCallState, callState);
                FTRACE(FPrint(_L("CRadioEngine::MrpoStateChange() callState = %d"), callState));
                                
                // check status from line
                if ( linestatus != RMobileCall::EStatusIdle &&
                     linestatus != RMobileCall::EStatusUnknown ||
                     iSystemEventDetector->IsCallActive() )
                    {
                    FTRACE(FPrint(_L("CRadioEngine::MrpoStateChange() EFMRadioEventCallStarted")  ) );
                    iInCall = ETrue;
                    ret = MRadioEngineStateChangeCallback::EFMRadioEventCallStarted;
                    }
                }
            else if ( aError == KErrHardwareNotAvailable ) // Radio is interrupted by FM Transmitter activation.
                {
                ret = MRadioEngineStateChangeCallback::EFMRadioEventFMTransmitterOn;
                }
            HandleCallback( ret, aError );
            }
        }
    if( iRadioSettings->IsRadioOn() )
        {
        TRAP_IGNORE( iPubSub->PublishStateL( ETrue ) )
        }
    else
        {
        TRAP_IGNORE
            (
            iTuningState = EFMRadioPSTuningUninitialized;
            iPubSub->PublishTuningStateL( iTuningState );
            iPubSub->PublishStateL( EFalse );
            )
        }
    }

// ----------------------------------------------------
// CRadioEngine::MrpoVolumeChange
//
// ----------------------------------------------------
//
void CRadioEngine::MrpoVolumeChange( TInt FDEBUGVAR( aVolume ) )
    {
    FTRACE(FPrint(_L("CRadioEngine::MrpoVolumeChange() - Volume = %d"), aVolume) );
    HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventVolumeUpdated, KErrNone );
    }

// ----------------------------------------------------
// CRadioEngine::MrpoMuteChange
//
// ----------------------------------------------------
//
void CRadioEngine::MrpoMuteChange( TBool aMute )
    {
    FTRACE(FPrint(_L("CRadioEngine::MrpoMuteChange() - Mute = %d"), aMute));

    if ( aMute && !iRadioSettings->IsMuteOn() )
        {
        iRadioSettings->SetMuteOn();
        HandleCallback(MRadioEngineStateChangeCallback::EFMRadioEventSetMuteState, KErrNone);
        }
    else if ( !aMute && iRadioSettings->IsMuteOn() )
        {
        iRadioSettings->SetMuteOff();
        HandleCallback(MRadioEngineStateChangeCallback::EFMRadioEventSetMuteState, KErrNone);
        }
    }

// ----------------------------------------------------
// CRadioEngine::MrpoBalanceChange
//
// ----------------------------------------------------
//
void CRadioEngine::MrpoBalanceChange(
    TInt /*aLeftPercentage*/,
    TInt /*aRightPercentage*/ )
    {
    // Not supported.
    }

// -----------------------------------------------------------------------------
// CRadioEngine::MrpeoPresetChanged
// Observer for Presets
// -----------------------------------------------------------------------------
void CRadioEngine::MrpeoPresetChanged(
    TPresetChangeEvent /*aChange*/,
    TInt /*aIndex*/ )
    {

    }

// -----------------------------------------------------------------------------
// CRadioEngine::NetworkUpCallbackL
// callback notifies when networks comes up
// -----------------------------------------------------------------------------
//
void CRadioEngine::NetworkUpCallbackL()
	{
#ifndef __WINS__
	
	if ( !iNetworkInfoListener )
		{
		// NetworkInfoListener takes care of listening to the network id and country code.
		iNetworkInfoListener = CFMRadioMobileNetworkInfoListener::NewL( *this );
		}
#endif
	}

// -----------------------------------------------------------------------------
// CRadioEngine::NetworkDownCallbackL
// callback notifies when network goes down
// -----------------------------------------------------------------------------
//
void CRadioEngine::NetworkDownCallbackL()
	{
	// No implementation needed	
	}

// -----------------------------------------------------------------------------
// CRadioEngine::NetworkIdChanged
// Used for notifying that the network id has changed
// -----------------------------------------------------------------------------
//
void CRadioEngine::NetworkIdChanged()
	{
	// No implementation needed	
	}

// -----------------------------------------------------------------------------
// CRadioEngine::AudioResourcesAvailableL
// This callback notifies when audio resources become available.
// -----------------------------------------------------------------------------
//
void CRadioEngine::AudioResourcesAvailableL()
	{
	FTRACE( FPrint( _L("CRadioEngine::AudioResourcesAvailableL()" ) ) );
	if ( iPubSub && !iInCall && iTunerControl ) // if iPubSub exists, then the ConstructL has been successfully completed
		{
	    HandleCallback(MRadioEngineStateChangeCallback::EFMRadioEventAudioResourceAvailable, KErrNone );
		}
	}

// -----------------------------------------------------------------------------
// CRadioEngine::AudioAutoResumeForbiddenL
// This callback notifies when audio auto resuming is forbidden.
// -----------------------------------------------------------------------------
//
void CRadioEngine::AudioAutoResumeForbiddenL()
	{
	FTRACE( FPrint( _L("CRadioEngine::AudioAutoResumeForbiddenL()" ) ) );
	if ( iPubSub ) // if iPubSub exists, then the ConstructL has been successfully completed
		{
    	HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventAudioResourceLost, KErrNone );
		}	
	}

// -----------------------------------------------------------------------------
// CRadioEngine::CallActivatedCallbackL
// This callback notifies when call becomes active.
// -----------------------------------------------------------------------------
//
void CRadioEngine::CallActivatedCallbackL()
	{
	FTRACE( FPrint( _L("CRadioEngine::CallActivatedCallbackL()" ) ) );
	// no implementation needed, CRadioEngine::MrpoStateChange handles call startup		
	}
	
// -----------------------------------------------------------------------------
// CRadioEngine::CallDeactivatedCallbackL
// This callback notifies when call becomes deactive.
// -----------------------------------------------------------------------------
//
void CRadioEngine::CallDeactivatedCallbackL()
	{
	FTRACE( FPrint( _L("CRadioEngine::CallDeactivatedCallbackL()" ) ) );	
	iInCall = EFalse;	
	if ( iPubSub ) // if iPubSub exists, then the ConstructL has been successfully completed	
		{
		HandleCallback( MRadioEngineStateChangeCallback::EFMRadioEventCallEnded, KErrNone );	
		}		
	}	

// ---------------------------------------------------------
// CRadioEngine::RdsDataProgrammeService
// ---------------------------------------------------------
//
void CRadioEngine::RdsDataProgrammeService( const TDesC& /*aProgramService*/ )
    {
    }

// ---------------------------------------------------------
// CRadioEngine::RdsDataPsNameIsStatic
// ---------------------------------------------------------
//
void CRadioEngine::RdsDataPsNameIsStatic( TBool /*aStatic*/ )
    {
    }

// ---------------------------------------------------------
// CRadioEngine::RdsDataRadioText
// ---------------------------------------------------------
//
void CRadioEngine::RdsDataRadioText( const TDesC& /*aRadioText*/ )
    {
    }

// ---------------------------------------------------------
// CRadioEngine::RdsDataRadioTextPlus
// ---------------------------------------------------------
//
void CRadioEngine::RdsDataRadioTextPlus( const TInt /*aRTPlusClass*/, const TDesC& /*aRadioText*/ )
    {
    }

// ---------------------------------------------------------
// CRadioEngine::RdsAfSearchBegin
// ---------------------------------------------------------
//
void CRadioEngine::RdsAfSearchBegin()
    {
    iFrequencySetByRdsAf = ETrue;
    }

// ---------------------------------------------------------
// CRadioEngine::RdsAfSearchEnd
// ---------------------------------------------------------
//
void CRadioEngine::RdsAfSearchEnd( TUint32 /*aFrequency*/, TInt aError )
    {
   if( aError != KErrNone )
        {
        iFrequencySetByRdsAf = EFalse;
        }
    }

// ---------------------------------------------------------
// CRadioEngine::RdsAfSearchStateChange
// ---------------------------------------------------------
//
void CRadioEngine::RdsAfSearchStateChange( TBool /*aEnabled*/ )
    {
    }

// ---------------------------------------------------------
// CRadioEngine::RdsAvailable
// ---------------------------------------------------------
//
void CRadioEngine::RdsAvailable( TBool /*aAvailable*/ )
    {
    }

// -----------------------------------------------------------------------------
// CRadioEngine::InitializeResourceLoadingL
// add resource file information
// -----------------------------------------------------------------------------
//
void CRadioEngine::InitializeResourceLoadingL()
    {
    CCoeEnv* coeEnv = CCoeEnv::Static();
    RFs fs( coeEnv->FsSession() );
    TFileName fileName; 
    TFileName baseResource;
    TFindFile finder( fs );
    TLanguage language( ELangNone );

    TParsePtrC parse( KFMRadioEngineResourceFile );
    _LIT( resourceFileWildExt, ".r*" );

    // Make sure to find all resource files, not only .rsc files as it may be so
    // that there is only .r01, .r02, etc. files available
    fileName.Copy( parse.Name() );
    fileName.Append( resourceFileWildExt );

    // TFindFile applies search order that is from 
    // drive Y to A, then Z
    CDir* entries = NULL;
    TInt err = finder.FindWildByDir( fileName , KDC_RESOURCE_FILES_DIR, entries );
    delete entries;
    entries = NULL;
    TBool found = EFalse;
    while ( !found && err == KErrNone )
        {
        // Found file
        fileName.Zero();
        TParsePtrC foundPath( finder.File() );
        fileName.Copy( foundPath.DriveAndPath() );
        fileName.Append( KFMRadioEngineResourceFile );
        BaflUtils::NearestLanguageFile( fs, fileName, language );
        if ( language != ELangNone && BaflUtils::FileExists( fs, fileName ) )
            {
            found = ETrue;
            iFMRadioEngineResourceOffset = coeEnv->AddResourceFileL( fileName );
            }
        else
            {
            if ( language == ELangNone &&
                 !baseResource.Compare( KNullDesC ) &&
                 BaflUtils::FileExists( fs, fileName ) )
                {
                baseResource.Copy( fileName );
                }
            err = finder.FindWild( entries );
            delete entries;
            entries = NULL;
            }
        }

    if ( !found && baseResource.Compare( KNullDesC ) )
        {
        // If we found *.rsc then better to use that than nothing
        if ( BaflUtils::FileExists( fs, baseResource ) )
            {
            iFMRadioEngineResourceOffset = coeEnv->AddResourceFileL( baseResource );
            }
        }
    }

// -----------------------------------------------------------------------------
// CRadioEngine::TunerFrequencyRangeForRegionId
// convert FMRadio region info to tuner utility frequency range
// -----------------------------------------------------------------------------
//
TFmRadioFrequencyRange CRadioEngine::TunerFrequencyRangeForRegionId( const TInt aRegionId ) const
    {
    FTRACE( FPrint( _L("CRadioEngine::TunerFrequencyRangeForRegionId(TInt aRegionID = %d)"), aRegionId ) );  
    TFmRadioFrequencyRange result = EFmRangeEuroAmerica;
    switch( aRegionId )
        {
        case EFMRadioRegionDefault:
            {
            result = EFmRangeEuroAmerica;
            break;
            }
        case EFMRadioRegionJapan:
            {
            result = EFmRangeJapan;
            break;
            }
        case EFMRadioRegionAmerica:
            {
            result = EFmRangeEuroAmerica;
            break;
            }
        default:
            break;
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CRadioEngine::GetRegionL
// determine current region
// -----------------------------------------------------------------------------
//	
EXPORT_C TFMRadioRegionSetting CRadioEngine::GetRegionL() const
    {
    FTRACE( FPrint( _L("CRadioEngine::GetRegionL()" ) ) );    
    TInt regionCount = iCentralRepositoryHandler->CountRegions();
    TFMRadioRegionSetting reg = EFMRadioRegionNone;

    if ( iSystemEventDetector->IsNetworkCoverage() )
        {
        TBool countryCodeMatch( EFalse );
        // Choose the frequency range according to country code
        const TDesC& countryCode = 
            iNetworkInfoListener->CountryCode();
        for ( TInt i = 0 ; i < regionCount ; i++ )
            {
            const RFMRadioCountryCodeArray& regionCountryCodes = 
                iCentralRepositoryHandler->Region( i ).CountryCodes();
                
            for ( TInt j = 0 ; j < regionCountryCodes.Count() ; j++ )
                {
                if ( countryCode == *regionCountryCodes[j] )
                    {
                    // We have a match
                    countryCodeMatch = ETrue;
                    reg = iCentralRepositoryHandler->Region(i).Id();
                    }
                }
            }
        if ( !countryCodeMatch )
            {  
            // All regions have been searched and no direct match found, 
            // use default one
            reg = EFMRadioRegionDefault;
            }
        }
    else 
        {
        if ( iCentralRepositoryHandler->RegionId() != EFMRadioRegionNone )
            {
            reg = static_cast<TFMRadioRegionSetting>( 
                iCentralRepositoryHandler->RegionId() );
            }
        }
    return reg;	
    }

// ----------------------------------------------------
// CRadioEngine::FillListWithRegionDataL
// fill given list with fmradio regions 
// ----------------------------------------------------
//	
EXPORT_C void CRadioEngine::FillListWithRegionDataL( CDesC16Array& aArray ) const
	{
	TInt regionCount = iCentralRepositoryHandler->CountRegions();	
	
	for ( TInt i = 0 ; i < regionCount ; i++ )
        {
        aArray.AppendL( iCentralRepositoryHandler->Region( i ).Name() );
        }	
	}

// ----------------------------------------------------
// CRadioEngine::RegionIdAtIndex
// return region from the array of supported regions
// ----------------------------------------------------
//	
EXPORT_C TFMRadioRegionSetting CRadioEngine::RegionIdAtIndex( const TInt aIndex ) const
	{
 	return iCentralRepositoryHandler->Region( aIndex ).Id();		
	}

// ----------------------------------------------------
// CRadioEngine::RegionId
// Return current region Id.
// ----------------------------------------------------
//
EXPORT_C TInt CRadioEngine::RegionId() const
	{
	return iCentralRepositoryHandler->RegionId();	
	}
	
// ----------------------------------------------------
// CRadioEngine::SetRegionIdL
// Change the the current region Id.
// ----------------------------------------------------
//	
EXPORT_C void CRadioEngine::SetRegionIdL( TInt aRegionId ) const
	{
	if ( iCentralRepositoryHandler->RegionId() != aRegionId )
        {
        // Change only when necessary as it changes also the default tuned frequency
        //iSetters.SetRegionIdL( aRegionId );
        iCentralRepositoryHandler->SetRegionIdL( aRegionId );
        iPubSub->PublishFrequencyDecimalCountL( 
                TFMRadioPSFrequencyDecimalCount( iRadioSettings->DecimalCount() ) );
        }		
	}

// ----------------------------------------------------
// CRadioEngine::RequestTunerControl
// request tuner control from RadioFmTunerUtility
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::RequestTunerControl() const
	{
	// Before first RequestTunerControl() call it is ok to enable offline mode without checking capabilities 
    iFmTunerUtility->EnableTunerInOfflineMode( ETrue );
    iFmTunerUtility->RequestTunerControl();	
	}

// ----------------------------------------------------
// CRadioEngine::DecimalCount
// return decimal count for current region MHz information
// ----------------------------------------------------
//
EXPORT_C TInt CRadioEngine::DecimalCount() const
	{
	return iCentralRepositoryHandler->DecimalCount();	
	}

// ----------------------------------------------------
// CRadioEngine::FrequencyStepSize
// return step size for tuning.
// ----------------------------------------------------
//
EXPORT_C TUint32 CRadioEngine::FrequencyStepSize() const
	{
	return iRadioSettings->FrequencyStepSize();
	}
	
// ----------------------------------------------------
// CRadioEngine::PubSubL
// Returns a reference to P&S interface
// ----------------------------------------------------
//
EXPORT_C CFMRadioPubSub& CRadioEngine::PubSubL() const
    {	
    User::LeaveIfNull( iPubSub );
    return *iPubSub;   
    }	

// ----------------------------------------------------
// CRadioEngine::StartupCount
// increases startup count by one and returns amount of app startups.
// ----------------------------------------------------
//
EXPORT_C TInt CRadioEngine::UpdatedStartupCount() const
	{
	FTRACE( FPrint( _L("CRadioEngine::UpdatedStartupCount()" ) ) );	
	iCentralRepositoryHandler->IncreaseStartupCount();
	return iRadioSettings->StartupCount();
	}

// ----------------------------------------------------
// CRadioEngine::CancelScanLocalStationsScan
// Cancel seek and notify UI
// ----------------------------------------------------
//
EXPORT_C void CRadioEngine::CancelScanLocalStationsScan()
	{
	FTRACE( FPrint( _L("CRadioEngine::CancelScanLocalStationsScan()" ) ) );
	CancelScan();
	TInt error = KErrNone;
	iStateHandler->Callback( MRadioEngineStateChangeCallback::EFMRadioEventScanLocalStationsCanceled, error ); 
	}

// ---------------------------------------------------------------------------
// CRadioEngine::IsAudioRoutingPossible
// Routing is not possible when headset is not available or radio is off
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CRadioEngine::IsAudioRoutingPossible() const
    {
    FTRACE( FPrint( _L("CRadioEngine::IsAudioRoutingPossible()" ) ) );
    TBool isAudioRoutingPossible = EFalse;
    
    TBool headsetConnected = iHeadsetObserver->IsHeadsetAccessoryConnected();
    TBool radioOn = iRadioSettings->IsRadioOn();
        
    if ( headsetConnected && radioOn )
        {
        isAudioRoutingPossible = ETrue;
        }
        
    return isAudioRoutingPossible;
    }
	
// ---------------------------------------------------------------------------
// CRadioEngine::MusicStoreNoteDisplayed
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CRadioEngine::MusicStoreNoteDisplayed()
    {
    return iCentralRepositoryHandler->MusicStoreNoteDisplayed();
    }

// ---------------------------------------------------------------------------
// CRadioEngine::SetMusicStoreNoteDisplayed
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioEngine::SetMusicStoreNoteDisplayed()
    {
    iCentralRepositoryHandler->SetMusicStoreNoteDisplayed();
    }

// ---------------------------------------------------------------------------
// CRadioEngine::MusicStoreNoteDisplayed
// ---------------------------------------------------------------------------
//
EXPORT_C TFMRadioRTPlusLevel CRadioEngine::GetRTPlusSupportLevel()
    {
    TInt value = iCentralRepositoryHandler->GetRTPlusSupportLevel();
    
    //do not return the value directly - make sure we return a valid value
    if ( value == EFMRadioAllInteractions )
        {
        return EFMRadioAllInteractions;
        }
    return EFMRadioMusicStoreOnly;
    }

// ---------------------------------------------------------------------------
// CRadioEngine::SetCurrentPresetIndex
// sets current preset index if the preset database changes
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioEngine::SetCurrentPresetIndex( const TInt aIndex )
    {
    TInt maxNumberOfPresets;
    iPresetUtility->GetMaxNumberOfPresets( maxNumberOfPresets );
    
    if ( ( aIndex >= KErrNotFound ) && ( aIndex  < maxNumberOfPresets ) )
        {
        iRadioSettings->SetCurrentPreset( aIndex );
        if ( aIndex >= 0 )
            {
            // publish current preset
            TRAP_IGNORE
                (
                TInt frequency = 0;
                TStationName name;
                iPresetUtility->GetPresetL( aIndex, name, frequency );
                iPubSub->PublishChannelNameL( name );
                iPubSub->PublishChannelL( aIndex );
                )
            }
        }
    }

// ---------------------------------------------------------------------------
// CRadioEngine::StoreAndPublishFrequency
// Updates the frequency into settings and P&S
// ---------------------------------------------------------------------------
//
void CRadioEngine::StoreAndPublishFrequency( TInt aFrequency )
    {
    iRadioSettings->SetFrequency( aFrequency );
    TRAP_IGNORE( iPubSub->PublishFrequencyL( aFrequency ); )
    }

// ---------------------------------------------------------------------------
// CRadioEngine::PresetUrlL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CRadioEngine::PresetUrlL( TInt aIndex, TDes& aUrl )
    {
    FTRACE( FPrint( _L("CRadioEngine::PresetUrlL( Index %d, url: %S)"), aIndex, &aUrl ) );
    TInt err = KErrNone;
    TFmPresetUrl presetUrl;
    
    if ( presetUrl.MaxLength() <= aUrl.MaxLength() )
        {
        iPresetUtility->GetPresetUrlL( aIndex, presetUrl );
        aUrl.Copy( presetUrl );
        if ( !aUrl.Length() )
            {
            err = KErrNotFound;
            }            
        }
    else
        {
        err = KErrOverflow;
        }
    return err;
    }

// ---------------------------------------------------------------------------
// CRadioEngine::DeletePresetL
// Delete preset. With index -1 all preset are reseted
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioEngine::DeletePresetL( TInt aIndex )
    {
    FTRACE( FPrint( _L("CRadioEngine::DeletePresetL( i: %d )"), aIndex ) );
    iPresetUtility->DeletePresetL( aIndex );
    }

// ---------------------------------------------------------------------------
// CRadioEngine::SaveUrlToCurrentPresetL
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioEngine::SaveUrlToPresetL( TInt aIndex, const TDesC& aUrl )
    {
    FTRACE( FPrint( _L("CRadioEngine::SaveUrlToCurrentPresetL( i: %d Url: %S)"), aIndex, &aUrl ) );
    TFmPresetUrl presetUrl;
    if ( aUrl.Length() <= presetUrl.MaxLength() )
        {
        presetUrl.Copy( aUrl );
        iPresetUtility->SetPresetUrlL( aIndex, presetUrl );
        }
    }

// End of file
