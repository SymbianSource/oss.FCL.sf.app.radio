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
#include "radiofrequencyscanninghandler.h"
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
                                                      RadioStationHandlerIf& stationHandler,
                                                      RadioEngineWrapperObserver& observer ) :
    q_ptr( wrapper ),
    mStationHandler( stationHandler ),
    mObserver( observer ),
    mEngineHandler( new CRadioEngineHandler( *this ) ),
    mControlEventListener( new RadioControlEventListener( *this ) ),
    mRdsListener ( new RadioRdsListener( mStationHandler, *this ) ),
    mCommandSender( 0 ),
    mUseLoudspeaker( false ),
    mIsSeeking( false )
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
        mObserver.audioRouteChanged( false );
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
RadioSettings& RadioEngineWrapperPrivate::settings()
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
void RadioEngineWrapperPrivate::tuneFrequency( uint frequency, const int sender )
{
    mCommandSender = sender;
    mEngineHandler->Tune( frequency );
}

/*!
 * Tunes to the given frequency after a delay
 */
void RadioEngineWrapperPrivate::tuneWithDelay( uint frequency, const int sender )
{
    mCommandSender = sender;
    mEngineHandler->TuneWithDelay( frequency );
}

/*!
 *
 */
RadioEngineWrapperObserver& RadioEngineWrapperPrivate::observer()
{
    return mObserver;
}

/*!
 *
 */
void RadioEngineWrapperPrivate::startSeeking( Seeking::Direction direction )
{
    mEngineHandler->Seek( direction );
    mObserver.seekingStarted( direction );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::PowerEventL( TBool aPowerState, TInt DEBUGVAR( aError ) )
{
    LOG_FORMAT( "RadioEngineWrapperPrivate::PowerEventL, PowerState: %d, Error: %d", aPowerState, aError );
    mObserver.radioStatusChanged( aPowerState );
    mEngineHandler->PubSub().PublishPowerState( aPowerState );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::FrequencyEventL( TUint32 aFrequency,
                                                 RadioEngine::TRadioFrequencyEventReason aReason,
                                                 TInt aError )
{
    LOG_FORMAT( "RadioEngineWrapperPrivate::FrequencyEventL - Frequency: %d, Reason: %d, Error: %d", aFrequency, aReason, aError );

    if ( mFrequencyScanningHandler )
    {
        // frequencyevents not handled during scanning //TODO remove
        return;
    }

    if ( !aError ) {
        const uint frequency = static_cast<uint>( aFrequency );

        mStationHandler.setCurrentStation( frequency );
        // Stations found by seeking (autotune) are saved as local stations
//        if ( aReason == RadioEngine::ERadioFrequencyEventReasonUp
//             || aReason == RadioEngine::ERadioFrequencyEventReasonDown  )
//        {
//            mStationHandler.addScannedFrequency( frequency );
//            mIsSeeking = false;
//            mCommandSender = 0;
//        }

        //mEngineHandler->SetMuted( EFalse );
        LOG_TIMESTAMP( "Channel change finished" );

        mObserver.tunedToFrequency( frequency, mCommandSender );

        mStationHandler.startDynamicPsCheck();

        mEngineHandler->PubSub().PublishFrequency( aFrequency );
    }
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::VolumeEventL( TInt aVolume, TInt aError )
{
    Q_UNUSED( aError );
    mObserver.volumeChanged( aVolume );
    mEngineHandler->PubSub().PublishVolume( aVolume );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::MuteEventL( TBool aMuteState, TInt aError )
{
    Q_UNUSED( aError );
    mObserver.muteChanged( aMuteState );
    mEngineHandler->PubSub().PublishRadioMuteState( aMuteState );
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
    mObserver.headsetStatusChanged( aAntennaAttached );
//    doc->PubSubL().PublishHeadsetStatusL( EVRPSHeadsetConnected );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::AudioRoutingEventL( TInt aAudioDestination, TInt aError )
{
    //TODO: Check how this event differs from AudioRoutingChangedL
    Q_UNUSED( aAudioDestination )
    Q_UNUSED( aError )
//    doc->PubSubL().PublishLoudspeakerStatusL( EVRPSLoudspeakerNotInUse );
//    Q_Q( RadioEngineWrapper );
//    q->audioRouteChanged( aAudioDestination == RadioEngine::ERadioSpeaker );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::SeekingEventL( TInt aSeekingState, TInt DEBUGVAR( aError ) )
{
    LOG_FORMAT( "RadioEngineWrapperPrivate::SeekingEventL, aSeekingState: %d, Error: %d", aSeekingState, aError );
    if ( aSeekingState != RadioEngine::ERadioNotSeeking ) {
        // We only set the flag here. It is reset in the FrequencyEventL
        mIsSeeking = true;
    }
//    Document()->PubSubL().PublishTuningStateL( EVRPSTuningStarted );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::RegionEventL( TInt DEBUGVAR( aRegion ), TInt DEBUGVAR( aError ) )
{
    LOG_FORMAT( "RadioEngineWrapperPrivate::RegionEventL, aRegion: %d, Error: %d", aRegion, aError );
//    Document()->PubSubL().PublishFrequencyDecimalCountL(
//        static_cast<TVRPSFrequencyDecimalCount>( Document()->RadioSettings()->DecimalCount() ) );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::AudioRouteChangedL( RadioEngine::TRadioAudioRoute aRoute )
{
    mUseLoudspeaker = aRoute == RadioEngine::ERadioSpeaker;
    mObserver.audioRouteChanged( mUseLoudspeaker );
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

/*!
 *
 */
void RadioEngineWrapperPrivate::frequencyScannerFinished()
{
    RadioFrequencyScanningHandler* handler = mFrequencyScanningHandler.take(); // Nulls the pointer
    handler->deleteLater();
    mObserver.scanAndSaveFinished();
}
