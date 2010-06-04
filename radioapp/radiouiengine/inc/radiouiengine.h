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


#ifndef RADIOUIENGINE_H_
#define RADIOUIENGINE_H_

// System includes
#include <QObject>
#include <QString>

// User includes
#include "radiouiengineexport.h"
#include "radio_global.h"

// Forward declarations
class RadioUiEnginePrivate;
class RadioStationModel;
class RadioSettingsIf;
class RadioStation;
class RadioHistoryModel;
class RadioHistoryItem;
class RadioCarouselModel;
class RadioStationFilterModel;
class RadioScannerEngine;
class RadioMonitorService;

namespace GenreTarget
{
    enum Target{
        Carousel,
        StationsList,
        HomeScreen
    };
}


class UI_ENGINE_DLL_EXPORT RadioUiEngine : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D( d_ptr, RadioUiEngine )
    Q_DISABLE_COPY( RadioUiEngine )

    friend class RadioScannerEngine;

public:

    /**
     * Static functions that are used before the ui engine is created
     */
    static bool isOfflineProfile();
    static uint lastTunedFrequency();

    RadioUiEngine( QObject* parent = 0 );
    ~RadioUiEngine();

    bool isInitialized() const;

    bool init();

    bool isFirstTimeStart();

    /**
     * Getters for things owned by the engine
     */
    RadioSettingsIf& settings();
    RadioStationModel& stationModel();
    RadioHistoryModel& historyModel();
    RadioStationFilterModel* createNewFilterModel( QObject* parent = 0 );
    RadioCarouselModel* carouselModel();
    RadioScannerEngine* scannerEngine();

    bool isRadioOn() const;
    bool isScanning() const;
    bool isMuted() const;
    bool isAntennaAttached() const;
    bool isUsingLoudspeaker() const;

    RadioRegion::Region region() const;
    uint currentFrequency() const;
    uint minFrequency() const;
    uint maxFrequency() const;
    uint frequencyStepSize() const;

    void setMute( bool muted );

    QList<RadioStation> stationsInRange( uint minFrequency, uint maxFrequency );

    QString genreToString( int genre, GenreTarget::Target target );

    bool isSongRecognitionAppAvailable();

    void addRecognizedSong( const QString& artist, const QString& title, const RadioStation& station );

    uint skipStation( StationSkip::Mode mode, uint startFrequency = 0 );

    enum MusicStore{ OviStore, OtherStore };
    void openMusicStore( const RadioHistoryItem& item, MusicStore store = OviStore );

signals:

    void tunedToFrequency( uint frequency, int commandSender );
    void seekingStarted( int direction );
    void radioStatusChanged( bool radioIsOn );

    void rdsAvailabilityChanged( bool available );

    void volumeChanged( int volume );
    void muteChanged( bool muted );

    void audioRouteChanged( bool loudspeaker );
    void antennaStatusChanged( bool connected );

public slots:

    /**
     * Slots to tune to given frequency or preset
     */
    void tuneFrequency( uint frequency, const int reason = TuneReason::Unspecified );
    void tuneWithDelay( uint frequency, const int reason = TuneReason::Unspecified );
    void tunePreset( int presetIndex );

    /*!
     * volume update command slot for the engine
     */
    void setVolume( int volume );
    void toggleMute();
    void toggleAudioRoute();

    void seekStation( int seekDirection );

    void launchSongRecognition();

private:

    /**
     * functions used only by the private class to get signals emitted
     */
    void emitTunedToFrequency( uint frequency, int commandSender );
    void emitSeekingStarted( Seeking::Direction direction );
    void emitRadioStatusChanged( bool radioIsOn );
    void emitRdsAvailabilityChanged( bool available );
    void emitVolumeChanged( int volume );
    void emitMuteChanged( bool muted );
    void emitAudioRouteChanged( bool loudspeaker );
    void emitAntennaStatusChanged( bool connected );

private: // data

    /**
     * Unmodifiable pointer to the private implementation
     */
    RadioUiEnginePrivate* const d_ptr;

};


#endif // RADIOUIENGINE_H_
