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
#include <QApplication>
#include <QStringList>
#include <QTime>
#ifndef BUILD_WIN32
#   include <XQSettingsManager>
#   include <XQPublishAndSubscribeSettingsKey>
#   include <XQPublishAndSubscribeUtils>
#endif

// User includes
#include "radiouiengine.h"
#include "radiouiengine_p.h"
#include "radioenginewrapper.h"
#include "radiostationmodel.h"
#include "radiostationmodel_p.h"
#include "radiopresetstorage.h"
#include "radiosettings.h"
#include "radiostation.h"
#include "radioscannerengine.h"
#include "radiostationhandlerif.h"
#ifndef BUILD_WIN32
#   include "radiocontrolservice.h"
#   include "radiomonitorservice.h"
#else
#   include "radiomonitorservice_win32.h"
#endif
#include "radioservicedef.h"
#include "radiologger.h"

/*!
 *
 */
RadioUiEnginePrivate::RadioUiEnginePrivate( RadioUiEngine* engine ) :
    q_ptr( engine ),
    mEngineWrapper( 0 ),
    mStationModel( 0 ),
    mHistoryModel( 0 ),
    mControlService( 0 ),
    mMonitorService( 0 )
{
}

/*!
 *
 */
RadioUiEnginePrivate::~RadioUiEnginePrivate()
{
#ifndef BUILD_WIN32
    XQSettingsManager settingsManager;
    XQPublishAndSubscribeUtils utils( settingsManager );
    XQPublishAndSubscribeSettingsKey radioStartupKey( KRadioPSUid, KRadioStartupKey );
    utils.deleteProperty( radioStartupKey );
#endif
}

/*!
 *
 */
RadioUiEngine& RadioUiEnginePrivate::api()
{
    Q_Q( RadioUiEngine );
    return *q;
}

/*!
 *
 */
bool RadioUiEnginePrivate::startRadio()
{
#ifndef BUILD_WIN32
    mControlService = new RadioControlService( *q_ptr );
#endif
    mMonitorService = new RadioMonitorService( *q_ptr );
    mStationModel = new RadioStationModel( *this );
    mEngineWrapper.reset( new RadioEngineWrapper( mStationModel->stationHandlerIf() ) );
    mEngineWrapper->addObserver( this );
    mPresetStorage.reset( new RadioPresetStorage() );
    mStationModel->initialize( mPresetStorage.data(), mEngineWrapper.data() );

#ifndef BUILD_WIN32
    // Write the startup timestamp to P&S key for the homescreen widget
    XQSettingsManager settingsManager;
    XQPublishAndSubscribeUtils utils( settingsManager );
    XQPublishAndSubscribeSettingsKey radioStartupKey( KRadioPSUid, KRadioStartupKey );
    utils.defineProperty( radioStartupKey, XQSettingsManager::TypeVariant );
    settingsManager.writeItemValue( radioStartupKey, QVariant( QTime::currentTime() ) );   
#endif

    return mEngineWrapper->isEngineConstructed();
}

/*!
 *
 */
void RadioUiEnginePrivate::cancelSeeking()
{
    mEngineWrapper->cancelSeeking();
    mMonitorService->notifyRadioStatus( mEngineWrapper->isMuted() ? RadioStatus::Muted : RadioStatus::Playing );
}

/*!
 *
 */
RadioEngineWrapper& RadioUiEnginePrivate::wrapper()
{
    return *mEngineWrapper;
}

/*!
 *
 */
void RadioUiEnginePrivate::tunedToFrequency( uint frequency, int reason )
{
    Q_Q( RadioUiEngine );
    q->emitTunedToFrequency( frequency, reason );
    mMonitorService->notifyRadioStatus( RadioStatus::Playing );
}

/*!
 *
 */
void RadioUiEnginePrivate::radioStatusChanged( bool radioIsOn )
{
    Q_Q( RadioUiEngine );
    q->emitRadioStatusChanged( radioIsOn );

    if ( radioIsOn ) {
        Q_Q( RadioUiEngine );
        QStringList args; // = qApp->arguments();
        if ( args.count() == 2 )
        {
            if ( args.at( 0 ) == "-f" ) // Frequency
            {
                uint frequency = args.at( 1 ).toUInt();

                if ( frequency >= mEngineWrapper->minFrequency() && frequency <= mEngineWrapper->maxFrequency() )
                {
                    LOG_FORMAT( "RadioApplication::handleArguments, Tuning to frequency: %d", frequency );
                    q->tuneFrequency( frequency, 0 );
                }
            }
            else if ( args.at( 0 ) == "-i" ) // Preset index
            {
                int preset = args.at( 1 ).toInt();
                if ( preset > 0 && preset < mStationModel->rowCount() )
                {
                    LOG_FORMAT( "RadioApplication::handleArguments, Tuning to preset %d", preset );
                    q->tunePreset( preset );
                }
            }
        }
    }
}

/*!
 *
 */
void RadioUiEnginePrivate::rdsAvailabilityChanged( bool available )
{
    Q_Q( RadioUiEngine );
    q->emitRdsAvailabilityChanged( available );
}

/*!
 *
 */
void RadioUiEnginePrivate::volumeChanged( int volume )
{
    Q_Q( RadioUiEngine );
    q->emitVolumeChanged( volume );
}

/*!
 *
 */
void RadioUiEnginePrivate::muteChanged( bool muted )
{
    Q_Q( RadioUiEngine );
    q->emitMuteChanged( muted );
    mMonitorService->notifyRadioStatus( muted ? RadioStatus::Muted : RadioStatus::Playing );
}

/*!
 *
 */
void RadioUiEnginePrivate::audioRouteChanged( bool loudspeaker )
{
    Q_Q( RadioUiEngine );
    q->emitAudioRouteChanged( loudspeaker );
}

/*!
 *
 */
void RadioUiEnginePrivate::antennaStatusChanged( bool connected )
{
    Q_Q( RadioUiEngine );
    q->emitAntennaStatusChanged( connected );
    mMonitorService->notifyAntennaStatus( connected );
}

/*!
 *
 */
void RadioUiEnginePrivate::skipPrevious()
{
    skip( Previous );
}

/*!
 *
 */
void RadioUiEnginePrivate::skipNext()
{
    skip( Next );
}

/*!
 * Tunes to next or previous favorite preset
 */
void RadioUiEnginePrivate::skip( RadioUiEnginePrivate::TuneDirection direction )
{
    LOG_FORMAT( "RadioUiEnginePrivate::skip: direction: %d", direction );

    //TODO: Refactor to go through RadioStationModel
    QList<uint> favorites;
    const uint currentFreq = mStationModel->currentStation().frequency();

    // Find all favorites
    foreach( const RadioStation& station, mStationModel->list() ) {
        if ( station.isFavorite() && station.frequency() != currentFreq ) {
            favorites.append( station.frequency() );
        }
    }

    const int favoriteCount = favorites.count();
    if ( favoriteCount == 0 ) {
        return;
    }

    // Find the previous and next favorite from current frequency
    uint previous = 0;
    uint next = 0;
    foreach( uint favorite, favorites ) {
        if ( favorite > currentFreq ) {
            next = favorite;
            break;
        }
        previous = favorite;
    }

    if ( direction == RadioUiEnginePrivate::Previous ) {
        if ( previous == 0 ) {
            previous = favorites.last();
        }
        LOG_FORMAT( "RadioUiEnginePrivate::skip. CurrentFreq: %u, tuning to: %u", currentFreq, previous );
        mEngineWrapper->tuneFrequency( previous, TuneReason::Unspecified );
    } else {
        if ( next == 0 ) {
            next = favorites.first();
        }
        LOG_FORMAT( "RadioUiEnginePrivate::skip. CurrentFreq: %u, tuning to: %u", currentFreq, next );
        mEngineWrapper->tuneFrequency( next, TuneReason::Unspecified );
    }    
}

