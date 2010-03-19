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

// User includes
#include "radioenginewrapper.h"
#include "radioenginewrapper_p.h"
#include "cradioenginehandler.h"
#include "radiofrequencyscanninghandler.h"

/*!
 * Constructor
 */
RadioEngineWrapper::RadioEngineWrapper( RadioStationHandlerIf& stationHandler,
                                        RadioEngineWrapperObserver& observer ) :
    d_ptr( new RadioEngineWrapperPrivate( this, stationHandler, observer ) )
{
    Q_D( RadioEngineWrapper );
    d->init();
}

/*!
 * Destructor
 */
RadioEngineWrapper::~RadioEngineWrapper()
{
    delete d_ptr;
}

/*!
 * Checks if the radio engine has been constructed properly
 */
bool RadioEngineWrapper::isEngineConstructed()
{
    Q_D( RadioEngineWrapper );
    return d->isEngineConstructed();
}

/*!
 * Returns the settings handler owned by the engine
 */
RadioSettings& RadioEngineWrapper::settings()
{
    Q_D( RadioEngineWrapper );
    return d->settings();
}

/*!
 * Returns the selected radio region
 */
RadioRegion::Region RadioEngineWrapper::region() const
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->Region();
}

/*!
 * Returns the minimum frequency
 */
uint RadioEngineWrapper::minFrequency() const
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->MinFrequency();
}

/*!
 * Returns the maximum frequency
 */
uint RadioEngineWrapper::maxFrequency() const
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->MaxFrequency();
}

/*!
 * Returns the frequency step size from the selected region
 */
uint RadioEngineWrapper::frequencyStepSize() const
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->FrequencyStepSize();
}

/*!
 * Returns the frequency step size from the selected region
 */
bool RadioEngineWrapper::isFrequencyValid( uint frequency )
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->IsFrequencyValid( frequency );
}

/*!
 * Checks if the radio engine is on
 */
bool RadioEngineWrapper::isRadioOn() const
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->IsRadioOn();
}

/*!
 * Checks if the scan is on
 */
bool RadioEngineWrapper::isScanning() const
{
    Q_D( const RadioEngineWrapper );
    return !d->mFrequencyScanningHandler.isNull();
}

/*!
 * Returns the currently tuned frequency
 */
uint RadioEngineWrapper::currentFrequency() const
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->TunedFrequency();
}

/*!
 * Returns the mute status
 */
bool RadioEngineWrapper::isMuted() const
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->IsMuted();
}

/*!
 * Returns the headset connection status
 */
bool RadioEngineWrapper::isAntennaAttached() const
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->IsAntennaAttached();
}

/*!
 * Returns the "use loudspeaker" status
 */
bool RadioEngineWrapper::isUsingLoudspeaker() const
{
    Q_D( const RadioEngineWrapper );
    return d->mUseLoudspeaker;
}

/*!
 * Tunes to the given frequency
 */
void RadioEngineWrapper::tuneFrequency( uint frequency, const int sender )
{
    Q_D( RadioEngineWrapper );
    d->tuneFrequency( frequency, sender );
}

/*!
 * Tunes to the given frequency after a delay
 */
void RadioEngineWrapper::tuneWithDelay( uint frequency, const int sender )
{
    Q_D( RadioEngineWrapper );
    d->tuneWithDelay( frequency, sender );
}

/*!
 * volume update command for the engine
 */
void RadioEngineWrapper::setVolume( int volume )
{
    Q_D( RadioEngineWrapper );
    d->mEngineHandler->SetVolume( volume );
}

/*!
 *
 */
void RadioEngineWrapper::toggleMute()
{
    Q_D( RadioEngineWrapper );
    d->mEngineHandler->SetMuted( !d->mEngineHandler->IsMuted() );
}

/*!
 *
 */
void RadioEngineWrapper::toggleAudioRoute()
{
    Q_D( RadioEngineWrapper );
    d->mUseLoudspeaker = !d->mUseLoudspeaker;
    d->mEngineHandler->SetAudioRouteToLoudspeaker( d->mUseLoudspeaker );
}

/*!
 *
 */
void RadioEngineWrapper::startSeeking( Seeking::Direction direction )
{
    Q_D( RadioEngineWrapper );
    d->startSeeking( direction );
}

/*!
 *
 */
void RadioEngineWrapper::scanFrequencyBand()
{
    Q_D( RadioEngineWrapper );
    if ( !d->mFrequencyScanningHandler ) {
        d->mFrequencyScanningHandler.reset( new RadioFrequencyScanningHandler( *d ) );
    }
    d->mFrequencyScanningHandler->startScanning( d->mEngineHandler->IsMuted() );
}

/*!
 *
 */
void RadioEngineWrapper::cancelScanFrequencyBand()
{
    Q_D( RadioEngineWrapper );
    d->mFrequencyScanningHandler->cancel();
    d->frequencyScannerFinished();
}