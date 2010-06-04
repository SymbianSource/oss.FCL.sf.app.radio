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
#include <QScopedPointer>
#include <QProcess>
#include <QFile>

#ifdef BUILD_WIN32
#   include <QSettings>
#else
#   include <qsysteminfo.h>
#   include <XQSettingsManager>
using namespace QtMobility;
#endif // WIN32_BUILD

// User includes
#include "radiouiengine.h"
#include "radiouiengine_p.h"
#include "radiologger.h"
#include "radioenginewrapper.h"
#include "radiostationmodel.h"
#include "radiohistorymodel.h"
#include "radiocarouselmodel.h"
#include "radiohistoryitem.h"
#include "radiosettings.h"
#include "radiostationfiltermodel.h"
#include "radioscannerengine.h"

// Constants
const QString KPathFormatter = "%1:%2%3";
const QString KApplicationDir = "\\sys\\bin\\";
const QString KSongRecognitionApp = "Shazam_0x200265B3.exe";
const QString KSongRecognitionAppParams = "-listen";

const uint DEFAULT_MIN_FREQUENCY = 87500000;
const uint RADIO_CENREP_UID = 0x101FF976;
const uint RADIO_CENREP_FREQUENCY_KEY = 0x00000107;

struct GenreStruct
{
    int mGenreCode;
    const char* mInCarousel;
    const char* mInStationsList;
    const char* mInHomeScreen;
};

const GenreStruct EuropeanGenres[] =
{
     { GenreEurope::RdsNone, "", "", "" }
    ,{ GenreEurope::RdsNews, "txt_rad_info_news", "txt_rad_dblist_l1_mhz_val_news", "txt_rad_info_news_hs" }
    ,{ GenreEurope::RdsCurrentAffairs, "txt_rad_info_current_affairs", "txt_rad_dblist_l1_mhz_val_current_affairs", "txt_rad_info_current_affairs_hs" }
    ,{ GenreEurope::RdsInformation, "txt_rad_info_information", "txt_rad_dblist_l1_mhz_val_information", "txt_rad_info_information_hs" }
    ,{ GenreEurope::RdsSport, "txt_rad_info_sport", "txt_rad_dblist_l1_mhz_val_sport", "txt_rad_info_sport_hs" }
    ,{ GenreEurope::RdsEducation, "txt_rad_info_education", "txt_rad_dblist_l1_mhz_val_education", "txt_rad_info_education_hs" }
    ,{ GenreEurope::RdsDrama, "txt_rad_info_drama", "txt_rad_dblist_l1_mhz_val_drama", "txt_rad_info_drama_hs" }
    ,{ GenreEurope::RdsCulture, "txt_rad_info_culture", "txt_rad_dblist_l1_mhz_val_culture", "txt_rad_info_culture_hs" }
    ,{ GenreEurope::RdsScience, "txt_rad_info_science", "txt_rad_dblist_l1_mhz_val_science", "txt_rad_info_science_hs" }
    ,{ GenreEurope::RdsVariedSpeech, "txt_rad_info_varied", "txt_rad_dblist_l1_mhz_val_varied", "txt_rad_info_varied_hs" }
    ,{ GenreEurope::RdsPopMusic, "txt_rad_info_pop_music", "txt_rad_dblist_l1_mhz_val_pop_music", "txt_rad_info_pop_music_hs" }
    ,{ GenreEurope::RdsRockMusic, "txt_rad_info_rock_music", "txt_rad_dblist_l1_mhz_val_rock_music", "txt_rad_info_rock_music_hs" }
    ,{ GenreEurope::RdsEasyListening, "txt_rad_info_easy_listening", "txt_rad_dblist_l1_mhz_val_easy_listening", "txt_rad_info_easy_listening_hs" }
    ,{ GenreEurope::RdsLightClassical, "txt_rad_info_light_classical", "txt_rad_dblist_l1_mhz_val_light_classical", "txt_rad_info_light_classical_hs" }
    ,{ GenreEurope::RdsSeriousClassical, "txt_rad_info_serious_classical", "txt_rad_dblist_l1_mhz_val_serious_classical", "txt_rad_info_serious_classical_hs" }
    ,{ GenreEurope::RdsOtherMusic, "txt_rad_info_other_music", "txt_rad_dblist_l1_mhz_val_other_music", "txt_rad_info_other_music_hs" }
    ,{ GenreEurope::RdsWeather, "txt_rad_info_weather", "txt_rad_dblist_l1_mhz_val_weather", "txt_rad_info_weather_hs" }
    ,{ GenreEurope::RdsFinance, "txt_rad_info_finance", "txt_rad_dblist_l1_mhz_val_finance", "txt_rad_info_finance_hs" }
    ,{ GenreEurope::RdsChildrensProgrammes, "txt_rad_info_childrens_programmes", "txt_rad_dblist_l1_mhz_val_childrens_programmes", "txt_rad_info_childrens_programmes_hs" }
    ,{ GenreEurope::RdsSocialAffairs, "txt_rad_info_social_affairs", "txt_rad_dblist_l1_mhz_val_social_affairs", "txt_rad_info_social_affairs_hs" }
    ,{ GenreEurope::RdsReligion, "txt_rad_info_religion", "txt_rad_dblist_l1_mhz_val_religion", "txt_rad_info_religion_hs" }
    ,{ GenreEurope::RdsPhoneIn, "txt_rad_info_phone_in", "txt_rad_dblist_l1_mhz_val_phone_in", "txt_rad_info_phone_in_hs" }
    ,{ GenreEurope::RdsTravel, "txt_rad_info_travel", "txt_rad_dblist_l1_mhz_val_travel", "txt_rad_info_travel_hs" }
    ,{ GenreEurope::RdsLeisure, "txt_rad_info_leisure", "txt_rad_dblist_l1_mhz_val_leisure", "txt_rad_info_leisure_hs" }
    ,{ GenreEurope::RdsJazzMusic, "txt_rad_info_jazz_music", "txt_rad_dblist_l1_mhz_val_jazz_music", "txt_rad_info_jazz_music_hs" }
    ,{ GenreEurope::RdsCountryMusic, "txt_rad_info_country_music", "txt_rad_dblist_l1_mhz_val_country_music", "txt_rad_info_country_music_hs" }
    ,{ GenreEurope::RdsNationalMusic, "txt_rad_info_national_music", "txt_rad_dblist_l1_mhz_val_national_music", "txt_rad_info_national_music_hs" }
    ,{ GenreEurope::RdsOldiesMusic, "txt_rad_info_oldies_music", "txt_rad_dblist_l1_mhz_val_oldies_music", "txt_rad_info_oldies_music_hs" }
    ,{ GenreEurope::RdsFolkMusic, "txt_rad_info_folk_music", "txt_rad_dblist_l1_mhz_val_folk_music", "txt_rad_info_folk_music_hs" }
    ,{ GenreEurope::RdsDocumentary, "txt_rad_info_documentary", "txt_rad_dblist_l1_mhz_val_documentary", "txt_rad_info_documentary_hs" }
    ,{ GenreEurope::RdsAlarmTest, "txt_rad_info_alarm_test", "txt_rad_dblist_l1_mhz_val_alarm_test", "txt_rad_info_alarm_test_hs" }
    ,{ GenreEurope::RdsAlarm, "txt_rad_info_alarm", "txt_rad_dblist_l1_mhz_val_alarm", "txt_rad_info_alarm_hs" }
};
const int EuropeanGenresCount = sizeof( EuropeanGenres ) / sizeof ( EuropeanGenres[0] );

const GenreStruct AmericanGenres[] =
{
     { GenreAmerica::RbdsNone, "", "", "" }
    ,{ GenreAmerica::RbdsNews, "txt_rad_info_news", "txt_rad_dblist_l1_mhz_val_news", "txt_rad_info_news_hs" }
    ,{ GenreAmerica::RbdsInformation, "txt_rad_info_information", "txt_rad_dblist_l1_mhz_val_information", "txt_rad_info_information_hs" }
    ,{ GenreAmerica::RbdsSports, "txt_rad_info_sport", "txt_rad_dblist_l1_mhz_val_sport", "txt_rad_info_sport_hs" }
    ,{ GenreAmerica::RbdsTalk, "txt_rad_info_talk", "txt_rad_dblist_l1_mhz_val_talk", "txt_rad_info_talk_hs" }
    ,{ GenreAmerica::RbdsRock, "txt_rad_info_rock_music", "txt_rad_dblist_l1_mhz_val_rock_music", "txt_rad_info_rock_music_hs" } //TODO: Check
    ,{ GenreAmerica::RbdsClassicRock, "txt_rad_info_classic_rock", "txt_rad_dblist_l1_mhz_val_classic_rock", "txt_rad_info_classic_rock_hs" }
    ,{ GenreAmerica::RbdsAdultHits, "txt_rad_info_adult_hits", "txt_rad_dblist_l1_mhz_val_adult_hits", "txt_rad_info_adult_hits_hs" }
    ,{ GenreAmerica::RbdsSoftRock, "txt_rad_info_soft_rock", "txt_rad_dblist_l1_mhz_val_soft_rock", "txt_rad_info_soft_rock_hs" }
    ,{ GenreAmerica::RbdsTop40, "txt_rad_info_top_40", "txt_rad_dblist_l1_mhz_val_top_40", "txt_rad_info_top_40_hs" }
    ,{ GenreAmerica::RbdsCountry, "txt_rad_info_country_music", "txt_rad_dblist_l1_mhz_val_country_music", "txt_rad_info_country_music_hs" }  //TODO: Check
    ,{ GenreAmerica::RbdsOldies, "txt_rad_info_oldies_music", "txt_rad_dblist_l1_mhz_val_oldies_music", "txt_rad_info_oldies_music_hs" }  //TODO: Check
    ,{ GenreAmerica::RbdsSoft, "txt_rad_info_soft", "txt_rad_dblist_l1_mhz_val_soft", "txt_rad_info_soft_hs" }
    ,{ GenreAmerica::RbdsNostalgia, "txt_rad_info_nostalgia", "txt_rad_dblist_l1_mhz_val_nostalgia", "txt_rad_info_nostalgia_hs" }
    ,{ GenreAmerica::RbdsJazz, "txt_rad_info_jazz_music", "txt_rad_dblist_l1_mhz_val_jazz_music", "txt_rad_info_jazz_music_hs" } //TODO: Check
    ,{ GenreAmerica::RbdsClassical, "txt_rad_info_classical", "txt_rad_dblist_l1_mhz_val_classical", "txt_rad_info_classical_hs" }
    ,{ GenreAmerica::RbdsRhythmAndBlues, "txt_rad_info_rhythm_and_blues", "txt_rad_dblist_l1_mhz_val_rhythm_and_blues", "txt_rad_info_rhythm_and_blues_hs" }
    ,{ GenreAmerica::RbdsSoftRhythmAndBlues, "txt_rad_info_soft_rhythm_and_blues", "txt_rad_dblist_l1_mhz_val_soft_rhythm_and_blues", "txt_rad_info_soft_rhythm_and_blues_hs" }
    ,{ GenreAmerica::RbdsLanguage, "txt_rad_info_language", "txt_rad_dblist_l1_mhz_val_language", "txt_rad_info_language_hs" }
    ,{ GenreAmerica::RbdsReligiousMusic, "txt_rad_info_religious_music", "txt_rad_dblist_l1_mhz_val_religious_music", "txt_rad_info_religious_music_hs" }
    ,{ GenreAmerica::RbdsReligiousTalk, "txt_rad_info_religious_talk", "txt_rad_dblist_l1_mhz_val_religious_talk", "txt_rad_info_religious_talk_hs" }
    ,{ GenreAmerica::RbdsPersonality, "txt_rad_info_personality", "txt_rad_dblist_l1_mhz_val_personality", "txt_rad_info_personality_hs" }
    ,{ GenreAmerica::RbdsPublic, "txt_rad_info_public", "txt_rad_dblist_l1_mhz_val_public", "txt_rad_info_public_hs" }
    ,{ GenreAmerica::RbdsCollege, "txt_rad_info_college", "txt_rad_dblist_l1_mhz_val_college", "txt_rad_info_college_hs" }
    ,{ GenreAmerica::RbdsUnassigned1, "", "", "" }
    ,{ GenreAmerica::RbdsUnassigned2, "", "", "" }
    ,{ GenreAmerica::RbdsUnassigned3, "", "", "" }
    ,{ GenreAmerica::RbdsUnassigned4, "", "", "" }
    ,{ GenreAmerica::RbdsUnassigned5, "", "", "" }
    ,{ GenreAmerica::RbdsWeather, "txt_rad_info_weather", "txt_rad_dblist_l1_mhz_val_weather", "txt_rad_info_weather_hs" }//TODO: Check
    ,{ GenreAmerica::RbdsEmergencyTest, "txt_rad_info_alarm_test", "txt_rad_dblist_l1_mhz_val_alarm_test", "txt_rad_info_alarm_test_hs" }//TODO: Check
    ,{ GenreAmerica::RbdsEmergency, "txt_rad_info_alarm", "txt_rad_dblist_l1_mhz_val_alarm", "txt_rad_info_alarm_hs" }//TODO: Check
};
const int AmericanGenresCount = sizeof( AmericanGenres ) / sizeof ( AmericanGenres[0] );

/*!
 *
 */
bool RadioUiEngine::isOfflineProfile()
{
    bool offline = false;

#ifdef BUILD_WIN32
    QScopedPointer<QSettings> settings( new QSettings( "Nokia", "QtFmRadio" ) );
    offline = settings->value( "Offline", false ).toBool();
#else
    QSystemDeviceInfo deviceInfo;
    if ( deviceInfo.currentProfile() == QSystemDeviceInfo::OfflineProfile ) {
        offline = true;
    }
#endif

    return offline;
}

/*!
 *
 */
uint RadioUiEngine::lastTunedFrequency()
{
    uint frequency = DEFAULT_MIN_FREQUENCY;

#ifdef BUILD_WIN32
    QScopedPointer<QSettings> settings( new QSettings( "Nokia", "QtFmRadio" ) );
    frequency = settings->value( "CurrentFreq", DEFAULT_MIN_FREQUENCY ).toUInt();
    if ( frequency == 0 ) {
        frequency = DEFAULT_MIN_FREQUENCY;
    }
#else
    QScopedPointer<XQSettingsManager> settings( new XQSettingsManager() );
    XQSettingsKey key( XQSettingsKey::TargetCentralRepository, RADIO_CENREP_UID, RADIO_CENREP_FREQUENCY_KEY );
    frequency = settings->readItemValue( key, XQSettingsManager::TypeInt ).toUInt();
#endif

    return frequency;
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
    delete d_ptr;
}

/*!
 *
 */
bool RadioUiEngine::isInitialized() const
{
    Q_D( const RadioUiEngine );
    return !d->mEngineWrapper.isNull();
}

/*!
 *
 */
bool RadioUiEngine::init()
{
    Q_D( RadioUiEngine );
    return d->init();
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
RadioSettingsIf& RadioUiEngine::settings()
{
    Q_D( RadioUiEngine );
    return d->mEngineWrapper->settings();
}

/*!
 * Returns the station model
 */
RadioStationModel& RadioUiEngine::stationModel()
{
    Q_D( RadioUiEngine );
    return *d->mStationModel;
}

/*!
 * Returns the history model
 */
RadioHistoryModel& RadioUiEngine::historyModel()
{
    Q_D( RadioUiEngine );
    return *d->mHistoryModel;
}

/*!
 * Creates a new filter model
 */
RadioStationFilterModel* RadioUiEngine::createNewFilterModel( QObject* parent )
{
    return new RadioStationFilterModel( *this, parent );
}

/*!
 * Creates a new carousel model
 */
RadioCarouselModel* RadioUiEngine::carouselModel()
{
    Q_D( RadioUiEngine );
    if ( !d->mCarouselModel ) {
        d->mCarouselModel.reset( new RadioCarouselModel( *this, *d->mStationModel ) );
    }

    return d->mCarouselModel.data();
}

/*!
 *
 */
RadioScannerEngine* RadioUiEngine::scannerEngine()
{
    Q_D( RadioUiEngine );
    if ( !d->mScannerEngine ) {
        d->mScannerEngine = new RadioScannerEngine( *d );
    }
    return d->mScannerEngine;
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
    if ( d->mScannerEngine ) {
        return d->mScannerEngine->isScanning();
    }
    return false;
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
 * Sets the mute status
 */
void RadioUiEngine::setMute( bool muted )
{
    Q_D( RadioUiEngine );
    d->mEngineWrapper->setMute( muted );
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
QString RadioUiEngine::genreToString( int genre, GenreTarget::Target target )
{
    RadioRegion::Region currentRegion = region();

    const GenreStruct* genreArray = currentRegion == RadioRegion::America ? AmericanGenres : EuropeanGenres;
    const int genreCount = currentRegion == RadioRegion::America ? AmericanGenresCount : EuropeanGenresCount;

    for( int i = 0; i < genreCount; ++i ) {
        if ( genreArray[i].mGenreCode == genre ) {
            if ( target == GenreTarget::Carousel ) {
                return qtTrId( genreArray[i].mInCarousel );
            } else if ( target == GenreTarget::StationsList ) {
                return qtTrId( genreArray[i].mInStationsList );
            } else if ( target == GenreTarget::HomeScreen ) {
                return qtTrId( genreArray[i].mInHomeScreen );
            }
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
void RadioUiEngine::addRecognizedSong( const QString& artist, const QString& title, const RadioStation& station )
{
    Q_D( RadioUiEngine );
    d->mHistoryModel->addItem( artist, title, station );
}

/*!
 *
 */
uint RadioUiEngine::skipStation( StationSkip::Mode mode, uint startFrequency )
{
    Q_D( RadioUiEngine );
    return d->skip( mode, startFrequency );
}

/*!
 *
 */
void RadioUiEngine::openMusicStore( const RadioHistoryItem& item, MusicStore store )
{
    Q_UNUSED( item );
    Q_UNUSED( store );
    //TODO: Integrate to music store
}

/*!
 * Public slot
 * Tunes to the given frequency
 */
void RadioUiEngine::tuneFrequency( uint frequency, const int reason )
{
    Q_D( RadioUiEngine );
    if ( frequency != d->mStationModel->currentStation().frequency() && d->mEngineWrapper->isFrequencyValid( frequency ) ) {
        LOG_FORMAT( "RadioUiEngine::tuneFrequency, frequency: %d", frequency );
        d->cancelSeeking();
        d->mEngineWrapper->tuneFrequency( frequency, reason );
    }
}

/*!
 * Public slot
 * Tunes to the given frequency after a delay
 */
void RadioUiEngine::tuneWithDelay( uint frequency, const int reason )
{
    Q_D( RadioUiEngine );
    if ( frequency != d->mStationModel->currentStation().frequency() &&  d->mEngineWrapper->isFrequencyValid( frequency ) ) {
        LOG_FORMAT( "RadioEngineWrapperPrivate::tuneWithDelay, frequency: %d", frequency );
        d->cancelSeeking();
        d->mEngineWrapper->tuneWithDelay( frequency, reason );
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

            d->mEngineWrapper->tuneFrequency( station.frequency(), TuneReason::Unspecified );
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
    if ( !isScanning() ) {
        d->mEngineWrapper->setMute( !d->mEngineWrapper->isMuted() );
    }
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
void RadioUiEngine::seekStation( int seekDirection )
{
    if ( isAntennaAttached() ) {
        Q_D( RadioUiEngine );
        Seeking::Direction direction = static_cast<Seeking::Direction>( seekDirection );
        emitSeekingStarted( direction );
        d->mEngineWrapper->startSeeking( direction, TuneReason::Seek );
    }
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
    Q_UNUSED( started );
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
 * Function used by the private implementation to emit a antennaStatusChanged signal
 */
void RadioUiEngine::emitAntennaStatusChanged( bool connected )
{
    emit antennaStatusChanged( connected );
}

