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

#ifndef RADIOENGINEWRAPPER_P_H
#define RADIOENGINEWRAPPER_P_H

// System includes
#include <QObject>
#include <QScopedPointer>

// User includes
#include "radioenginewrapper.h"

// Forward declarations
class RadioControlEventListener;
class RadioRdsListener;
class RadioSettings;
class RadioFrequencyScanningHandler;
class RadioStationHandlerIf;
class RadioEngineWrapperObserver;
class T_RadioDataParser;
class QSettings;
class QTimer;

// Class declaration
class WRAPPER_DLL_EXPORT RadioEngineWrapperPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC( RadioEngineWrapper )
    Q_DISABLE_COPY( RadioEngineWrapperPrivate )

    friend class RadioFrequencyScanningHandler;

public:

    RadioEngineWrapperPrivate( RadioEngineWrapper* wrapper,
                               RadioStationHandlerIf& stationHandler,
                               RadioEngineWrapperObserver& observer );

    ~RadioEngineWrapperPrivate();

    static RadioEngineWrapperPrivate* instance();

    /**
     * Initialization and startup
     */
    void init();
    bool isEngineConstructed();

    /**
     * Returns the radio settings
     */
    RadioSettings& settings();

    /**
     * Functions called from slots to tune to given frequency or preset
     */
    void tuneFrequency( uint frequency, const int sender );
    void tuneWithDelay( uint frequency, const int sender );

    RadioEngineWrapperObserver& observer();

    void startSeeking( Seeking::Direction direction );

// Functions used by the win32 test window

    QString dataParsingError() const;

    void setHeadsetStatus( bool connected );

    void setVolume( int volume );

    void addSong( const QString& artist, const QString& title );

    void clearSong();

    bool isOffline() const;
    void setOffline( bool offline );

private slots:

    void frequencyEvent();
    void addSongTags();

private:

// New functions

    /**
     * Called by RadioFrequencyScanningHandler when the scanning has finished
     */
    void frequencyScannerFinished();

    void parseData();

private: // data

    /**
     * Pointer to the public class
     * Not own.
     */
    RadioEngineWrapper*                             q_ptr;

    /**
     * Map of radio stations read from the radio engine
     * Own.
     */
    RadioStationHandlerIf&                          mStationHandler;

    /**
     * Reference to the wrapper observer
     */
    RadioEngineWrapperObserver&                     mObserver;

    /**
     * Radio settings handler
     * Own.
     */
    QScopedPointer<RadioSettings>                   mSettings;

    QTimer*                                         mTuneTimer;

    /**
     * Preset scanning handler
     * Own.
     */
//    QScopedPointer<RadioFrequencyScanningHandler>   mFrequencyScanningHandler;

//    QScopedPointer<T_RadioDataParser>               mDataParser;

    QString                                         mParsingError;

    /**
     * Id of the sender of the last tune command. RadioFrequencyStrip or someone else
     */
    int                                             mCommandSender;

    /**
     * Flag to indicate whether or not audio should be routed to loudspeaker
     */
    bool                                            mUseLoudspeaker;

    /**
     * Flag to indicate whether or not the engine is seeking
     */
    bool                                            mIsSeeking;

    QScopedPointer<QSettings>                       mEngineSettings;

    bool                                            mAntennaAttached;

    uint                                            mFrequency;
    uint                                            mNextFrequency;

    int                                             mVolume;

    int                                             mMaxVolume;
    int                                             mFrequencyStepSize;
    RadioRegion::Region                             mRegionId;
    uint                                            mMinFrequency;
    uint                                            mMaxFrequency;

    QString                                         mArtist;
    QString                                         mTitle;

};

#endif // RADIOENGINEWRAPPER_P_H
