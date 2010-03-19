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
#include <qtimer>
#include <qsettings>

// User includes
#include "radioenginewrapper_win32_p.h"
#include "radiosettings.h"
#include "radiosettings_p.h"
#include "radiologger.h"
#include "radio_global.h"
#include "radiostationhandlerif.h"
#include "radioenginewrapperobserver.h"
//#include "t_radiodataparser.h"

static RadioEngineWrapperPrivate* theInstance = 0;

const QString KKeyFrequency = "CurrentFreq";
const QString KKeyOffline = "Offline";

/*!
 *
 */
RadioEngineWrapperPrivate::RadioEngineWrapperPrivate( RadioEngineWrapper* wrapper,
                                                      RadioStationHandlerIf& stationHandler,
                                                      RadioEngineWrapperObserver& observer ) :
    q_ptr( wrapper ),
    mStationHandler( stationHandler ),
    mObserver( observer ),
    mTuneTimer( new QTimer( this ) ),
    mCommandSender( 0 ),
    mUseLoudspeaker( false ),
    mIsSeeking( false ),
    mAntennaAttached( true ),
    mFrequency( 0 ),
    mVolume( 5 ),
    mMaxVolume( 10000 ),
    mFrequencyStepSize( 50000 ),
    mRegionId( RadioRegion::Default ),
    mMinFrequency( 87500000 ),
    mMaxFrequency( 108000000 )
{
    ::theInstance = this;
    mEngineSettings.reset( new QSettings( "Nokia", "QtFmRadio" ) );
    mFrequency = mEngineSettings->value( KKeyFrequency ).toUInt();
    if ( mFrequency == 0 ) {
        mFrequency = mMinFrequency;
    }

    connectAndTest( mTuneTimer, SIGNAL(timeout()), this, SLOT(frequencyEvent()) );
    mTuneTimer->setSingleShot( true );    
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
 *
 */
RadioEngineWrapperPrivate* RadioEngineWrapperPrivate::instance()
{
    return ::theInstance;
}

/*!
 * Initializes the private implementation
 */
void RadioEngineWrapperPrivate::init()
{
    mUseLoudspeaker = false;
    if ( !mUseLoudspeaker ) {
        mObserver.audioRouteChanged( false );
    }

    parseData();
}

/*!
 * Starts up the radio engine
 */
bool RadioEngineWrapperPrivate::isEngineConstructed()
{
    return true;
}

/*!
 * Returns the settings handler owned by the engine
 */
RadioSettings& RadioEngineWrapperPrivate::settings()
{
    if ( !mSettings ) {
        mSettings.reset( new RadioSettings() );
//        mSettings->d_func()->init( &mEngineHandler->ApplicationSettings() );
    }
    return *mSettings;
}

/*!
 * Tunes to the given frequency
 */
void RadioEngineWrapperPrivate::tuneFrequency( uint frequency, const int sender )
{
    mFrequency = frequency;
    mEngineSettings->setValue( KKeyFrequency, mFrequency );
    mCommandSender = sender;
    mTuneTimer->stop();
    mTuneTimer->start( 500 );
}

/*!
 * Tunes to the given frequency after a delay
 */
void RadioEngineWrapperPrivate::tuneWithDelay( uint frequency, const int sender )
{
    mFrequency = frequency;
    mEngineSettings->setValue( KKeyFrequency, mFrequency );
    mCommandSender = sender;
    mTuneTimer->stop();
    mTuneTimer->start( 1500 );
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
    mObserver.seekingStarted( direction );
}

/*!
 *
 */
QString RadioEngineWrapperPrivate::dataParsingError() const
{
    return mParsingError;
}

/*!
 *
 */
void RadioEngineWrapperPrivate::setHeadsetStatus( bool connected )
{
    mAntennaAttached = connected;
    mObserver.headsetStatusChanged( mAntennaAttached );
}

/*!
 *
 */
void RadioEngineWrapperPrivate::setVolume( int volume )
{
    mVolume = volume;
    mObserver.volumeChanged( mVolume );
}

/*!
 *
 */
void RadioEngineWrapperPrivate::addSong( const QString& artist, const QString& title )
{
    QString radioText = QString( "Now Playing: %1 - %2" ).arg( artist ).arg( title );

    const uint frequency = mStationHandler.currentFrequency();
    mStationHandler.setCurrentRadioText( frequency, radioText );
    mStationHandler.setCurrentRadioTextPlus( frequency, RtPlus::Artist, artist );
    mStationHandler.setCurrentRadioTextPlus( frequency, RtPlus::Title, title );
}

/*!
 *
 */
void RadioEngineWrapperPrivate::clearSong()
{
    mStationHandler.setCurrentRadioText( mStationHandler.currentFrequency(), "" );
}

/*!
 *
 */
bool RadioEngineWrapperPrivate::isOffline() const
{
    return mEngineSettings->value( KKeyOffline, false ).toBool();
}

/*!
 *
 */
void RadioEngineWrapperPrivate::setOffline( bool offline )
{
    mEngineSettings->setValue( KKeyOffline, offline );
}

/*!
 * Private slot
 */
void RadioEngineWrapperPrivate::frequencyEvent()
{
    mStationHandler.setCurrentStation( mFrequency );
    mObserver.tunedToFrequency( mFrequency, mCommandSender );
    mStationHandler.startDynamicPsCheck();
}

/*!
 *
 */
void RadioEngineWrapperPrivate::frequencyScannerFinished()
{
    mObserver.scanAndSaveFinished();
}

/*!
 *
 */
void RadioEngineWrapperPrivate::parseData()
{
    /*
    mDataParser.reset( new T_RadioDataParser() );
    bool ok = mDataParser->parse();
    if ( !ok ) {
        mParsingError = mDataParser->errorString();
    } else {
        if ( mDataParser->mEngineSettings.mMaxVolume > 0 ) {
            mMaxVolume = mDataParser->mEngineSettings.mMaxVolume;
        }

        if ( mDataParser->mEngineSettings.mFrequencyStepSize > 0 ) {
            mFrequencyStepSize = mDataParser->mEngineSettings.mFrequencyStepSize;
        }

        if ( mDataParser->mEngineSettings.mRegionId >= 0 ) {
            mRegionId = static_cast<RadioRegion::Region>( mDataParser->mEngineSettings.mRegionId );
        }

        if ( mDataParser->mEngineSettings.mMinFrequency > 0 ) {
            mMinFrequency = mDataParser->mEngineSettings.mMinFrequency;
        }

        if ( mDataParser->mEngineSettings.mMaxFrequency > 0 ) {
            mMaxFrequency = mDataParser->mEngineSettings.mMaxFrequency;
        }
    }
*/
}
