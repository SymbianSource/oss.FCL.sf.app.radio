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
#include <HbProgressDialog>
#include <QTimer>
#include <HbLabel>
#include <HbPushButton>

// User includes
#include "radiofrequencyscanner.h"
#include "radioscannerengine.h"
#include "radiouiengine.h"
#include "radiologger.h"
#include "radiostationmodel.h"
#include "radiofrequencystrip.h"
#include "radiostationcarousel.h"
#include "radiouiutilities.h"
#include "radiomainview.h"

// Constants
const int KExtraRoomToMaxValue = 100000;

/*!
 *
 */
RadioFrequencyScanner::RadioFrequencyScanner( RadioUiEngine& uiEngine, QObject* parent ) :
    QObject( parent ),
    mUiEngine( uiEngine ),
    mInMainView( parent->metaObject()->className() == RadioMainView::staticMetaObject.className() ),
    mScannerEngine( mUiEngine.scannerEngine() ),
    mStripScrollTime( 0 ),
    mCarouselScrollTime( 0 ),
    mIsAlive( false )
{
    RadioUiUtilities::setFrequencyScanner( this );
}

/*!
 *
 */
RadioFrequencyScanner::~RadioFrequencyScanner()
{
}

/*!
 *
 */
void RadioFrequencyScanner::startScanning()
{
    mIsAlive = true;
    RadioFrequencyStrip* frequencyStrip = RadioUiUtilities::frequencyStrip();
    RadioStationCarousel* carousel = RadioUiUtilities::carousel();

    if ( mInMainView ) {
        mStripScrollTime = frequencyStrip->autoScrollTime();
        mCarouselScrollTime = carousel->autoScrollTime();

        carousel->setScanningMode( true );
        carousel->setAutoScrollTime( 1000 );
        frequencyStrip->setAutoScrollTime( 1100 );

        connectAndTest( carousel,               SIGNAL(scanAnimationFinished()),
                        this,                   SLOT(continueScanning()) );

        static_cast<RadioMainView*>( parent() )->setScanningMode( true );
        frequencyStrip->setScanningMode( true );
    } else {
        carousel->setCarouselModel( NULL );

        mScanningProgressNote.reset( new HbProgressDialog( HbProgressDialog::ProgressDialog ) ),
        mScanningProgressNote->setModal( true );
        mScanningProgressNote->setAutoClose( true );

        // Add some extra to the maximum value to allow room for the station at the low band edge
        mScanningProgressNote->setRange( mUiEngine.minFrequency(), mUiEngine.maxFrequency() + KExtraRoomToMaxValue );
        mScanningProgressNote->setProgressValue( mUiEngine.minFrequency() );
        mScanningProgressNote->setText( hbTrId( "txt_rad_info_searching_local_stations_please_wait" ) );
        mScanningProgressNote->show();

        connectAndTest( mScanningProgressNote.data(),   SIGNAL(cancelled()),
                        this,                           SLOT(cancelScanning()) );
    }

    connectAndTest( mScannerEngine.data(),  SIGNAL(stationFound(RadioStation)),
                    this,                   SLOT(updateScanProgress(RadioStation)) );

    QTimer::singleShot( 1000, this, SLOT(delayedStart()) );
}

/*!
 *
 */
bool RadioFrequencyScanner::isAlive() const
{
    return mIsAlive;
}

/*!
 * Public slot
 *
 */
void RadioFrequencyScanner::cancelScanning()
{
    finishScanning();
}

/*!
 * Private slot
 *
 */
void RadioFrequencyScanner::delayedStart()
{
    mScannerEngine->startScanning();
}

/*!
 * Private slot
 *
 */
void RadioFrequencyScanner::updateScanProgress( const RadioStation& station )
{
    if ( !station.isValid() ) {
        finishScanning();
        return;
    }

    const uint frequency = station.frequency();
    LOG_FORMAT( "RadioFrequencyScanner::updateScanAndSaveProgress frequency: %d", frequency );

    if ( mInMainView ) {

        RadioUiUtilities::frequencyStrip()->setFrequency( frequency, TuneReason::StationScan );
        RadioUiUtilities::carousel()->animateNewStation( station );

    } else {
        // Check for special case that can happen during scanning.
        // If there is a valid radio station at the low frequency band edge it will be reported last after
        // all of the higher frequencies. We don't update the progress value here because the value would
        // be lower than the previous one. The progress value is set to maximum when the scanner finishes.
        if ( frequency != mUiEngine.minFrequency() ) {
            mScanningProgressNote->setProgressValue( frequency );
        }

        mScannerEngine->continueScanning();
    }    
}

/*!
 * Private slot
 *
 */
void RadioFrequencyScanner::continueScanning()
{
    mScannerEngine->continueScanning();
}

/*!
 * Private slot
 *
 */
void RadioFrequencyScanner::restoreUiControls()
{
    if ( mInMainView ) {
        RadioUiUtilities::frequencyStrip()->setScanningMode( false );
        static_cast<RadioMainView*>( parent() )->setScanningMode( false );
        RadioUiUtilities::carousel()->setScanningMode( false );
    }

    deleteLater();
}

/*!
 *
 */
void RadioFrequencyScanner::finishScanning()
{
    mScannerEngine->cancel();
    RadioUiUtilities::setFrequencyScanner( NULL );
    mIsAlive = false;
    RadioFrequencyStrip* frequencyStrip = RadioUiUtilities::frequencyStrip();
    RadioStationCarousel* carousel = RadioUiUtilities::carousel();

    if ( mInMainView ) {
        RadioStationModel& model = mUiEngine.stationModel();

        // Scroll the carousel and frequency strip through all of the scanned stations
        const int stationCount = model.rowCount();
        if ( stationCount > 1 ) {
            frequencyStrip->setAutoScrollTime( 1000 );
            carousel->setAutoScrollTime( 1000 );
            const uint frequency = model.data( model.index( 0, 0 ), RadioStationModel::RadioStationRole ).value<RadioStation>().frequency();
            frequencyStrip->setFrequency( frequency, TuneReason::StationScan );
            carousel->setFrequency( frequency, TuneReason::StationScan );

            frequencyStrip->setAutoScrollTime( mStripScrollTime );
            carousel->setAutoScrollTime( mCarouselScrollTime );
        }

        QTimer::singleShot( 100, this, SLOT(restoreUiControls()) );

    } else {
        mScanningProgressNote->setProgressValue( mScanningProgressNote->maximum() );
        deleteLater();

        carousel->setCarouselModel( mUiEngine.carouselModel() );
    }

    disconnect( mScannerEngine.data(),  SIGNAL(stationFound(RadioStation)),
                this,                   SLOT(updateScanAndSaveProgress(RadioStation)) );

    emit frequencyScannerFinished();
}
