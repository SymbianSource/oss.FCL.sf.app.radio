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
* Description:
*
*/

// System includes
#include <audiooutput.h>
#include <eikdef.h>
#include <coemain.h>
#include <badesca.h>
#include <tzlocalizer.h>

// User includes
#include "cradioenginelogger.h"
#include "radiointernalpskeys.h"
#include "radiointernalcrkeys.h"
#include "cradioaudiorouter.h"
#include "cradiopubsub.h"
#include "cradioengineimp.h"
#include "mradioengineobserver.h"
#include "mradioscanobserver.h"
#include "cradioregion.h"
#include "cradiosettings.h"
#include "mradioenginesettings.h"
#include "mradiosettingssetter.h"
#include "cradiorepositorymanager.h"
#include "cradiordsreceiver.h"
#include "cradiosystemeventcollector.h"
#include "cradionetworkinfolistener.h"
#include "radioengine.hrh"

#include "../../group/buildflags.hrh"
#ifdef __FEATURE_RDS_SIMULATOR
#   include "t_cradiordsreceiversimulator.h"
#endif

// Constants

/** The limit of volume steps that must not be divided */
const TInt KRadioVolumeStepsDividinglimit = 20;
/** If CMMTunerUtility has 200 volume steps, AknVolume control has 20 steps */
const TInt KRadioVolumeStepsDivider = 500;
/** amount of volume steps used previously */
#if defined __WINS__
const TInt KRadioVolumeStepsOld = 10;
#endif // defined __WINS__

/** KRadioRadioSwitchDelay value must not be too small, otherwise problems with
 radioserver will occur, when swithing between valid and invalid presets */
const TInt KRadioRadioSwitchDelay = 300; // In milliseconds

const TInt KRadioThousand = 1000;
const TInt KRadioMillion = KRadioThousand * KRadioThousand;

// Country order from timezones.rss. Default = 0, Japan = 1, America = 2.
const TUint KCityGroupRegions[] =
    {0,0,0,0,0,0,2,0,0,0,0,2,0,2,0,2,0,2,0,2,2,0,2,0,0,2,2,2,0,2,0,0, //32
     0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,2,1,0,0,0,0,0,0,0,0,0,0,0, //64
     0,0,0,0,0,2,0,0,0,2,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //96
     0,0,0,2,0,0,0,0,2,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //128
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //160
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0, //192
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0, //224
     2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};



// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineImp::CRadioEngineImp( CRadioAudioRouter* aAudioRouter )
    : CRadioEngine( aAudioRouter )
    , iAntennaAttached( ETrue )
    , iFreqEventReason( RadioEngine::ERadioFrequencyEventReasonUnknown )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::ConstructL()
    {
    LOG_METHOD_AUTO;

    if ( !iAudioRouter || !iSystemEventCollector || !iSettings )
        {
        User::Leave( KErrNotReady );
        }

    iSystemEventCollector->AddObserverL( this );

    // Initial default values to be over by init
    User::LeaveIfError( iSettings->RadioSetter().SetPowerOn( EFalse ) );

    // The output source needs to be in headset when starting the radio.
    // But if the headset is not connected, audio cannot be heard if it is routed
    // to headset
    RadioEngine::TRadioAudioRoute route = iSystemEventCollector->IsHeadsetConnectedL() ?
                                RadioEngine::ERadioHeadset : RadioEngine::ERadioSpeaker;
    User::LeaveIfError( iSettings->RadioSetter().SetAudioRoute( route ) );

#ifdef __FEATURE_RDS_SIMULATOR
    iRdsReceiver = CRadioRdsReceiverSimulator::NewL( iSettings->EngineSettings() );
#else
    iRdsReceiver = CRadioRdsReceiver::NewL( iSettings->EngineSettings() );
#endif

    iNetworkInfoListener = CRadioNetworkInfoListener::NewL( iSettings->RadioSetter(), NULL );

    iRdsReceiver->AddObserverL( this );

    // Create timer that is used when polling for radio restart.
    iRadioTimer = CPeriodic::NewL( CActive::EPriorityHigh );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineImp::~CRadioEngineImp()
    {
    LOG( "CRadioEngineImp::~CRadioEngineImp -- Start" );

    delete iNetworkInfoListener;

    PowerOff();

    delete iRadioTimer;

    DeleteAudioOutput();

    if ( iTunerUtility )
        {
        iTunerUtility->Close();
        }

    if ( iPlayerUtility )
        {
        iPlayerUtility->Close();
        }

    if ( iRdsReceiver )
        {
        iRdsReceiver->RemoveObserver( this );
        }

    delete iRdsReceiver;
    delete iRadioUtility;

    iObservers.Close();

    if ( iSystemEventCollector )
        {
        iSystemEventCollector->RemoveObserver( this );
        }
    delete iSystemEventCollector;

    delete iPubSub;

    if ( iSettings )
        {
        iSettings->RadioSetter().SetObserver( NULL );
        }
    delete iSettings;

    LOG( "CRadioEngineImp::~CRadioEngineImp -- End" );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::SetSystemEventCollector( CRadioSystemEventCollector* aCollector )
    {
    iSystemEventCollector = aCollector;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::SetRadioSettings( CRadioSettings* aSettings )
    {
    iSettings = aSettings;
    iSettings->RadioSetter().SetObserver( this );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::SetRadioPubSub( CRadioPubSub* aPubSub )
    {
    iPubSub = aPubSub;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioAudioRouter& CRadioEngineImp::AudioRouter() const
    {
    return *iAudioRouter;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSystemEventCollector& CRadioEngineImp::SystemEventCollector() const
    {
    return *iSystemEventCollector;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSettings& CRadioEngineImp::Settings() const
    {
    return *iSettings;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioPubSub* CRadioEngineImp::PubSub() const
    {
    return iPubSub;
    }

// ---------------------------------------------------------------------------
// Determines radio region
// ---------------------------------------------------------------------------
//
TRadioRegion CRadioEngineImp::DetermineRegion()
    {
    TRadioRegion region = ERadioRegionNone;

    MRadioEngineSettings& engineSettings = iSettings->EngineSettings();
    if ( iSystemEventCollector->IsMobileNetworkCoverage() )
        {
        region = RegionFromMobileNetwork();
        }
    else
        {
        region = RegionFromTimezone();
        }

    if ( !iSettings->IsRegionAllowed( region ) )
        {
        region = ERadioRegionNone;
        }

    // Region not found, try to use the previously set region
    if ( region == ERadioRegionNone && engineSettings.RegionId() != ERadioRegionNone )
        {
        region = engineSettings.RegionId();
        }

    // All regions have been searched and no direct match found, use default one
    if ( region == ERadioRegionNone )
        {
        region = engineSettings.DefaultRegion();
        }

    return region;
    }

// ---------------------------------------------------------------------------
// Initializes / reinitializes the radio. If this is not called
// the radio is not functional
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::InitRadioL( TInt aRegionId, CRadioPubSub* aPubSub )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "CRadioEngineImp::InitRadioL: Region: %d", aRegionId );

    iRadioInitializationState = ERadioNotInitialized;

    iPubSub = aPubSub;

    iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonUnknown;
    if ( iSettings->EngineSettings().RegionId() != aRegionId )
        {
        // Change only when necessary as it changes also the default tuned frequency
        User::LeaveIfError( iSettings->RadioSetter().SetRegionId( aRegionId ) );
        }

    if ( !iRadioUtility )
        {
        iRadioUtility = CRadioUtility::NewL( ETrue );
        }

    if ( !iPlayerUtility )
        {
        iPlayerUtility = &iRadioUtility->RadioPlayerUtilityL( *this );
        SetAudioOutput( CAudioOutput::NewL( *iPlayerUtility ) );
        }

    if ( !iTunerUtility )
        {
        iTunerUtility = &iRadioUtility->RadioFmTunerUtilityL( *this );
        }

    // Utilities have been created now
    iRadioInitializationState = ERadioUtilitiesConstructed;

    // Before first RequestTunerControl() call it is ok to enable offline mode without checking capabilities
    iTunerUtility->EnableTunerInOfflineMode( ETrue );
    iTunerUtility->RequestTunerControl();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineImp::RadioInitialized() const
    {
    return iRadioInitializationState == ERadioTunerControlGranted;
    }

// ---------------------------------------------------------------------------
// Sets the state for radio audio
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::EnableAudio( TBool aEnable, TBool aDelay )
    {
    LOG_FORMAT( "CRadioEngineImp::EnableAudio( %d )", aEnable );
    iRadioEnabled = aEnable;
    if ( aDelay )
        {
        SwitchPower( iRadioEnabled );
        }
    else if ( aEnable )
        {
        if ( !RadioInitialized() &&
             iRadioInitializationState == ERadioUtilitiesConstructed &&
             iRadioEnabled &&
             OkToPlay( iSettings->EngineSettings().TunedFrequency() ) )
            {
            iTunerUtility->RequestTunerControl();
            }
        else
            {
            PowerOn();
            }
        }
    else
        {
        PowerOff();
        }
    }

// ---------------------------------------------------------------------------
// Gets the state for radio audio.
// ---------------------------------------------------------------------------
//
TBool CRadioEngineImp::RadioAudioEnabled() const
    {
    return iRadioEnabled;
    }

// ---------------------------------------------------------------------------
// Sets the state for audio overriding
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::SetAudioOverride( TBool aOverride )
    {
    LOG_FORMAT( "CRadioEngineImp::SetAudioOverride( %d )", aOverride );
    iOverrideAudioResources = aOverride;
    }

// ---------------------------------------------------------------------------
// Adds an observer for the radio state changes notifications.
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::AddObserverL( MRadioEngineObserver* aObserver )
    {
    LOG( "CRadioEngineImp::AddObserver" );
    TInt index = iObservers.FindInAddressOrder( aObserver );
    if ( index == KErrNotFound )
        {
        iObservers.InsertInAddressOrderL( aObserver );
        }
    }

// ---------------------------------------------------------------------------
// Removes an observer from the list of obsevers
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::RemoveObserver( MRadioEngineObserver* aObserver )
    {
    LOG( "CRadioEngineImp::RemoveObserver" );
    TInt index = iObservers.FindInAddressOrder( aObserver );

    if ( index >= 0 )
        {
        iObservers.Remove( index );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TFmRadioFrequencyRange CRadioEngineImp::TunerFrequencyRangeForRegionId( TInt aRegionId ) const
    {
    TFmRadioFrequencyRange result = EFmRangeEuroAmerica;
    switch ( aRegionId )
        {
        case ERadioRegionDefault:
            {
            result = EFmRangeEuroAmerica;
            break;
            }
        case ERadioRegionJapan:
            {
            result = EFmRangeJapan;
            break;
            }
        case ERadioRegionAmerica:
            {
            result = EFmRangeEuroAmerica;
            break;
            }
        default:
            break;
        }
    return result;
    }

// ---------------------------------------------------------------------------
// Sets radio mode ERadioStereo or ERadioMono
// ---------------------------------------------------------------------------

void CRadioEngineImp::SetAudioMode( TInt aAudioMode )
    {
    LOG_FORMAT( "CRadioEngineImp::SetAudioMode: aAudioMode: %d", aAudioMode );
    TInt err = KErrNone;
    if ( !RadioInitialized() )
        {
        TInt err = iSettings->RadioSetter().SetOutputMode( aAudioMode );
        NotifyRadioEvent( ERadioEventAudioMode, err );
        }
    else
        {
        err = iTunerUtility->ForceMonoReception( aAudioMode == RadioEngine::ERadioMono );

        if ( err )
            {
            NotifyRadioEvent( ERadioEventAudioMode, err );
            }
        }
    }

// ---------------------------------------------------------------------------
// Switches power on/off after a delay
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::SwitchPower( TBool aPowerOn )
    {
    LOG_FORMAT( "CRadioEngineImp::SwitchPower( %d )", aPowerOn );
    if ( RadioInitialized() )
        {
        if ( !aPowerOn || OkToPlay( iSettings->EngineSettings().TunedFrequency() ) )
            {
            iRadioTimer->Cancel();
            if ( aPowerOn )
                {
                iRadioTimer->Start( TTimeIntervalMicroSeconds32( KRadioRadioSwitchDelay * KRadioThousand ),
                        TTimeIntervalMicroSeconds32( 0 ),
                        TCallBack( StaticPowerOnCallback, this ) );
                }
            else
                {
                iRadioTimer->Start( TTimeIntervalMicroSeconds32( KRadioRadioSwitchDelay * KRadioThousand ),
                        TTimeIntervalMicroSeconds32( 0 ),
                        TCallBack( StaticPowerOffCallback, this ) );
                }
            }
        }
    else if ( aPowerOn &&
            iRadioInitializationState == ERadioUtilitiesConstructed &&
            iRadioEnabled &&
            OkToPlay( iSettings->EngineSettings().TunedFrequency() ) )
        {
        iTunerUtility->RequestTunerControl();
        }
    else
        {
        LOG( "CRadioEngineImp::SwitchPower - Unhandled case" );
        LOG_FORMAT( "PowerOn: %d, InitializationState: %d, Enabled: %d, Frequency: %d",
                aPowerOn, iRadioInitializationState, iRadioEnabled, iSettings->EngineSettings().TunedFrequency() );
        }
    }

// ---------------------------------------------------------------------------
// Executes the power switch
// ---------------------------------------------------------------------------
//
TInt CRadioEngineImp::StaticPowerOnCallback( TAny* aSelfPtr )
    {
    LOG( "CRadioEngineImp::StaticPowerOnCallback" );
    CRadioEngineImp* self = reinterpret_cast<CRadioEngineImp*>( aSelfPtr );

    if ( self )
        {
        self->iRadioTimer->Cancel(); // prevents the further calls.

        if ( !self->iSettings->EngineSettings().IsPowerOn() )
            {
            self->PowerOn();
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Executes the power switch
// ---------------------------------------------------------------------------
//
TInt CRadioEngineImp::StaticPowerOffCallback( TAny* aSelfPtr )
    {
    LOG( "CRadioEngineImp::StaticPowerOffCallback" );
    CRadioEngineImp* self = reinterpret_cast<CRadioEngineImp*>( aSelfPtr );

    if ( self )
        {
        self->iRadioTimer->Cancel(); // prevents the further calls.

        if ( self->iSettings->EngineSettings().IsPowerOn() )
            {
            self->PowerOff();
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::PowerOn()
    {
    LOG_METHOD_AUTO;

    if ( RadioInitialized() &&
        iRadioEnabled &&
        OkToPlay( iSettings->EngineSettings().TunedFrequency() ) )
        {
        SetAudioMode( iSettings->EngineSettings().OutputMode() );
        iPlayerUtility->SetVolumeRamp( TTimeIntervalMicroSeconds( MAKE_TINT64( 0, KRadioMillion ) ) );
        iPlayerUtility->SetVolume( TunerVolumeForUiVolume( iSettings->EngineSettings().Volume() ) );

        // If we are about to start scanning, mute the radio and set minimum frequency
        if ( iScanObserver )
            {
            iPlayerUtility->Mute( ETrue );
            iTunerUtility->SetFrequency( iSettings->EngineSettings().MinFrequency() );
            }
        else
            {
            iPlayerUtility->Mute( iSettings->EngineSettings().IsVolMuted() );
            iTunerUtility->SetFrequency( iSettings->EngineSettings().TunedFrequency() );
            }
        iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonImplicit;

        TRAP_IGNORE( iAudioRouter->SetAudioRouteL(
                RadioEngine::TRadioAudioRoute( iSettings->EngineSettings().AudioRoute() ) ) )

        iPlayerUtility->Play();
        }
    else
        {
        HandlePowerEvent( EFalse, KErrGeneral );
        }
    }

// ---------------------------------------------------------------------------
// Radio power off
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::PowerOff()
    {
    LOG( "CRadioEngineImp::PowerOff" );

    if ( iSettings->EngineSettings().IsPowerOn() )
        {
        if ( RadioInitialized() )
            {
            iPlayerUtility->Stop();
            }
        else // This shouldn't occur normally, just a recovery action
            {
            HandlePowerEvent( EFalse, KErrNone );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineImp::OkToPlay( TUint32 aFrequency ) const
    {
    TBool audioResourcesAvailable = iSystemEventCollector->IsAudioResourcesAvailable();
    TBool okToPlay = iAntennaAttached &&
                     !iFmTransmitterActive &&
                     ( audioResourcesAvailable || iOverrideAudioResources ) &&
#ifdef COMPILE_IN_IVALO
                     IsFrequencyValid( aFrequency )
                     && !iSystemEventCollector->IsCallActive();
#else
                     IsFrequencyValid( aFrequency );
#endif //COMPILE_IN_IVALO

    LOG_FORMAT( "CRadioEngineImp::OkToPlay, returning %d ", okToPlay );
    return okToPlay;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineImp::IsFrequencyValid( TUint32 aFrequency ) const
    {
    TBool ret( EFalse );
    if ( !aFrequency )
        {
        aFrequency = iSettings->EngineSettings().TunedFrequency();
        }
    if ( aFrequency >= iSettings->EngineSettings().MinFrequency() && aFrequency <= iSettings->EngineSettings().MaxFrequency() )
        {
        ret = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// Radio tuning
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::SetFrequency( TUint32 aFrequency, RadioEngine::TRadioFrequencyEventReason aReason )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "CRadioEngineImp::SetFrequency, freq: %u, Initialized: %d, Enabled: %d",
            aFrequency, RadioInitialized(), iRadioEnabled );

    iFrequencySetByRdsAf = EFalse;
    iFreqEventReason = aReason;

    TInt frequency = 0;
    if ( iTunerUtility )
        {
        iTunerUtility->GetFrequency( frequency );
        }
    CancelSeek();

    if ( aFrequency == frequency ) //radio has already the frequency to be set.
        {
        LOG( "CRadioEngineImp::SetFrequency: Already at the requested frequency" );
        HandleFrequencyEvent( aFrequency );
        }
    else
        {
        iRadioTimer->Cancel();
        if ( RadioInitialized() && iRadioEnabled && OkToPlay( aFrequency ) )
            {
            LOG( "CRadioEngineImp::SetFrequency: Tuning to frequency" );
            iTunerUtility->SetFrequency( aFrequency );
            }
        else
            {
            HandleFrequencyEvent( aFrequency );
            }
        }
    }

// ---------------------------------------------------------------------------
// Radio tuning
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::SetFrequencyFast( TUint32 aFrequency,
                                        RadioEngine::TRadioFrequencyEventReason /*aReason*/ )
{
    if ( iSeekingState != RadioEngine::ERadioNotSeeking )
    {
        iSeekingState = RadioEngine::ERadioNotSeeking;
        iTunerUtility->CancelStationSeek();
    }
    iTunerUtility->SetFrequency( aFrequency );
    iSettings->RadioSetter().SetTunedFrequency( aFrequency );
}

// ---------------------------------------------------------------------------
// Frequency stepping
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::StepToFrequency( RadioEngine::TRadioTuneDirection aDirection )
    {
    LOG( "CRadioEngineImp::StepToFrequency" );

    TUint32 freq = iSettings->EngineSettings().TunedFrequency();
    RadioEngine::TRadioFrequencyEventReason reason( RadioEngine::ERadioFrequencyEventReasonUnknown );
    if ( aDirection == RadioEngine::ERadioUp )
        {
        freq = freq + iSettings->EngineSettings().FrequencyStepSize();
        reason = RadioEngine::ERadioFrequencyEventReasonUp;
        }
    else
        {
        freq = freq - iSettings->EngineSettings().FrequencyStepSize();
        reason = RadioEngine::ERadioFrequencyEventReasonDown;
        }

    // Check overflow or underflow
    if ( IsFrequencyValid( freq ) )
        {
        if ( aDirection == RadioEngine::ERadioUp )
            {
            freq = iSettings->EngineSettings().MinFrequency();
            }
        else
            {
            freq = iSettings->EngineSettings().MaxFrequency();
            }
        }
    SetFrequency( freq, reason );
    }

// ---------------------------------------------------------------------------
// Radio seek
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::Seek( RadioEngine::TRadioTuneDirection aDirection )
    {
    LOG_FORMAT( "CRadioEngineImp::Seek-- Start direction:%d",aDirection );

    iFrequencySetByRdsAf = EFalse;

    // Check if audio playing parameters ( other than frequency ) are OK
    if ( iRadioEnabled &&
        OkToPlay( iSettings->EngineSettings().MinFrequency() ) &&
        iSeekingState == RadioEngine::ERadioNotSeeking )
        {
        if ( aDirection == RadioEngine::ERadioDown )
            {
            iSeekingState = RadioEngine::ERadioSeekingDown;
            iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonDown;
            }
        else
            {
            iSeekingState = RadioEngine::ERadioSeekingUp;
            iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonUp;
            }

        NotifyRadioEvent( ERadioEventSeeking );

        if ( IsFrequencyValid() )
            {
            if ( iSettings->EngineSettings().IsPowerOn() )
                {
                iTunerUtility->StationSeek( aDirection == RadioEngine::ERadioUp ? ETrue : EFalse );
                }
            else
                {
                // Try to switch power on ( reinitialization )
                SwitchPower( ETrue );
                }
            }
        else
            {
            // Tune first to valid frequency, start seeking after radio power is on
            SetFrequency( iSettings->EngineSettings().MinFrequency(), RadioEngine::ERadioFrequencyEventReasonImplicit );
            }
        }
    else
        {
        NotifyRadioEvent( ERadioEventSeeking, KErrGeneral );
        StopScan( KErrGeneral );
        }
    }

// ---------------------------------------------------------------------------
// Cancels seek up/down request
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::CancelSeek()
    {
    LOG_FORMAT( "CRadioEngineImp::CancelSeek -- seeking state was:%d", iSeekingState );

    if ( !iScanObserver )
        {
        if ( iSeekingState != RadioEngine::ERadioNotSeeking )
            {
            iSeekingState = RadioEngine::ERadioNotSeeking;
            iTunerUtility->CancelStationSeek();
            iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonImplicit;
            NotifyRadioEvent( ERadioEventSeeking, KErrCancel );
            NotifyRadioEvent( ERadioEventFrequency, KErrNone ); // Notify the observers even if the frequency remains the same.
            }
        }
    else
        {
        StopScan( KErrCancel );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
RadioEngine::TRadioSeeking CRadioEngineImp::Seeking() const
    {
    return iSeekingState;
    }

// ---------------------------------------------------------------------------
// Starts scanning all available stations from the minimum frequency
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::StartScan( MRadioScanObserver& aObserver )
    {
    LOG( "CRadioEngineImp::StartScan" );

    const TUint32 minFrequency = iSettings->EngineSettings().MinFrequency();
    if ( !iScanObserver && iRadioEnabled && RadioInitialized() && OkToPlay( minFrequency ) )
        {
        CancelSeek();
        iScanObserver = &aObserver;
        iPreviousMuteState = iSettings->EngineSettings().IsVolMuted();
        iPreviousScannedFrequency = 0;
        iPlayerUtility->Mute( ETrue );
        iTunerUtility->SetFrequency( minFrequency );
        Seek( RadioEngine::ERadioUp );
        }
    else
        {
        TInt error = iScanObserver ? KErrAlreadyExists : KErrNotReady;
        TRAP_IGNORE( aObserver.ScanCompletedEventL( error ) )
        }
    }

// ---------------------------------------------------------------------------
// Stops any scans currently in progress.
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::StopScan( TInt aError )
    {
    LOG_FORMAT( "CRadioEngineImp::StopScan, error: %d", aError );
    if ( iScanObserver )
        {
        if ( iSeekingState != RadioEngine::ERadioNotSeeking )
            {
            iSeekingState = RadioEngine::ERadioNotSeeking;
            if ( RadioInitialized() )
                {
                iTunerUtility->CancelStationSeek();
                }
            }

        iPreviousScannedFrequency = 0;
        MRadioScanObserver& observer = *iScanObserver;
        iScanObserver = NULL;
        NotifyRadioScanEvent( ERadioEventScanCompleted, observer, aError );

        if ( !OkToPlay( iSettings->EngineSettings().MinFrequency() ) )
            {
            // Try to reset the frequency as sometimes extra frequency event occurs after seeking
            iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonImplicit;
            iTunerUtility->SetFrequency( iSettings->EngineSettings().TunedFrequency() );
            }

        if ( iPlayerUtility && OkToPlay( iSettings->EngineSettings().TunedFrequency() ) )
            {
            iPlayerUtility->Mute( iPreviousMuteState );
            }
        }
    }

// ---------------------------------------------------------------------------
// Sets volume level up/down one step.
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::AdjustVolume( RadioEngine::TRadioVolumeSetDirection aDirection )
    {
    LOG_FORMAT( "CRadioEngineImp::AdjustVolume( %d )", aDirection );

    if ( iSettings->EngineSettings().IsPowerOn() )
        {
        TInt volume = iSettings->EngineSettings().Volume();
        LOG_FORMAT( "CRadioEngineImp::AdjustVolume volume = ( %d )", volume );

        if ( aDirection == RadioEngine::ERadioDecVolume )
            {
            TInt min = iSettings->EngineSettings().DefaultMinVolumeLevel();
            if ( --volume < min )
                {
                volume = min;
                }
            }
        else if ( aDirection == RadioEngine::ERadioIncVolume )
            {
            TInt max = MaxVolumeLevel();

            if ( ++volume > max )
                {
                volume = max;
                }
            }
        else
            {
            LOG( "CRadioEngineImp::AdjustVolume - Unhandled case" );
            }
        SetVolume( volume );
        }
    }

// ---------------------------------------------------------------------------
// Sets audio volume level
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::SetVolume( TInt aVolume )
    {
    LOG_FORMAT( "CRadioEngineImp::SetVolume vol: %d", aVolume );

    if ( iSettings->EngineSettings().IsPowerOn() && RadioInitialized() )
        {
        LOG( "CRadioEngineImp::SetVolume: Setting volume to player utility" );

        if ( aVolume == 0 )
            {
            SetVolumeMuted( ETrue );
            }
        else
            {
            if ( iSettings->EngineSettings().IsVolMuted() )
                {
                SetVolumeMuted( EFalse );
                }
            else if ( iPlayerUtility->SetVolume( TunerVolumeForUiVolume( aVolume ) ) == KErrNone )
                {
                iSettings->RadioSetter().SetVolume( aVolume );
                NotifyRadioEvent( ERadioEventVolume, KErrNone );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Set radio audio muted/unmuted
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::SetVolumeMuted( TBool aMuteState )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "MuteState = %d", aMuteState );

    if ( iSettings->EngineSettings().IsPowerOn() )
        {
        TInt err = KErrNone;
        if ( RadioInitialized() )
            {
            err = iPlayerUtility->Mute( aMuteState );
            iSettings->RadioSetter().SetVolMuted( aMuteState );
            NotifyRadioEvent( ERadioEventMute, err );
            }
        else
            {
            err = iSettings->RadioSetter().SetVolMuted( aMuteState );
            NotifyRadioEvent( ERadioEventMute, err );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineImp::IsAntennaAttached()
    {
    LOG_FORMAT( "CRadioEngineImp::IsAntennaAttached, returning %d", iAntennaAttached );
    return iAntennaAttached;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineImp::IsFmTransmitterActive() const
    {
    LOG_FORMAT( "CRadioEngineImp::IsFmTransmitterActive, returning %d", iFmTransmitterActive );
    return iFmTransmitterActive;
    }

// ---------------------------------------------------------------------------
// This function is usable only in WINS emulator.
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::SetAntennaAttached( TBool aAntennaAttached )
    {
#ifdef __WINS__
    MrftoAntennaStatusChange( aAntennaAttached );
#endif
    }

// ---------------------------------------------------------------------------
// If CRadioPlayerUtility has 10 steps, CAknVolumeControl has also 10 steps and also
// the maximum volume level of Visual radio is 10. Otherwise CRadioPlayerUtility has
// 200 steps, and CAknVolumeControl has 20 steps, so maximum volume level of Visual radio
// is 20.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineImp::MaxVolumeLevel() const
    {
    TInt maxLevel = 0;

#if defined __WINS__
    maxLevel = KRadioVolumeStepsOld;
#else
    if ( RadioInitialized() )
        {
        iPlayerUtility->GetMaxVolume( maxLevel );
        }
#endif //defined __WINS__

    if ( maxLevel > KRadioVolumeStepsDividinglimit )
        {
        maxLevel = maxLevel / KRadioVolumeStepsDivider;
        }

    return maxLevel;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineImp::FrequencySetByRdsAf() const
    {
    return iFrequencySetByRdsAf;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
MRadioRdsReceiver& CRadioEngineImp::RdsReceiver()
    {
    return *iRdsReceiver;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineImp::TunerVolumeForUiVolume( TInt aUiVolume )
    {
    TInt vol = aUiVolume * KRadioVolumeStepsDivider;

    return vol;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::NotifyRadioEvent( TInt aRadioEvent, TInt aErrorCode )
    {
    TRAP_IGNORE( DoNotifyRadioEventL( aRadioEvent, aErrorCode ) )
    }

// ---------------------------------------------------------------------------
// Notifies all the registered observers of radio events
// by sending notifications to observers.
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::DoNotifyRadioEventL( TInt aRadioEvent, TInt aErrorCode )
    {
    TInt count = iObservers.Count();

    for ( TInt i = 0; i<count; i++)
        {
        MRadioEngineObserver * observer = iObservers[i];

        switch ( aRadioEvent )
            {
            case ERadioEventPower:
                {
                observer->PowerEventL( iSettings->EngineSettings().IsPowerOn(), aErrorCode );
                break;
                }
            case ERadioEventFrequency:
                {
                observer->FrequencyEventL( iSettings->EngineSettings().TunedFrequency(),
                                           iFreqEventReason, aErrorCode );
                break;
                }
            case ERadioEventVolume:
                {
                observer->VolumeEventL( iSettings->EngineSettings().Volume(), aErrorCode );
                break;
                }
            case ERadioEventMute:
                {
                if ( !iScanObserver )
                    {
                    observer->MuteEventL( iSettings->EngineSettings().IsVolMuted(), aErrorCode );
                    }
                break;
                }
            case ERadioEventAudioMode:
                {
                observer->AudioModeEventL( iSettings->EngineSettings().OutputMode(), aErrorCode );
                break;
                }
            case ERadioEventAntenna:
                {
                observer->AntennaEventL( iAntennaAttached, aErrorCode );
                break;
                }
            case ERadioEventAudioRouting:
                {
                observer->AudioRoutingEventL( iSettings->EngineSettings().AudioRoute(), aErrorCode );
                break;
                }
            case ERadioEventSeeking:
                {
                observer->SeekingEventL( iSeekingState, aErrorCode );
                break;
                }
            case ERadioEventRegion:
                {
                observer->RegionEventL( iSettings->EngineSettings().RegionId(), aErrorCode );
                break;
                }
            case ERadioEventFmTransmitter:
                {
                observer->FmTransmitterEventL( iFmTransmitterActive );
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Notifies the observer of a radio scan event.
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::NotifyRadioScanEvent( TRadioScanEvent aEvent,
                                            MRadioScanObserver& aObserver,
                                            TInt aError  )
    {
    TRAP_IGNORE( DoNotifyRadioScanEventL( aEvent, aObserver, aError ) )
    }

// ---------------------------------------------------------------------------
// Notifies the observer of a radio scan event.
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::DoNotifyRadioScanEventL( TRadioScanEvent aEvent,
                                               MRadioScanObserver& aObserver,
                                               TInt aError )
    {
    LOG_FORMAT( "CRadioEngineImp::DoNotifyRadioScanEventL, aEvent: %d, aError: %d", aEvent, aError );

    if ( aEvent == ERadioEventFrequencyScanned )
        {
        if ( aError == KErrNone )
            {
            aObserver.ScanFrequencyEventL( iPreviousScannedFrequency );
            }
        }
    else if ( aEvent == ERadioEventScanCompleted )
        {
        // KFmRadioErrTuning error means that no channels can be found anymore
        if ( aError == KFmRadioErrTuning )
            {
            aError = KErrNotFound;
            }

        aObserver.ScanCompletedEventL( aError );
        }
    }

// ---------------------------------------------------------------------------
// Handles some system events
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::HandleSystemEventL( TRadioSystemEventType aEventType )
    {
    LOG_FORMAT( "CRadioEngineImp::HandleSystemEventL, aEventType = %d", aEventType );

    switch ( aEventType )
        {
        case ERadioAudioResourcesAvailable:
            {
            LOG( "CRadioEngineImp::HandleSystemEventL, Audio resources available" );
            SwitchPower( ETrue );
            }
            break;

        case ERadioAudioAutoResumeForbidden:
            {
            EnableAudio( EFalse );
            }
            break;

        case ERadioAudioRouteHeadset:
            {
            HandleAudioRoutingEvent( RadioEngine::ERadioHeadset );
            }
            break;

        case ERadioAudioRouteSpeaker:
            {
            HandleAudioRoutingEvent( RadioEngine::ERadioSpeaker );
            }
            break;

        case ERadioCallActivated:
            {
            StopScan( KErrGeneral );
            CancelSeek();
            }
            break;

        case ERadioCallDeactivated:
            {
            SwitchPower( ETrue );
            }
            break;

        case ERadioHeadsetConnected:
            {
            // Explicitly set the audio routing to headset. Because system
            // forces the routing to headset anyway, and without our knowledge.
            iAudioRouter->SetAudioRouteL( RadioEngine::ERadioHeadset );
            }
            break;

        case ERadioHeadsetDisconnected:
            {
            // Explicitly set the audio routing to speaker. Because system
            // forces the routing to speaker anyway, if radio stays on.
            iAudioRouter->SetAudioRouteL( RadioEngine::ERadioSpeaker );
            }
            break;
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoRequestTunerControlComplete( TInt aError )
    {
    LOG_FORMAT( "CRadioEngineImp::MrftoRequestTunerControlComplete( %d )", aError );

    if ( aError == KErrNone || aError == KErrAlreadyExists ) // Tuner activated now or was already active
        {
        iRadioInitializationState = ERadioTunerControlGranted;

        TFmTunerCapabilities tunerCaps;
        tunerCaps.iTunerBands = 0;
        tunerCaps.iTunerFunctions = 0;
        tunerCaps.iAdditionalFunctions1 = 0;
        tunerCaps.iAdditionalFunctions2 = 0;
        iTunerUtility->GetCapabilities( tunerCaps );

#ifdef LOGGING_ENABLED
        TBuf<50> tunerCapsBuf;
        if ( tunerCaps.ETunerAvailableInOfflineMode )
            {
            tunerCapsBuf.Append( _L("OfflineMode ") );
            }
        if ( tunerCaps.ETunerRdsSupport )
            {
            tunerCapsBuf.Append( _L("RDS ") );
            }
        if ( tunerCaps.ETunerDualTunerSupport )
            {
            tunerCapsBuf.Append( _L("DualTuner ") );
            }
        LOG_FORMAT( "Radio tuner capabilities: %S", &tunerCapsBuf );

#endif // LOGGING_ENABLED

        TBool offlineAvailable( tunerCaps.iTunerFunctions &
                                TFmTunerCapabilities::ETunerAvailableInOfflineMode );
        iTunerUtility->EnableTunerInOfflineMode( offlineAvailable );

        TBool rdsSupported = EFalse;
        TRAP_IGNORE( CRadioRepositoryManager::GetRepositoryValueL( KRadioCRUid, KRadioCRRdsSupport,
                                                                   rdsSupported ) )
        if ( tunerCaps.iTunerFunctions & TFmTunerCapabilities::ETunerRdsSupport &&
                rdsSupported )
            {
            TRAP_IGNORE( iRdsReceiver->InitL( *iRadioUtility, iPubSub ) )
            }

        iTunerUtility->SetFrequencyRange( TunerFrequencyRangeForRegionId(
                                          iSettings->EngineSettings().RegionId() ) );
        // To prevent false frequency changes
        iTunerUtility->SetFrequency( iSettings->EngineSettings().TunedFrequency() );
        SwitchPower( ETrue );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoSetFrequencyRangeComplete( TInt aError )
    {
    LOG_FORMAT( "CRadioEngineImp::MrftoSetFrequencyRangeComplete( %d )", aError );
    if ( aError )
        {
        NotifyRadioEvent( ERadioEventRegion, aError );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoSetFrequencyComplete( TInt aError )
    {
    LOG_FORMAT( "CRadioEngineImp::MrftoSetFrequencyComplete: Err: %d", aError );

    if ( aError )
        {
        if ( aError == KErrNotReady )
            {
            iRadioInitializationState = ERadioUtilitiesConstructed;
            }
        HandleFrequencyEvent( iSettings->EngineSettings().TunedFrequency(), aError );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoStationSeekComplete( TInt aError, TInt aFrequency )
    {
    LOG_FORMAT( "CRadioEngineImp::MrftoStationSeekComplete() -- aError = %d, aFrequency = %d", aError, aFrequency );
    // Seeking has ended, error code tells if it was successful

    if ( aError == KFmRadioErrAntennaNotConnected )
        {
        iAntennaAttached = EFalse;
        }
    else if ( aError == KErrNotReady )
        {
        iRadioInitializationState = ERadioUtilitiesConstructed;
        }

    iSeekingState = RadioEngine::ERadioNotSeeking;

    NotifyRadioEvent( ERadioEventSeeking, aError );

//    if ( aError != KErrNone )
//        {
//        iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonImplicit;
//        NotifyRadioEvent( ERadioEventFrequency, KErrNone ); // Frequency change is not otherwise notified when seeking fails.
//        }
//    else
//        {
//        // sometimes frequency change is not reported even if seeking succeeds
//        if ( !iFrequencySetByRdsAf )
//            {
//            NotifyRadioEvent( ERadioEventFrequency, KErrNone );
//            }
//        }

    if ( iScanObserver )
        {
        if ( aError != KErrNone )
            {
            StopScan( aError );
            }
        else
            {
            if ( aFrequency > iSettings->EngineSettings().MinFrequency() && aFrequency > iPreviousScannedFrequency )
                {
                iPreviousScannedFrequency = aFrequency;
                NotifyRadioScanEvent( ERadioEventFrequencyScanned, *iScanObserver, KErrNone );
                Seek( RadioEngine::ERadioUp ); // Continue scanning.
                }
            else
                {
                if ( aFrequency == iSettings->EngineSettings().MinFrequency() )
                    {
                    iPreviousScannedFrequency = aFrequency;
                    NotifyRadioScanEvent( ERadioEventFrequencyScanned, *iScanObserver, KErrNone );
                    }
                StopScan( aError );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoFmTransmitterStatusChange( TBool aActive )
    {
    LOG_FORMAT( "CRadioEngineImp::MrftoFmTransmitterStatusChange( %d )", aActive );
    iFmTransmitterActive = aActive;

    if ( !iFmTransmitterActive )
        {
        SwitchPower( ETrue );
        }

    NotifyRadioEvent( ERadioEventFmTransmitter );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoAntennaStatusChange( TBool aAttached )
    {
    LOG_FORMAT( "CRadioEngineImp::MrftoAntennaStatusChange( %d )", aAttached );
    iAntennaAttached = aAttached;
    if ( iAntennaAttached )
        {
        SwitchPower( ETrue );
        NotifyRadioEvent( ERadioEventAntenna );
        iPubSub->PublishAntennaState( ERadioPSRadioAntennaAttached );
        }
    else
        {
        NotifyRadioEvent( ERadioEventAntenna, KErrDisconnected );
        iPubSub->PublishAntennaState( ERadioPSRadioAntennaDetached );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

//
void CRadioEngineImp::MrftoOfflineModeStatusChange( TBool DEBUGVAR( aOfflineMode ) )
    {
    LOG_FORMAT( "CRadioEngineImp::MrftoOfflineModeStatusChange( %d )", aOfflineMode );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoFrequencyRangeChange( TFmRadioFrequencyRange DEBUGVAR( aBand ) )
    {
    LOG_FORMAT( "CRadioEngineImp::MrftoFrequencyRangeChange( %d )", aBand );
    if ( RadioInitialized() )
        {
        iTunerUtility->SetFrequency( iSettings->EngineSettings().TunedFrequency() );
        iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonImplicit;
        }
    NotifyRadioEvent( ERadioEventRegion );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoFrequencyChange( TInt aNewFrequency )
    {
    LOG_FORMAT( "CRadioEngineImp::MrftoFrequencyChange aNewFrequency = %u", aNewFrequency );

    // There may be frequency changed events when radio is not initialized ( because
    // of SetFrequency or Seek returns with KErrNotReady ).
    if ( RadioInitialized() )
        {
      
        HandleFrequencyEvent( aNewFrequency );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoForcedMonoChange( TBool aForcedMono )
    {
    LOG_FORMAT( "CRadioEngineImp::MrftoForcedMonoChange -- aForcedMono = %d", aForcedMono );

    iSettings->RadioSetter().SetOutputMode( aForcedMono ? RadioEngine::ERadioMono : RadioEngine::ERadioStereo );
    NotifyRadioEvent( ERadioEventAudioMode );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoSquelchChange( TBool /*aSquelch*/ )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrpoStateChange( TPlayerState aState, TInt aError )
    {
    LOG_FORMAT( "CRadioEngineImp::MrpoStateChange() -- aState = %d, aError = %d", aState, aError );

    if ( aError == KFmRadioErrAntennaNotConnected )
        {
        iAntennaAttached = EFalse;
        }
    HandlePowerEvent( aState == ERadioPlayerPlaying, aError );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrpoVolumeChange( TInt aVolume )
    {
    aVolume = aVolume / KRadioVolumeStepsDivider;
    LOG_FORMAT( "CRadioEngineImp::MrpoVolumeChange() --  volume = %d", aVolume );
    if ( aVolume != iSettings->EngineSettings().Volume() )
        {
        iSettings->RadioSetter().SetVolume( aVolume );
        NotifyRadioEvent( ERadioEventVolume );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrpoMuteChange( TBool aMute )
    {
    TBool muted = iSettings->EngineSettings().IsVolMuted();
    if ( !iScanObserver && !aMute != !muted )
        {
        iSettings->RadioSetter().SetVolMuted( aMute );
        NotifyRadioEvent( ERadioEventMute );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrpoBalanceChange( TInt /*aLeftPercentage*/, TInt /*aRightPercentage*/ )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::RdsAfSearchSettingChangedL( TBool aEnabled )
    {
    LOG_FORMAT( "CRadioEngineImp::RdsAfSearchSettingChangedL( %d )", aEnabled );
    iRdsReceiver->SetAutomaticSwitchingL( aEnabled );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::RegionSettingChangedL( TInt DEBUGVAR( aRegion ) )
    {
    LOG_FORMAT( "CRadioEngineImp::RegionSettingChangedL( %d )", aRegion );

    if ( RadioInitialized() )
        {
        iTunerUtility->SetFrequencyRange( TunerFrequencyRangeForRegionId( iSettings->EngineSettings().RegionId() ) );
        iTunerUtility->SetFrequency( iSettings->EngineSettings().TunedFrequency() );
        iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonImplicit;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::RdsAfSearchBegin()
    {
    LOG( "CRadioEngineImp::RdsAfSearchBegin()" );
    iFrequencySetByRdsAf = ETrue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::RdsAfSearchEnd( TUint32 DEBUGVAR( aFrequency ), TInt aError )
    {
    LOG_FORMAT( "CRadioEngineImp::RdsAfSearchEnd( %d, %d )", aFrequency, aError );
    if ( aError != KErrNone )
        {
        iFrequencySetByRdsAf = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// Tries to figure the region out based on current mobile network id
// ---------------------------------------------------------------------------
//
TRadioRegion CRadioEngineImp::RegionFromMobileNetwork() const
    {
    TRadioRegion region = ERadioRegionNone;

    // Choose the frequency range according to country code
    MRadioEngineSettings& engineSettings = iSettings->EngineSettings();
    const TDesC& countryCode = engineSettings.CountryCode();
    const TInt regionCount = engineSettings.CountRegions();
    TBool matchFound = EFalse;
    for ( TInt i = 0; i < regionCount && !matchFound; ++i )
        {
        const RRadioCountryCodeArray& regionCountryCodes =
                engineSettings.Region( i ).CountryCodes();

        const TInt countryCodeCount = regionCountryCodes.Count();
        for ( TInt j = 0; j < countryCodeCount && !matchFound; ++j )
            {
            if ( countryCode == *regionCountryCodes[j] )
                {
                // We have a match
                matchFound = ETrue;
                region = engineSettings.Region( i ).Id();
                }
            }
        }

    return region;
    }

// ---------------------------------------------------------------------------
// Tries to figure the region out based on timezone selection
// ---------------------------------------------------------------------------
//
TRadioRegion CRadioEngineImp::RegionFromTimezone() const
    {
    TRadioRegion region = ERadioRegionNone;
    TRAP_IGNORE( region = DoRegionFromTimezoneL() );
    return region;
    }

// ---------------------------------------------------------------------------
// Performs the timezone-based check
// ---------------------------------------------------------------------------
//
TRadioRegion CRadioEngineImp::DoRegionFromTimezoneL() const
    {
    LOG_METHOD_AUTO;
    CTzLocalizer* timezoneLocalizer = CTzLocalizer::NewL();
    CleanupStack::PushL( timezoneLocalizer );

    CTzLocalizedCityGroupArray* cityGroups = timezoneLocalizer->GetAllCityGroupsL(
                                            CTzLocalizer::ETzAlphaNameAscending );
    CleanupStack::PushL( cityGroups );

    // We get the ownership so we must delete
    CTzLocalizedCity* city = timezoneLocalizer->GetFrequentlyUsedZoneCityL( CTzLocalizedTimeZone::ECurrentZone );
    const TUint8 cityId = city->GroupId();
    delete city;
    city = NULL;
    LOG_FORMAT( "CRadioEngineHandler::CurrentTimeZoneToRegionL group id: %d", cityId );

    TRadioRegion region = ERadioRegionNone;
    const TInt cityGroupCount = cityGroups->Count();
    TBool found = EFalse;
    for ( TInt i = 0; i < cityGroupCount && !found; ++i )
        {
        if ( cityId == cityGroups->At( i ).Id() )
            {
            region = static_cast<TRadioRegion>( KCityGroupRegions[ cityId - 1 ] );
            found = ETrue;
            }
        }

    CleanupStack::PopAndDestroy( cityGroups );
    CleanupStack::PopAndDestroy( timezoneLocalizer );

    LOG_ASSERT( found, LOG_FORMAT( "CRadioEngine::DoMapCurrentTimeZoneToRegionL. City not found: %d", cityId ) );

    return region;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::HandleAudioRoutingEvent( RadioEngine::TRadioAudioRoute aDestination )
    {
    LOG_FORMAT( "CRadioEngineImp::HandleAudioRoutingL( %d )", aDestination );

    // Make modifications to volume ONLY if new audio source state
    // differs from settings. If they don't differ, this state
    // change is a part of the radio initialization.
    MRadioSettingsSetter& setter = iSettings->RadioSetter();
    if ( aDestination != iSettings->EngineSettings().AudioRoute() )
        {
        setter.SetAudioRoute( aDestination );

        // If audio muted, change it to minimum volume
        MRadioEngineSettings& engineSettings = iSettings->EngineSettings();
        TInt vol = engineSettings.IsVolMuted() ? engineSettings.DefaultMinVolumeLevel()
                                               : engineSettings.Volume();

        if ( RadioInitialized() )
            {
            TInt err = iPlayerUtility->Mute( EFalse );
            if ( !err )
                {
                setter.SetVolMuted( EFalse );
                err = iPlayerUtility->SetVolume( TunerVolumeForUiVolume( vol ));

                if ( !err )
                    {
                    setter.SetVolume( vol );
                    }
                }
            }
        else
            {
            setter.SetVolMuted( EFalse );
            setter.SetVolume( vol );
            }
        NotifyRadioEvent( ERadioEventAudioRouting );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::HandlePowerEvent( TBool aPowerOn, TInt aErrorCode )
    {
    LOG_FORMAT( "CRadioEngineImp::HandlePowerEvent( %d, %d )", aPowerOn, aErrorCode );

    const TBool powerState = iSettings->EngineSettings().IsPowerOn();
    if ( !powerState != !aPowerOn )
        {
        iSettings->RadioSetter().SetPowerOn( aPowerOn );

        if ( aPowerOn )
            {
            iRdsReceiver->StartReceiver();
            }
        else
            {
            iRdsReceiver->StopReceiver();
            }
        }

    if ( !iSettings->EngineSettings().IsPowerOn() )
        {
        StopScan( aErrorCode );
        CancelSeek();
        }

    // If we are seeking, power event starts seeking
    if ( iSeekingState != RadioEngine::ERadioNotSeeking && iSettings->EngineSettings().IsPowerOn() )
        {
        // Reset seeking state to enable seeking start
        RadioEngine::TRadioSeeking oldSeeking = iSeekingState;
        iSeekingState = RadioEngine::ERadioNotSeeking;
        Seek( oldSeeking == RadioEngine::ERadioSeekingUp ? RadioEngine::ERadioUp : RadioEngine::ERadioDown );
        }

    if ( !powerState != !aPowerOn )
        {
        NotifyRadioEvent( ERadioEventPower, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::HandleFrequencyEvent( TUint32 aFrequency, TInt aErrorCode )
    {
    LOG_FORMAT( "CRadioEngineImp::HandleFrequencyEvent( %d, %d )", aFrequency, aErrorCode );

    if ( iSettings->EngineSettings().TunedFrequency() != aFrequency )
        {
        if ( iSettings->EngineSettings().IsPowerOn() )
            {
            if ( !OkToPlay( aFrequency ) )
                {
                // Radio is going to be powered off, stop rds receiver immediately because
                // some rds data from previous channel might come before power off event.
                iRdsReceiver->StopReceiver();
                }
            else
                {
                // Normal frequency change, make sure that rds receiver is started
                iRdsReceiver->StartReceiver();
                }
            }
        if ( !iFrequencySetByRdsAf )
            {
            iRdsReceiver->ClearRdsInformation();
            }
        }
    else if ( iFrequencySetByRdsAf )
        {
        // frequency didn't change, so AF search didn't complete successfully
        iFrequencySetByRdsAf = EFalse;
        }

    iSettings->RadioSetter().SetTunedFrequency( aFrequency );

    NotifyRadioEvent( ERadioEventFrequency, aErrorCode );
    iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonUnknown;

    if ( aErrorCode == KErrNone )
        {
        SwitchPower( iRadioEnabled && OkToPlay( aFrequency ) );
        }
    }

// ---------------------------------------------------------------------------
// Routing is not possible when headset is not available, power is off or
// audio routing is not supported.
// ---------------------------------------------------------------------------
//
TBool CRadioEngineImp::IsAudioRoutingPossible() const
    {
    TBool headsetConnected = EFalse;
    TRAP_IGNORE( ( headsetConnected = iSystemEventCollector->IsHeadsetConnectedL() ) )

    TBool powerOn = iSettings->EngineSettings().IsPowerOn();

    TBool isAudioRoutingPossible = EFalse;
    if ( headsetConnected && powerOn && AudioRoutingSupported() )
        {
        isAudioRoutingPossible = ETrue;
        }

    return isAudioRoutingPossible;
    }

