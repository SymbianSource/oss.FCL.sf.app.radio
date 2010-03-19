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


#ifdef USE_STUBBED_RADIOENGINE
#   include "t_cradioengine.h"
#   include "t_mrdshandler.h"
#else
#   include "cradioengine.h"
#   include "cradioaudiorouter.h"
#   include "cradiosystemeventcollector.h"
#   include "cradiosettings.h"
#   include "mradioenginesettings.h"
#   include "cradiopubsub.h"
#   include "cradiorepositorymanager.h"
#   include "mradiordsreceiver.h"
#endif

#include "cradioenginehandler.h"
#include "mradioenginehandlerobserver.h"
#include "radio_global.h"
#include "radiologger.h"
#include "radioenummapper.h"

/**
 * Delayed tuning delay
 */
const TInt KTuneDelay = 100000;

/*!
 * Map to translate seek direction enum from its definition in the engine to
 * its definition in the ui and vice versa
 */
BEGIN_ENUM_MAP( KSeekDirectionMap )
    ENUM_MAP_ITEM( Seeking::Down, RadioEngine::ERadioDown ),
    ENUM_MAP_ITEM( Seeking::Up,   RadioEngine::ERadioUp )
END_ENUM_MAP( KSeekDirectionMap )

/*!
 * Convenience macro to do the mapping of seek directions
 */
#define MAP_FROM_UI_DIRECTION(ui_enum) MAP_FROM_UI_ENUM( RadioEngine::TRadioTuneDirection, ui_enum, KSeekDirectionMap )

/*!
 * Map to translate radio region enum from its definition in the engine to
 * its definition in the ui and vice versa
 */
BEGIN_ENUM_MAP( KRegionMap )
    ENUM_MAP_ITEM( RadioRegion::None,       ERadioRegionNone ),
    ENUM_MAP_ITEM( RadioRegion::Default,    ERadioRegionDefault ),
    ENUM_MAP_ITEM( RadioRegion::Japan,      ERadioRegionJapan ),
    ENUM_MAP_ITEM( RadioRegion::America,    ERadioRegionAmerica ),
    ENUM_MAP_ITEM( RadioRegion::Poland,     ERadioRegionPoland ),
END_ENUM_MAP( KRegionMap )

/*!
 * Convenience macros to do the mapping of radio regions
 */
#define MAP_FROM_UI_REGION(ui_enum) MAP_FROM_UI_ENUM( TRadioRegion, ui_enum, KRegionMap )
#define MAP_TO_UI_REGION(engine_enum) MAP_TO_UI_ENUM( RadioRegion::Region, engine_enum, KRegionMap )

/*!
 * Map to translate seeking state enum from its definition in the engine to
 * its definition in the ui and vice versa
 */
BEGIN_ENUM_MAP( KSeekingStateMap )
    ENUM_MAP_ITEM( Seeking::NotSeeking,     RadioEngine::ERadioNotSeeking ),
    ENUM_MAP_ITEM( Seeking::SeekingUp,      RadioEngine::ERadioSeekingUp ),
    ENUM_MAP_ITEM( Seeking::SeekingDown,    RadioEngine::ERadioSeekingDown )
END_ENUM_MAP( KSeekingStateMap )

/*!
 * Convenience macro to do the mapping of seeking states
 */
#define MAP_TO_UI_SEEKING_STATE(ui_enum) MAP_TO_UI_ENUM( Seeking::State, ui_enum, KSeekingStateMap )


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineHandler::CRadioEngineHandler( MRadioEngineHandlerObserver& aObserver )
    : iObserver( aObserver )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineHandler::~CRadioEngineHandler()
    {
    if ( iEngine ) {
        iEngine->Settings().Repository().RemoveObserver( &iObserver );
    }

    delete iEngine;
    delete iDelayTimer;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineHandler::ConstructL()
    {
    LOG_METHOD;

    iDelayTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    iEngine = CRadioEngine::NewL( *this );

    iEngine->SystemEventCollector().AddObserverL( &iObserver );
    iEngine->Settings().Repository().AddObserverL( &iObserver );
    iEngine->AudioRouter().SetAudioRouteL( RadioEngine::ERadioHeadset );
    iEngine->AddObserverL( &iObserver );

    iRegion = MAP_TO_UI_REGION( iEngine->Settings().EngineSettings().RegionId() );

//    iEngineHolder->PubSub().SetControlEventObserver( &iObserver );

//    iRadioEngine->SetVolumeMuted( EFalse );
//    iRadioEngine->SetVolume( MaxVolume() );

//    CVRRepositoryManager::GetRepositoryValueL( KVRCRUid, KVRCRLaunchCount, usageCount );
//    CVRRepositoryManager::SetRepositoryValueL( KVRCRUid, KVRCRLaunchCount, ++usageCount );
//    iControlEventObserver = CVRControlEventObserverImpl::NewL( *this );
//    doc->PubSubL().SetControlEventObserver( iControlEventObserver );
    }

// ---------------------------------------------------------------------------
// Sets the rds data observer
// ---------------------------------------------------------------------------
//
void CRadioEngineHandler::SetRdsObserver( MRadioRdsDataObserver* aObserver )
    {
    TRAP_IGNORE( iEngine->RdsReceiver().AddObserverL( aObserver ) );
    }

// ---------------------------------------------------------------------------
// Returns the radio status.
// ---------------------------------------------------------------------------
//
TBool CRadioEngineHandler::IsRadioOn()
    {
    return iEngine->Settings().EngineSettings().IsPowerOn();
    }

// ---------------------------------------------------------------------------
// Tune to the specified frequency
// ---------------------------------------------------------------------------
//
void CRadioEngineHandler::Tune( TUint aFrequency )
    {
    iEngine->SetFrequency( aFrequency );
    }

// ---------------------------------------------------------------------------
// Tune to the specified frequency after a delay
// ---------------------------------------------------------------------------
//
void CRadioEngineHandler::TuneWithDelay( TUint aFrequency )
    {
    iFrequency = aFrequency;
    iDelayTimer->Cancel();
    iDelayTimer->Start( KTuneDelay, KTuneDelay, TCallBack( TuneDelayCallback, this ) );
    }

// ---------------------------------------------------------------------------
// Sets the audio mute state
// ---------------------------------------------------------------------------
//
void CRadioEngineHandler::SetMuted( const TBool aMuted )
    {
    iEngine->SetVolumeMuted( aMuted );
    }

// ---------------------------------------------------------------------------
// Gets the audio mute state
// ---------------------------------------------------------------------------
//
TBool CRadioEngineHandler::IsMuted() const
    {
    return iEngine->Settings().EngineSettings().IsVolMuted();
    }

// ---------------------------------------------------------------------------
// Sets the volume level of the FM radio
// ---------------------------------------------------------------------------
//
void CRadioEngineHandler::SetVolume( TInt aVolume )
    {
    if ( Volume() != aVolume )
        {
        if ( aVolume > 0 )
            {
            iEngine->SetVolumeMuted( EFalse );
            }

        iEngine->SetVolume( aVolume );
        }
    }

// ---------------------------------------------------------------------------
// Gets the volumelevel.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineHandler::Volume() const
    {
    return iEngine->Settings().EngineSettings().Volume();
    }

// ---------------------------------------------------------------------------
// Gets the max volumelevel.
// ---------------------------------------------------------------------------
//
TInt CRadioEngineHandler::MaxVolume() const
    {
    return iEngine->MaxVolumeLevel();
    }

// ---------------------------------------------------------------------------
// Increases the volume by one increment
// ---------------------------------------------------------------------------
//
void CRadioEngineHandler::IncreaseVolume()
    {
    iEngine->AdjustVolume( RadioEngine::ERadioIncVolume );
    }

// ---------------------------------------------------------------------------
// Decreases the volume by one increment
// ---------------------------------------------------------------------------
//
void CRadioEngineHandler::DecreaseVolume()
    {
    iEngine->AdjustVolume( RadioEngine::ERadioDecVolume );
    }

// ---------------------------------------------------------------------------
// Checks if the antenna is attached
// ---------------------------------------------------------------------------
//
TBool CRadioEngineHandler::IsAntennaAttached() const
    {
    return iEngine->IsAntennaAttached();
    }

// ---------------------------------------------------------------------------
// Retrieves the current frequency.
// ---------------------------------------------------------------------------
//
TUint CRadioEngineHandler::TunedFrequency() const
    {
    return iEngine->Settings().EngineSettings().TunedFrequency();
    }

// ---------------------------------------------------------------------------
// Returns the minimum allowed frequency in the current region
// ---------------------------------------------------------------------------
//
TUint CRadioEngineHandler::MinFrequency() const
    {
    return iEngine->Settings().EngineSettings().MinFrequency();
    }

// ---------------------------------------------------------------------------
// Returns the maximum allowed frequency in the current region
// ---------------------------------------------------------------------------
//
TUint CRadioEngineHandler::MaxFrequency() const
    {
    return iEngine->Settings().EngineSettings().MaxFrequency();
    }

// ---------------------------------------------------------------------------
// Checks if the given frequency is valid in the current region
// ---------------------------------------------------------------------------
//
TBool CRadioEngineHandler::IsFrequencyValid( TUint aFrequency ) const
    {
    return iEngine->IsFrequencyValid( aFrequency );
    }

// ---------------------------------------------------------------------------
// Scan up to the next available frequency
// ---------------------------------------------------------------------------
//
void CRadioEngineHandler::Seek( Seeking::Direction direction )
    {
    LOG_TIMESTAMP( "Seek" );
    iEngine->Seek( MAP_FROM_UI_DIRECTION( direction ) );
    }

// ---------------------------------------------------------------------------
// Cancel previously requested scan, and return to the
// already tuned frequency
// ---------------------------------------------------------------------------
//
void CRadioEngineHandler::CancelSeek()
    {
    iEngine->CancelSeek();
    }

// ---------------------------------------------------------------------------
// Returns the engine seeking state
// ---------------------------------------------------------------------------
//
Seeking::State CRadioEngineHandler::SeekingState() const
    {
    return MAP_TO_UI_SEEKING_STATE( iEngine->Seeking() );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineHandler::StartScan( MRadioScanObserver& aObserver )
    {
    iEngine->StartScan( aObserver );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineHandler::StopScan( TInt aError )
    {
    iEngine->StopScan( aError );
    }

// ---------------------------------------------------------------------------
// return step size for tuning.
// ---------------------------------------------------------------------------
//
TUint32 CRadioEngineHandler::FrequencyStepSize() const
    {
    return iEngine->Settings().EngineSettings().FrequencyStepSize();
    }

// ----------------------------------------------------------------------------------------------------
// Returns the selected radio region
// ----------------------------------------------------------------------------------------------------
//
RadioRegion::Region CRadioEngineHandler::Region() const
    {
    return iRegion;
    }

// ---------------------------------------------------------------------------
// Sets whether or not audio should be routed to loudspeaker
// ---------------------------------------------------------------------------
//
void CRadioEngineHandler::SetAudioRouteToLoudspeaker( TBool aLoudspeaker )
    {
    TRAPD( err, iEngine->AudioRouter().SetAudioRouteL( aLoudspeaker ? RadioEngine::ERadioSpeaker
                                                         : RadioEngine::ERadioHeadset ) );
    if ( err )
        {
        LOG_FORMAT( "Failed to set audioroute: UseLoudspeadker: %d", aLoudspeaker );
        }
    }

// ---------------------------------------------------------------------------
// Checks if audio is routed to loudspeaker
// ---------------------------------------------------------------------------
//
TBool CRadioEngineHandler::IsAudioRoutedToLoudspeaker() const
    {
    return iEngine->Settings().EngineSettings().AudioRoute() == RadioEngine::ERadioSpeaker;
    }

// ---------------------------------------------------------------------------
// Returns ar reference to the publish & subscribe handler
// ---------------------------------------------------------------------------
//
CRadioPubSub& CRadioEngineHandler::PubSub()
    {
    return *iEngine->PubSub();
    }

// ---------------------------------------------------------------------------
// Returns the repository manager.
// ---------------------------------------------------------------------------
//
CRadioRepositoryManager& CRadioEngineHandler::Repository() const
    {
    return iEngine->Settings().Repository();
    }

// ---------------------------------------------------------------------------
// Returns the repository manager.
// ---------------------------------------------------------------------------
//
MRadioApplicationSettings& CRadioEngineHandler::ApplicationSettings() const
    {
    return iEngine->Settings().ApplicationSettings();
    }

// ---------------------------------------------------------------------------
// From MRadioEngineInitializer
//
// ---------------------------------------------------------------------------
//
CRadioAudioRouter* CRadioEngineHandler::InitAudioRouterL()
    {
    return CRadioAudioRouter::NewL( iObserver );
    }

// ---------------------------------------------------------------------------
// From MRadioEngineInitializer
//
// ---------------------------------------------------------------------------
//
CRadioSystemEventCollector* CRadioEngineHandler::InitSystemEventCollectorL()
    {
    return CRadioSystemEventCollector::NewL();
    }

// ---------------------------------------------------------------------------
// From MRadioEngineInitializer
//
// ---------------------------------------------------------------------------
//
CRadioSettings* CRadioEngineHandler::InitSettingsL()
    {
    return CRadioSettings::NewL();
    }

// ---------------------------------------------------------------------------
// From MRadioEngineInitializer
//
// ---------------------------------------------------------------------------
//
CRadioPubSub* CRadioEngineHandler::InitPubSubL()
    {
    return CRadioPubSub::NewL();
    }

// --------------------------------------------------------------------------------
// Static callback function to be used by the tune delay timer
// --------------------------------------------------------------------------------
//
TInt CRadioEngineHandler::TuneDelayCallback( TAny* aSelf )
    {
    CRadioEngineHandler* self = static_cast<CRadioEngineHandler*>( aSelf );
    self->iDelayTimer->Cancel();
    self->Tune( self->iFrequency );
    return 0; // Not used by CPeriodic
    }
