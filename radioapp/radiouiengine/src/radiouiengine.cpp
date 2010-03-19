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
#include <qscopedpointer>
#include <qprocess>
#include <qfile>

#ifdef BUILD_WIN32
#   include <qsettings>
#endif // WIN32_BUILD

// User includes
#include "radiouiengine.h"
#include "radiouiengine_p.h"
#include "radiologger.h"
#include "radioenginewrapper.h"
#include "radiostationmodel.h"
#include "radioplaylogmodel.h"
#include "radiosettings.h"
#include "radiostationfiltermodel.h"
#include "radiolocalization.h"

#ifdef USE_MOBILE_EXTENSIONS_API
    #include "xqprofile"
#endif

// Constants
const QString KPathFormatter = "%1:%2%3";
const QString KApplicationDir = "\\sys\\bin\\";
const QString KSongRecognitionApp = "Shazam_0x200265B3.exe";
const QString KSongRecognitionAppParams = "-listen";

/*!
 *
 */
bool RadioUiEngine::isOfflineProfile()
{
    bool offline = false;

#ifdef USE_MOBILE_EXTENSIONS_API
    QScopedPointer<XQProfile> profile ( new XQProfile() );  // Deletes automatically when out of scope
    offline = profile->activeProfile() == XQProfile::ProfileOffLine;
#elif BUILD_WIN32
    QScopedPointer<QSettings> settings( new QSettings( "Nokia", "QtFmRadio" ) );
    offline = settings->value( "Offline", false ).toBool();
#endif

    return offline;
}

/*!
 *
 */
QString RadioUiEngine::parseFrequency( uint frequency )
{
    return QTRANSLATE( KFrequencyMhz ).arg( RadioStation::parseFrequency( frequency ) );
}

/*!
 *
 */
QString RadioUiEngine::nameOrFrequency( const RadioStation& station, uint frequency )
{
    if ( frequency == 0 )
    {
        frequency = station.frequency();
    }

    QString text = "";
    if ( station.isValid() && !station.name().isEmpty() )
    {
        text = station.name();
    }
    else
    {
        text = parseFrequency( frequency );
    }

    return text;
}

/*!
 *
 */
RadioUiEngine::RadioUiEngine( QObject* parent ) :
    QObject( parent ),
    d_ptr( new RadioUiEnginePrivate( this ) )
{
}

/*!
 *
 */
RadioUiEngine::~RadioUiEngine()
{
}

/*!
 *
 */
bool RadioUiEngine::startRadio()
{
    Q_D( RadioUiEngine );
    return d->startRadio();
}

/*!
 *
 */
bool RadioUiEngine::isFirstTimeStart()
{
    Q_D( RadioUiEngine );
    return d->mEngineWrapper->settings().isFirstTimeStart();
}

/*!
 * Returns the settings handler owned by the engine
 */
RadioSettings& RadioUiEngine::settings()
{
    Q_D( RadioUiEngine );
    return d->mEngineWrapper->settings();
}

/*!
 * Returns the station model
 */
RadioStationModel& RadioUiEngine::model()
{
    Q_D( RadioUiEngine );
    return *d->mStationModel;
}

/*!
 * Returns the play log model
 */
RadioPlayLogModel& RadioUiEngine::playLogModel()
{
    Q_D( RadioUiEngine );
    if ( !d->mPlayLogModel ) {
        d->mPlayLogModel = new RadioPlayLogModel( *this );
    }

    return *d->mPlayLogModel;
}

/*!
 * Returns the stations list
 */
RadioStationFilterModel* RadioUiEngine::createNewFilterModel( QObject* parent )
{
    return new RadioStationFilterModel( *this, parent );
}

/*!
 *
 */
bool RadioUiEngine::isRadioOn() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->isRadioOn();
}

/*!
 *
 */
bool RadioUiEngine::isScanning() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->isScanning();
}

/*!
 *
 */
bool RadioUiEngine::isMuted() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->isMuted();
}

/*!
 *
 */
bool RadioUiEngine::isAntennaAttached() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->isAntennaAttached();
}

/*!
 *
 */
bool RadioUiEngine::isUsingLoudspeaker() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->isUsingLoudspeaker();
}

/*!
 * Returns the selected radio region
 */
RadioRegion::Region RadioUiEngine::region() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->region();
}

/*!
 * Returns the currently tuned frequency
 */
uint RadioUiEngine::currentFrequency() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->currentFrequency();
}

/*!
 * Returns the minimum frequency
 */
uint RadioUiEngine::minFrequency() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->minFrequency();
}

/*!
 * Returns the maximum frequency
 */
uint RadioUiEngine::maxFrequency() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->maxFrequency();
}

/*!
 * Returns the frequency step size from the selected region
 */
uint RadioUiEngine::frequencyStepSize() const
{
    Q_D( const RadioUiEngine );
    return d->mEngineWrapper->frequencyStepSize();
}

/*!
 *
 */
void RadioUiEngine::scanFrequencyBand()
{
    Q_D( RadioUiEngine );
    d->mEngineWrapper->scanFrequencyBand();
}

/*!
 *
 */
void RadioUiEngine::cancelScanFrequencyBand()
{
    Q_D( RadioUiEngine );
    d->mEngineWrapper->cancelScanFrequencyBand();
}

/*!
 *
 */
QList<RadioStation> RadioUiEngine::stationsInRange( uint minFrequency, uint maxFrequency )
{
    Q_D( RadioUiEngine );
    return d->mStationModel->stationsInRange( minFrequency, maxFrequency );
}

/*!
 *
 */
QString RadioUiEngine::genreToString( int genre )
{
    RadioRegion::Region currentRegion = region();

    const GenreStruct* genreArray = currentRegion == RadioRegion::America ? AmericanGenres : EuropeanGenres;
    const int genreCount = currentRegion == RadioRegion::America ? AmericanGenresCount : EuropeanGenresCount;

    for( int i = 0; i < genreCount; ++i ) {
        if ( genreArray[i].mGenreCode == genre ) {
            return TRANSLATE( genreArray[i].mTranslation );
//            return qApp->translate( GenreContext,
//                    genreArray[i].mTranslation.source,
//                    genreArray[i].mTranslation.comment );
        }
    }

    return "";
}

/*!
 *
 */
bool RadioUiEngine::isSongRecognitionAppAvailable()
{
    //TODO: Check if there is a better way to check if an application is available
    bool available = false;

    // Check the Z: drive
    QString fullPath = QString( KPathFormatter ).arg( "Z" ).arg( KApplicationDir ).arg( KSongRecognitionApp );
    available = QFile::exists( fullPath );

    LOG_FORMAT( "Checking file: %s. found %d", GETSTRING( fullPath ), available );

    if ( !available ) {
        // Check the C: drive
        fullPath = QString( KPathFormatter ).arg( "C" ).arg( KApplicationDir ).arg( KSongRecognitionApp );
        available = QFile::exists( fullPath );
        LOG_FORMAT( "Checking file: %s. found %d", GETSTRING( fullPath ), available );
        if ( !available ) {
            // Check the E: drive
            fullPath = QString( KPathFormatter ).arg( "E" ).arg( KApplicationDir ).arg( KSongRecognitionApp );
            available = QFile::exists( fullPath );
            LOG_FORMAT( "Checking file: %s. found %d", GETSTRING( fullPath ), available );
        }
    }
    return available;
}

/*!
 *
 */
void RadioUiEngine::addRecognizedSong( const QString& artist, const QString& title )
{
    Q_D( RadioUiEngine );
    d->mPlayLogModel->addItem( artist, title );
}

/*!
 * Public slot
 * Tunes to the given frequency
 */
void RadioUiEngine::tuneFrequency( uint frequency, const int sender )
{
    Q_D( RadioUiEngine );
    if ( frequency != d->mStationModel->currentStation().frequency() && d->mEngineWrapper->isFrequencyValid( frequency ) ) {
        LOG_FORMAT( "RadioUiEngine::tuneFrequency, frequency: %d", frequency );
        d->mEngineWrapper->tuneFrequency( frequency, sender );
    }
}

/*!
 * Public slot
 * Tunes to the given frequency after a delay
 */
void RadioUiEngine::tuneWithDelay( uint frequency, const int sender )
{
    Q_D( RadioUiEngine );
    if ( frequency != d->mStationModel->currentStation().frequency() &&  d->mEngineWrapper->isFrequencyValid( frequency ) ) {
        LOG_FORMAT( "RadioEngineWrapperPrivate::tuneWithDelay, frequency: %d", frequency );
        d->mEngineWrapper->tuneWithDelay( frequency, sender );
    }
}

/*!
 * Public slot
 * Tunes to the given preset
 */
void RadioUiEngine::tunePreset( int presetIndex )
{
    Q_D( RadioUiEngine );
    if ( presetIndex != d->mStationModel->currentStation().presetIndex() ) {
        RadioStation station;
        if ( d->mStationModel->findPresetIndex( presetIndex, station ) != RadioStation::NotFound &&
                d->mEngineWrapper->isFrequencyValid( station.frequency() ) ) {
            LOG_FORMAT( "RadioEngineWrapperPrivate::tunePreset, presetIndexPosition: %d", presetIndex );

            d->mEngineWrapper->tuneFrequency( station.frequency(), CommandSender::Unspecified );
        }
    }
}

/*!
 * Public slot
 * volume update command slot for the engine
 */
void RadioUiEngine::setVolume( int volume )
{
    Q_D( RadioUiEngine );
    d->mEngineWrapper->setVolume( volume );
}

/*!
 * Public slot
 *
 */
void RadioUiEngine::toggleMute()
{
    Q_D( RadioUiEngine );
    d->mEngineWrapper->toggleMute();
}

/*!
 * Public slot
 *
 */
void RadioUiEngine::toggleAudioRoute()
{
    Q_D( RadioUiEngine );
    d->mEngineWrapper->toggleAudioRoute();
}

/*!
 * Public slot
 *
 */
void RadioUiEngine::skipPrevious()
{
    Q_D( RadioUiEngine );    
    d->skip( RadioUiEnginePrivate::Previous );
}

/*!
 * Public slot
 *
 */
void RadioUiEngine::skipNext()
{
    Q_D( RadioUiEngine );
    d->skip( RadioUiEnginePrivate::Next );
}

/*!
 * Public slot
 *
 */
void RadioUiEngine::seekUp()
{
    Q_D( RadioUiEngine );
    d->mEngineWrapper->startSeeking( Seeking::Up );
}

/*!
 * Public slot
 *
 */
void RadioUiEngine::seekDown()
{
    Q_D( RadioUiEngine );
    d->mEngineWrapper->startSeeking( Seeking::Down );
}

/*!
 * Public slot
 *
 */
void RadioUiEngine::launchSongRecognition()
{
    LOG_FORMAT("RadioUiEngine::launchSongRecognition() starting:  %s", GETSTRING( KSongRecognitionApp ) );

    QStringList arguments;
    arguments << KSongRecognitionAppParams;

    bool started = QProcess::startDetached( KSongRecognitionApp, arguments );
    LOG_ASSERT( started, LOG_FORMAT("RadioUiEngine::launchSongRecognition() failed to start %s", GETSTRING( KSongRecognitionApp ) ) );
}

/*!
 * Function used by the private implementation to emit a tunedToFrequency signal
 */
void RadioUiEngine::emitTunedToFrequency( uint frequency, int commandSender )
{
    emit tunedToFrequency( frequency, commandSender );
}

/*!
 * Function used by the private implementation to emit a seekingStarted signal
 */
void RadioUiEngine::emitSeekingStarted( Seeking::Direction direction )
{
    emit seekingStarted( direction );
}

/*!
 * Function used by the private implementation to emit a radioStatusChanged signal
 */
void RadioUiEngine::emitRadioStatusChanged( bool radioIsOn )
{
    emit radioStatusChanged( radioIsOn );
}

/*!
 * Function used by the private implementation to emit a rdsAvailabilityChanged signal
 */
void RadioUiEngine::emitRdsAvailabilityChanged( bool available )
{
    emit rdsAvailabilityChanged( available );
}

/*!
 * Function used by the private implementation to emit a volumeChanged signal
 */
void RadioUiEngine::emitVolumeChanged( int volume )
{
    emit volumeChanged( volume );
}

/*!
 * Function used by the private implementation to emit a muteChanged signal
 */
void RadioUiEngine::emitMuteChanged( bool muted )
{
    emit muteChanged( muted );
}

/*!
 * Function used by the private implementation to emit a audioRouteChanged signal
 */
void RadioUiEngine::emitAudioRouteChanged( bool loudspeaker )
{
    emit audioRouteChanged( loudspeaker );
}

/*!
 * Function used by the private implementation to emit a scanAndSaveFinished signal
 */
void RadioUiEngine::emitScanAndSaveFinished()
{
    emit scanAndSaveFinished();
}
/*!
 * Function used by the private implementation to emit a headsetStatusChanged signal
 */
void RadioUiEngine::emitheadsetStatusChanged( bool connected )
{
    emit headsetStatusChanged( connected );
}

