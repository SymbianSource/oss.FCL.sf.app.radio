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
class RadioSettings;
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
    RadioEngineWrapper( RadioStationHandlerIf& stationHandler, RadioEngineWrapperObserver& observer );
    ~RadioEngineWrapper();

    /**
     * Checks if the radio engine has been constructed properly
     */
    bool isEngineConstructed();

    /**
     * Getters for things owned by the engine
     */
    RadioSettings& settings();

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
    bool isScanning() const;
    uint currentFrequency() const;
    bool isMuted() const;
    bool isAntennaAttached() const;
    bool isUsingLoudspeaker() const;

    /**
     * Slots to tune to given frequency or preset
     */
    void tuneFrequency( uint frequency, const int sender = CommandSender::Unspecified );
    void tuneWithDelay( uint frequency, const int sender = CommandSender::Unspecified );

    /*!
     * volume update command slot for the engine
     */
    void setVolume( int volume );
    void toggleMute();
    void toggleAudioRoute();

    void startSeeking( Seeking::Direction direction );
    void scanFrequencyBand();
    void cancelScanFrequencyBand();

private: // data

    /**
     * Unmodifiable pointer to the private implementation
     */
    RadioEngineWrapperPrivate* const d_ptr;

};

#endif // RADIOENGINEWRAPPER_H
