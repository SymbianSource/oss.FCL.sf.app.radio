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
#include <qobject>
#include <qstring>
//#include <qlist>

// User includes
#include "radiouiengineexport.h"
#include "radio_global.h"

// Forward declarations
class RadioUiEnginePrivate;
class RadioStationModel;
class RadioSettings;
class RadioStation;
class RadioPlayLogModel;
class RadioStationFilterModel;

class UI_ENGINE_DLL_EXPORT RadioUiEngine : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D( d_ptr, RadioUiEngine )
    Q_DISABLE_COPY( RadioUiEngine )

public:

    static bool isOfflineProfile();
    static QString parseFrequency( uint frequency );
    static QString nameOrFrequency( const RadioStation& station, uint frequency = 0 );

    RadioUiEngine( QObject* parent = 0 );
    ~RadioUiEngine();

    bool startRadio();

    bool isFirstTimeStart();

    /**
     * Getters for things owned by the engine
     */
    RadioSettings& settings();
    RadioStationModel& model();
    RadioPlayLogModel& playLogModel();
    RadioStationFilterModel* createNewFilterModel( QObject* parent = 0 );

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

    void scanFrequencyBand();
    void cancelScanFrequencyBand();

    QList<RadioStation> stationsInRange( uint minFrequency, uint maxFrequency );

    QString genreToString( int genre );

    bool isSongRecognitionAppAvailable();

    void addRecognizedSong( const QString& artist, const QString& title );

signals:

    void tunedToFrequency( uint frequency, int commandSender );
    void seekingStarted( int direction );
    void radioStatusChanged( bool radioIsOn );

    void rdsAvailabilityChanged( bool available );

    void volumeChanged( int volume );
    void muteChanged( bool muted );

    void audioRouteChanged( bool loudspeaker );
    void scanAndSaveFinished();
    void headsetStatusChanged( bool connected );

public slots:

    /**
     * Slots to tune to given frequency or preset
     */
    void tuneFrequency( uint frequency, const int sender = CommandSender::Unspecified );
    void tuneWithDelay( uint frequency, const int sender = CommandSender::Unspecified );
    void tunePreset( int presetIndex );

    /*!
     * volume update command slot for the engine
     */
    void setVolume( int volume );
    void toggleMute();
    void toggleAudioRoute();

    void skipPrevious();
    void skipNext();

    void seekUp();
    void seekDown();

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
    void emitScanAndSaveFinished();
    void emitheadsetStatusChanged( bool connected );

private: // data

    /**
     * Unmodifiable pointer to the private implementation
     */
    RadioUiEnginePrivate* const d_ptr;

};


#endif // RADIOUIENGINE_H_
