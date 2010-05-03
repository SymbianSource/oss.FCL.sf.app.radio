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
#include <QTimer>

// User includes
#include "radioscannerengine.h"
#include "radioscannerengine_p.h"
#include "radiouiengine.h"
#include "radiouiengine_p.h"
#include "radioenginewrapper.h"
#include "radiostationhandlerif.h"
#include "radiostationmodel.h"
#include "radiostation.h"

/*!
 *
 */
RadioScannerEngine::RadioScannerEngine( RadioUiEnginePrivate& uiEngine ) :
    QObject( &uiEngine.api() ),
    d_ptr( new RadioScannerEnginePrivate( this, uiEngine ) )
{
}

/*!
 *
 */
RadioScannerEngine::~RadioScannerEngine()
{
    cancel();
    Q_D( RadioScannerEngine );
    if ( d->mMutedByScanner ) {
        d->mUiEngine.api().toggleMute();
    }
    delete d_ptr;
}

/*!
 * Starts the scanning from minimum frequency
 */
void RadioScannerEngine::startScanning()
{
    cancel();
    Q_D( RadioScannerEngine );
    if ( !d->mUiEngine.api().isMuted() ) {
        d->mUiEngine.api().toggleMute();
        d->mMutedByScanner = true;
    }

    d->mUiEngine.api().model().stationHandlerIf().removeLocalStations();
    d->mLastFoundFrequency = d->mUiEngine.api().minFrequency();
    d->mUiEngine.wrapper().tuneFrequency( d->mLastFoundFrequency, TuneReason::StationScanInitialization );
}

/*!
 * Continues the scanning upwards from current frequency
 */
void RadioScannerEngine::continueScanning()
{
    Q_D( RadioScannerEngine );
    d->mUiEngine.wrapper().startSeeking( Seeking::Up, TuneReason::StationScan );
}

/*!
 * Cancels the scanning process
 */
void RadioScannerEngine::cancel()
{
    Q_D( RadioScannerEngine );
    if ( d->mUiEngine.api().isScanning() ) {
        d->mUiEngine.cancelSeeking();
    }
    if ( d->mMutedByScanner ) {
        d->mUiEngine.api().toggleMute();
        d->mMutedByScanner = false;
    }
}

/*!
 * Adds a new station that was found
 */
void RadioScannerEngine::addScannedFrequency( const uint frequency )
{
    Q_D( RadioScannerEngine );
    if ( frequency > d->mLastFoundFrequency ) {
        // Station has been found normally
        d->mLastFoundFrequency = frequency;
        d->addFrequencyAndReport( frequency );
    } else if ( frequency == d->mUiEngine.api().minFrequency() ) {
        // Special case. A station has been found in the mininmum frequency
        d->addFrequencyAndReport( frequency );
    } else {
        // Seeking looped around the frequency band. Send invalid station as indicator that the scanning should stop
        emit stationFound( RadioStation() );
    }
}

/*!
 *
 */
void RadioScannerEngine::emitStationFound( const RadioStation& station )
{
    emit stationFound( station );
}
