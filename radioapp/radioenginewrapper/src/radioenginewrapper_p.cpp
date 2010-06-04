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

// User includes
#include "radioenginewrapper_p.h"
#include "radiosettings.h"
#include "radiosettings_p.h"
#include "radiologger.h"
#include "radio_global.h"
#include "cradioenginehandler.h"
#include "radiostationhandlerif.h"
#include "cradiopubsub.h"
#include "radiocontroleventlistener.h"
#include "radiordslistener.h"
#include "cradiorepositorymanager.h"
#include "radioenginewrapperobserver.h"

// Constants

/*!
 *
 */
RadioEngineWrapperPrivate::RadioEngineWrapperPrivate( RadioEngineWrapper* wrapper,
                                                      RadioStationHandlerIf& stationHandler ) :
    q_ptr( wrapper ),
    mStationHandler( stationHandler ),
    mEngineHandler( new CRadioEngineHandler( *this ) ),
    mControlEventListener( new RadioControlEventListener( *this ) ),
    mRdsListener ( new RadioRdsListener( mStationHandler, *this ) ),
    mTuneReason( 0 ),
    mUseLoudspeaker( false )
{
}

/*!
 *
 */
RadioEngineWrapperPrivate::~RadioEngineWrapperPrivate()
{
    // Destructor needs to be defined because some member variables that are forward declared
    // in the header are managed by QT's smart pointers and they require that the owning class
    // has a non-inlined destructor. Compiler generates an inlined destructor if it isn't defined.
}

/*!
 * Initializes the private implementation
 */
void RadioEngineWrapperPrivate::init()
{
    TRAPD( err, mEngineHandler->ConstructL() );
    if ( err != KErrNone ) {
        LOG_FORMAT( "RadioEngineWrapperPrivate::init, EngineHandler construct failed: %d", err );
        mEngineHandler.reset();
        return;
        //TODO: Error handling?
    }

    mEngineHandler->SetRdsObserver( mRdsListener.data() );
    mEngineHandler->PubSub().SetControlEventObserver( mControlEventListener.data() );
    mControlEventListener->init();

    // Start observing profile changes
    mEngineHandler->Repository().AddEntityL( KCRUidProfileEngine,
                                             KProEngActiveProfile,
                                             CRadioRepositoryManager::ERadioEntityInt );

    mUseLoudspeaker = mEngineHandler->IsAudioRoutedToLoudspeaker();
    if ( !mUseLoudspeaker ) {
        RUN_NOTIFY_LOOP( mObservers, audioRouteChanged( false ) );
    }
}

/*!
 * Starts up the radio engine
 */
bool RadioEngineWrapperPrivate::isEngineConstructed()
{
    return mEngineHandler != 0;
}

/*!
 * Returns the settings handler owned by the engine
 */
RadioSettingsIf& RadioEngineWrapperPrivate::settings()
{
    if ( !mSettings ) {
        mSettings.reset( new RadioSettings() );
        mSettings->d_func()->init( &mEngineHandler->ApplicationSettings() );
    }
    return *mSettings;
}

/*!
 * Returns the enginehandler owned by the engine
 */
CRadioEngineHandler& RadioEngineWrapperPrivate::RadioEnginehandler()
{
    return *mEngineHandler;
}

/*!
 * Tunes to the given frequency
 */
void RadioEngineWrapperPrivate::tuneFrequency( uint frequency, const int reason )
{
    if ( mEngineHandler->TunedFrequency() != frequency ) {
        mTuneReason = reason;
        mEngineHandler->Tune( frequency );
    }
}

/*!
 * Tunes to the given frequency after a delay
 */
void RadioEngineWrapperPrivate::tuneWithDelay( uint frequency, const int reason )
{
    if ( mEngineHandler->TunedFrequency() != frequency ) {
        mTuneReason = reason;
        mEngineHandler->TuneWithDelay( frequency );
    }
}

/*!
 *
 */
ObserverList& RadioEngineWrapperPrivate::observers()
{
    return mObservers;
}

/*!
 *
 */
void RadioEngineWrapperPrivate::startSeeking( Seeking::Direction direction, const int reason )
{
    mTuneReason = reason;
    mEngineHandler->Seek( direction );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::PowerEventL( TBool aPowerState, TInt DEBUGVAR( aError ) )
{
    LOG_FORMAT( "RadioEngineWrapperPrivate::PowerEventL, PowerState: %d, Error: %d", aPowerState, aError );
    RUN_NOTIFY_LOOP( mObservers, radioStatusChanged( aPowerState ) );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::FrequencyEventL( TUint32 aFrequency,
                                                 RadioEngine::TRadioFrequencyEventReason aReason,
                                                 TInt aError )
{
    Q_UNUSED( aReason );
    LOG_FORMAT( "RadioEngineWrapperPrivate::FrequencyEventL - Frequency: %d, Reason: %d, Error: %d", aFrequency, aReason, aError );

    if ( !aError ) {
        const uint frequency = static_cast<uint>( aFrequency );
        RUN_NOTIFY_LOOP( mObservers, tunedToFrequency( frequency, mTuneReason ) );
    }
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::VolumeEventL( TInt aVolume, TInt aError )
{
    Q_UNUSED( aError );
    RUN_NOTIFY_LOOP( mObservers, volumeChanged( aVolume ) );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::MuteEventL( TBool aMuteState, TInt aError )
{
    Q_UNUSED( aError );
    RUN_NOTIFY_LOOP( mObservers, muteChanged( aMuteState ) );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::AudioModeEventL( TInt DEBUGVAR( aAudioMode ), TInt DEBUGVAR( aError ) )
{
    LOG_FORMAT( "RadioEngineWrapperPrivate::AudioModeEventL, AudioMode: %d, Error: %d", aAudioMode, aError );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::AntennaEventL( TBool aAntennaAttached, TInt aError )
{
    Q_UNUSED( aError );
    RUN_NOTIFY_LOOP( mObservers, antennaStatusChanged( aAntennaAttached ) );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::AudioRoutingEventL( TInt aAudioDestination, TInt aError )
{
    Q_UNUSED( aAudioDestination )
    Q_UNUSED( aError )
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::SeekingEventL( TInt aSeekingState, TInt aError )
{
    Q_UNUSED( aSeekingState );
    Q_UNUSED( aError );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::RegionEventL( TInt DEBUGVAR( aRegion ), TInt DEBUGVAR( aError ) )
{
    LOG_FORMAT( "RadioEngineWrapperPrivate::RegionEventL, aRegion: %d, Error: %d", aRegion, aError );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::AudioRouteChangedL( RadioEngine::TRadioAudioRoute aRoute )
{
    mUseLoudspeaker = aRoute == RadioEngine::ERadioSpeaker;
    RUN_NOTIFY_LOOP( mObservers, audioRouteChanged( mUseLoudspeaker ) );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::HandleSystemEventL( TRadioSystemEventType DEBUGVAR( aEventType ) )
{
    LOG_FORMAT( "RadioEngineWrapperPrivate::HandleSystemEventL, Event: %d", aEventType );
//    ERadioHeadsetConnected,         ///< Headset was connected
//    ERadioHeadsetDisconnected,      ///< Headset was disconnected
//    ERadioNetworkCoverageUp,        ///< Network coverage detected
//    ERadioNetworkCoverageDown,      ///< Network coverage lost
//    ERadioCallActivated,            ///< Call activated or ringing
//    ERadioCallDeactivated,          ///< Call disconnected
//    ERadioEmergencyCallActivated,   ///< Call activated or ringing
//    ERadioEmergencyCallDeactivated, ///< Call disconnected
//    ERadioLowDiskSpace,             ///< Low disk space
//    ERadioAudioRoutingHeadset,      ///< Audio routed through headset
//    ERadioAudioRoutingSpeaker,      ///< Audio routed through speaker ( IHF )
//    ERadioAudioResourcesAvailable,  ///< Audio resources have become available
//    ERadioAudioAutoResumeForbidden  ///< Audio auto resuming is forbidden
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::HandleRepositoryValueChangeL( const TUid& aUid, TUint32 aKey, TInt aValue, TInt aError )
{
    if ( aUid == KCRUidProfileEngine && aKey == KProEngActiveProfile && !aError && aValue == KOfflineProfileId ) {
        LOG( "RadioEngineWrapperPrivate::HandleRepositoryValueChangeL: Offline profile activated" );
    }
}
