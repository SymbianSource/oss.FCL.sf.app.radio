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

#ifndef RADIOENGINEWRAPPER_H
#define RADIOENGINEWRAPPER_H

// System includes

// User includes
#include "radiowrapperexport.h"
#include "radio_global.h"

// Forward declarations
class RadioEngineWrapperPrivate;
class RadioSettingsIf;
class RadioStationHandlerIf;
class RadioEngineWrapperObserver;

// Class declaration
class WRAPPER_DLL_EXPORT RadioEngineWrapper
{
    Q_DECLARE_PRIVATE_D( d_ptr, RadioEngineWrapper )
    Q_DISABLE_COPY( RadioEngineWrapper )

public:

    /**
     * Constructor and destructor
     */
    RadioEngineWrapper( RadioStationHandlerIf& stationHandler );
    ~RadioEngineWrapper();

    void addObserver( RadioEngineWrapperObserver* observer );
    void removeObserver( RadioEngineWrapperObserver* observer );

    /**
     * Checks if the radio engine has been constructed properly
     */
    bool isEngineConstructed();

    /**
     * Getters for things owned by the engine
     */
    RadioSettingsIf& settings();

    /**
     * Getters for region and other region dependent settings
     */
    RadioRegion::Region region() const;
    uint minFrequency() const;
    uint maxFrequency() const;
    uint frequencyStepSize() const;
    bool isFrequencyValid( uint frequency );

    /**
     * Getters for current radio status
     */
    bool isRadioOn() const;
    uint currentFrequency() const;
    bool isMuted() const;
    bool isAntennaAttached() const;
    bool isUsingLoudspeaker() const;

    /**
     * Functions to tune to given frequency or preset
     */
    void tuneFrequency( uint frequency, const int reason = TuneReason::Unspecified );
    void tuneWithDelay( uint frequency, const int reason = TuneReason::Unspecified );

    /*!
     * Audio update command functions for the engine
     */
    void setVolume( int volume );
    void setMute( bool muted );
    void toggleAudioRoute();

    void startSeeking( Seeking::Direction direction, const int reason = TuneReason::Unspecified );
    void cancelSeeking();

private: // data

    /**
     * Unmodifiable pointer to the private implementation
     */
    RadioEngineWrapperPrivate* const d_ptr;

};

#endif // RADIOENGINEWRAPPER_H
