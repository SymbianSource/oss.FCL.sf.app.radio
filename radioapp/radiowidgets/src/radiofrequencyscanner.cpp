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
#include <hbprogressdialog.h>

// User includes
#include "radiofrequencyscanner.h"
#include "radiolocalization.h"
#include "radiouiengine.h"
#include "radiologger.h"
#include "radiostationmodel.h"

// Constants
const int KExtraRoomToMaxValue = 100000;

/*!
 *
 */
RadioFrequencyScanner::RadioFrequencyScanner( RadioUiEngine& uiEngine, QObject* parent ) :
    QObject( parent ),
    mUiEngine( uiEngine ),
    mScanningProgressNote( new HbProgressDialog( HbProgressDialog::ProgressDialog ) ),
    mChannelCount( 0 ),
    mMinFrequency( 0 )
{
    mScanningProgressNote->setModal( true );
    mScanningProgressNote->setAutoClose( true );

    mMinFrequency = mUiEngine.minFrequency();

    // Add some extra to the maximum value to allow room for the station at the low band edge
    mScanningProgressNote->setRange( mMinFrequency, mUiEngine.maxFrequency() + KExtraRoomToMaxValue );
    mScanningProgressNote->setProgressValue( mMinFrequency );
    mScanningProgressNote->setText( TRANSLATE( KProgressTitleScanStations ) );

    RadioStationModel* stationModel = &mUiEngine.model();
    connectAndTest( stationModel,           SIGNAL(stationAdded(RadioStation)),
                    this,                   SLOT(updateScanAndSaveProgress(RadioStation)) );

    connectAndTest( &mUiEngine,             SIGNAL(scanAndSaveFinished()),
                    this,                   SLOT(scanAndSavePresetsFinished()) );

    connectAndTest( mScanningProgressNote,  SIGNAL(cancelled()),
                    this,                   SLOT(scanAndSavePresetsCancelled()) );
}

/*!
 *
 */
void RadioFrequencyScanner::startScanning()
{
    mUiEngine.scanFrequencyBand();
    mScanningProgressNote->show();
}

/*!
 * Private slot
 *
 */
void RadioFrequencyScanner::updateScanAndSaveProgress( const RadioStation& station )
{
    const uint frequency = station.frequency();
    LOG_FORMAT( "RadioFrequencyScanner::updateScanAndSaveProgress frequency: %d", frequency );

    // Check for special case that can happen during scanning.
    // If there is a valid radio station at the low frequency band edge it will be reported last after
    // all of the higher frequencies. We don't update the progress value here because the value would
    // be lower than the previous one. The progress value is set to maximum when the scanner finishes.
    if ( frequency != mMinFrequency ) {
        mScanningProgressNote->setProgressValue( frequency );
    }

    ++mChannelCount;
    mScanningProgressNote->setText( QString( TRANSLATE( KProgressTitleStationsFound ) ).arg( mChannelCount ) );
}

/*!
 * Private slot
 *
 */
void RadioFrequencyScanner::scanAndSavePresetsFinished()
{
    mScanningProgressNote->setProgressValue( mScanningProgressNote->maximum() );

    disconnect( &mUiEngine.model(),     SIGNAL(stationAdded(RadioStation)),
                this,                   SLOT(updateScanAndSaveProgress(RadioStation)) );
    disconnect( &mUiEngine,             SIGNAL(scanAndSaveFinished()),
                this,                   SLOT(scanAndSavePresetsFinished()) );

    emit frequencyScannerFinished();
    mScanningProgressNote->deleteLater();
    deleteLater();
}

/*!
 * Private slot
 *
 */
void RadioFrequencyScanner::scanAndSavePresetsCancelled()
{
    mUiEngine.cancelScanFrequencyBand();
    scanAndSavePresetsFinished();
    mScanningProgressNote = 0;
    mChannelCount = 0;
}
