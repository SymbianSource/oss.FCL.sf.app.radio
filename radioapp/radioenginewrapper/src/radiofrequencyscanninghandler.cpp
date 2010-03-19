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
#include <qtimer>

// User includes
#include "radiofrequencyscanninghandler.h"
#include "radioenginewrapper_p.h"
#include "cradioenginehandler.h"
#include "radiostationhandlerif.h"

/*!
 *
 */
RadioFrequencyScanningHandler::RadioFrequencyScanningHandler( RadioEngineWrapperPrivate& radioEngine ) :
    QObject( 0 ),
    mEngine( radioEngine ),
    mLastFoundFrequency( 0 )
{
}

/*!
 * From MRadioScanObserver
 *
 */
void RadioFrequencyScanningHandler::ScanFrequencyEventL( TUint32 aFrequency )
{
    // The scanning logic starts from the minimun frequency and advances as long as
    // the found frequency is higher than the last one. When it loops around from the highest
    // frequency back to the lowest one we check if it stops at the minimun frequency and
    // add the station there. This logic handles the special case where there is a station in the
    // minimum frequency, but does not work if there are no stations at all. The adaptation has a
    // timeout timer that stops the scanning if nothing is found and reports the scanning start
    // frequency which is this case will be the minimum frequency. In that case this logic will
    // assume that it was an audible station and adds it.

    uint frequency = static_cast<uint>( aFrequency );
    mEngine.mStationHandler.setCurrentStation( frequency );
    if ( frequency > mLastFoundFrequency ) {
        mLastFoundFrequency = frequency;
        addFoundStation( frequency );
    } else if ( frequency == mEngine.mEngineHandler->MinFrequency() ) {
        // Special case. A station has been found in the mininmum frequency
        addFoundStation( frequency );
    }
}

/*!
 * From MRadioScanObserver
 *
 */
void RadioFrequencyScanningHandler::ScanCompletedEventL( TInt aError )
{
    Q_UNUSED( aError )
  
    mEngine.frequencyScannerFinished(); // Causes the scanner to be deleted so nothing can be done after this
}


/*!
 * Starts the scanning
 */
void RadioFrequencyScanningHandler::startScanning( bool muted )
{
    cancel();
    QTimer::singleShot( 1000, this, SLOT(delayedStart()) );
}

/*!
 * Adds a new station that was found
 */
void RadioFrequencyScanningHandler::addFoundStation( const uint frequency )
{
    mEngine.mStationHandler.addScannedFrequency( frequency );
}

/*!
 * Cancels the scanning process
 */
void RadioFrequencyScanningHandler::cancel()
{
    mEngine.mEngineHandler->StopScan();
}

/*!
 *
 */
void RadioFrequencyScanningHandler::delayedStart()
{
    mEngine.mStationHandler.removeLocalStations();

    mLastFoundFrequency = mEngine.mEngineHandler->MinFrequency();
    mEngine.mEngineHandler->StartScan( *this );
}
