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
#include "radioenginewrapper_win32_p.h"
#include "radioenginewrapperobserver.h"

/*!
 * Constructor
 */
RadioEngineWrapper::RadioEngineWrapper( RadioStationHandlerIf& stationHandler ) :
    d_ptr( new RadioEngineWrapperPrivate( this, stationHandler ) )
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
 *
 */
void RadioEngineWrapper::addObserver( RadioEngineWrapperObserver* observer )
{
    Q_D( RadioEngineWrapper );
    d->mObservers.append( observer );
}

/*!
 *
 */
void RadioEngineWrapper::removeObserver( RadioEngineWrapperObserver* observer )
{
    Q_D( RadioEngineWrapper );
    d->mObservers.removeAll( observer );
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
RadioSettingsIf& RadioEngineWrapper::settings()
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
    return d->mRegionId;
}

/*!
 * Returns the minimum frequency
 */
uint RadioEngineWrapper::minFrequency() const
{
    Q_D( const RadioEngineWrapper );
    return d->mMinFrequency;
}

/*!
 * Returns the maximum frequency
 */
uint RadioEngineWrapper::maxFrequency() const
{
    Q_D( const RadioEngineWrapper );
    return d->mMaxFrequency;
}

/*!
 * Returns the frequency step size from the selected region
 */
uint RadioEngineWrapper::frequencyStepSize() const
{
    Q_D( const RadioEngineWrapper );
    return d->mFrequencyStepSize;
}

/*!
 * Returns the frequency step size from the selected region
 */
bool RadioEngineWrapper::isFrequencyValid( uint frequency )
{
    Q_UNUSED( frequency );
    return true;
}

/*!
 * Checks if the radio engine is on
 */
bool RadioEngineWrapper::isRadioOn() const
{
    return true;
}

/*!
 * Returns the currently tuned frequency
 */
uint RadioEngineWrapper::currentFrequency() const
{
    Q_D( const RadioEngineWrapper );
    return d->mFrequency;
}

/*!
 * Returns the mute status
 */
bool RadioEngineWrapper::isMuted() const
{
    return false;
}

/*!
 * Returns the antenna connection status
 */
bool RadioEngineWrapper::isAntennaAttached() const
{
    Q_D( const RadioEngineWrapper );
    return d->mAntennaAttached;
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
    d->setVolume( volume );
}

/*!
 *
 */
void RadioEngineWrapper::setMute( bool muted )
{
}

/*!
 *
 */
void RadioEngineWrapper::toggleAudioRoute()
{
    Q_D( RadioEngineWrapper );
    d->toggleAudioRoute();
}

/*!
 *
 */
void RadioEngineWrapper::startSeeking( Seeking::Direction direction, const int reason )
{
    Q_D( RadioEngineWrapper );
    d->startSeeking( direction, reason );
}

/*!
 *
 */
void RadioEngineWrapper::cancelSeeking()
{
    Q_D( RadioEngineWrapper );
    d->cancelSeeking();
}
