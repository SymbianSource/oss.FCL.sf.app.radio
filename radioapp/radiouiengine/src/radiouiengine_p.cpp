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
#include <QDateTime>
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
#include "radiohistorymodel.h"
#include "radiocarouselmodel.h"
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
    q_ptr( engine )
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
    bool deleted = utils.deleteProperty( radioStartupKey );
    LOG_ASSERT( deleted, LOG( "RadioUiEnginePrivate::~RadioUiEnginePrivate(). Failed to delete P&S key" ) );
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
bool RadioUiEnginePrivate::init()
{
#ifndef BUILD_WIN32
    mControlService.reset( new RadioControlService( *q_ptr ) );
#endif
    mMonitorService.reset( new RadioMonitorService( *this ) );
    mStationModel.reset( new RadioStationModel( *this ) );
    mEngineWrapper.reset( new RadioEngineWrapper( mStationModel->stationHandlerIf() ) );
    mEngineWrapper->addObserver( this );
    mPresetStorage.reset( new RadioPresetStorage() );
    mStationModel->initialize( mPresetStorage.data(), mEngineWrapper.data() );
    mHistoryModel.reset( new RadioHistoryModel( *q_ptr ) );

#ifndef BUILD_WIN32
    // Write the startup timestamp to P&S key for the homescreen widget
    XQSettingsManager settingsManager;
    XQPublishAndSubscribeUtils utils( settingsManager );
    XQPublishAndSubscribeSettingsKey radioStartupKey( KRadioPSUid, KRadioStartupKey );
    bool defined = utils.defineProperty( radioStartupKey, XQSettingsManager::TypeInt );
    if ( defined ) {
        settingsManager.writeItemValue( radioStartupKey, (int)QDateTime::currentDateTime().toTime_t() );
    }
#endif

    mMonitorService->init();

    return mEngineWrapper->isEngineConstructed();
}

/*!
 *
 */
void RadioUiEnginePrivate::cancelSeeking()
{
    mEngineWrapper->cancelSeeking();
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
}

/*!
 *
 */
void RadioUiEnginePrivate::skipPrevious()
{
    skip( StationSkip::PreviousFavorite );
}

/*!
 *
 */
void RadioUiEnginePrivate::skipNext()
{
    skip( StationSkip::NextFavorite );
}

/*!
 * Tunes to next or previous station
 */
uint RadioUiEnginePrivate::skip( StationSkip::Mode mode, uint startFrequency )
{
    LOG_FORMAT( "RadioUiEnginePrivate::skip: mode: %d", mode );
    if ( startFrequency == 0 ) {
        startFrequency = mEngineWrapper->currentFrequency();
    }

    const uint newFrequency = mStationModel->findClosest( startFrequency, mode ).frequency();

    LOG_FORMAT( "RadioUiEnginePrivate::skip. CurrentFreq: %u, tuning to: %u", startFrequency, newFrequency );
    mEngineWrapper->tuneFrequency( newFrequency, TuneReason::Skip );
    return newFrequency;
}

