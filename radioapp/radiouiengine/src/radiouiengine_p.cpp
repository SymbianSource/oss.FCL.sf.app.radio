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
#include <qapplication>
#include <qstringlist>

// User includes
#include "radiouiengine.h"
#include "radiouiengine_p.h"
#include "radioenginewrapper.h"
#include "radiostationmodel.h"
#include "radiopresetstorage.h"
#include "radiosettings.h"
#include "radiostation.h"
#include "radiologger.h"

/*!
 *
 */
RadioUiEnginePrivate::RadioUiEnginePrivate( RadioUiEngine* engine ) :
    q_ptr( engine ),
    mEngineWrapper( 0 ),
    mStationModel( 0 ),
    mPlayLogModel( 0 )
{
}

/*!
 *
 */
RadioUiEnginePrivate::~RadioUiEnginePrivate()
{
}

/*!
 *
 */
bool RadioUiEnginePrivate::startRadio()
{
    mStationModel = new RadioStationModel( *q_ptr );
    mEngineWrapper.reset( new RadioEngineWrapper( mStationModel->stationHandlerIf(), *this ) );
    mPresetStorage.reset( new RadioPresetStorage() );
    mStationModel->initialize( mPresetStorage.data(), mEngineWrapper.data() );

    return mEngineWrapper->isEngineConstructed();
}

/*!
 *
 */
void RadioUiEnginePrivate::tunedToFrequency( uint frequency, int commandSender )
{
    Q_Q( RadioUiEngine );
    q->emitTunedToFrequency( frequency, commandSender );
}

/*!
 *
 */
void RadioUiEnginePrivate::seekingStarted( Seeking::Direction direction )
{
    Q_Q( RadioUiEngine );
    q->emitSeekingStarted( direction );
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
void RadioUiEnginePrivate::scanAndSaveFinished()
{
    Q_Q( RadioUiEngine );
    q->emitScanAndSaveFinished();
}

/*!
 *
 */
void RadioUiEnginePrivate::headsetStatusChanged( bool connected )
{
    Q_Q( RadioUiEngine );
    q->emitheadsetStatusChanged( connected );
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
        mEngineWrapper->tuneFrequency( previous, CommandSender::Unspecified );
    } else {
        if ( next == 0 ) {
            next = favorites.first();
        }
        LOG_FORMAT( "RadioUiEnginePrivate::skip. CurrentFreq: %u, tuning to: %u", currentFreq, next );
        mEngineWrapper->tuneFrequency( next, CommandSender::Unspecified );
    }    
}

