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
#include "radiotuningview.h"
#include "radioxmluiloader.h"

// Constants
const int KExtraRoomToMaxValue = 100000;

/*!
 *
 */
RadioFrequencyScanner::RadioFrequencyScanner( RadioUiEngine& uiEngine, QObject* parent ) :
    QObject( parent ),
    mUiEngine( uiEngine ),
    mInTuningView( parent->metaObject()->className() == RadioTuningView::staticMetaObject.className() ),
    mScannerEngine( mUiEngine.createScannerEngine() ),
    mScanningProgressNote( new HbProgressDialog( HbProgressDialog::ProgressDialog ) ),
    mChannelCount( 0 ),
    mStripScrollTime( 0 ),
    mCarouselScrollTime( 0 )
{
}

/*!
 *
 */
RadioFrequencyScanner::~RadioFrequencyScanner()
{
    restoreUiControls();
}

/*!
 *
 */
void RadioFrequencyScanner::startScanning( RadioXmlUiLoader& uiLoader )
{
    mChannelCount = 0;
    RadioFrequencyStrip* frequencyStrip = RadioUiUtilities::frequencyStrip();
    RadioStationCarousel* carousel = RadioUiUtilities::carousel();

    disconnect( &mUiEngine,         SIGNAL(tunedToFrequency(uint,int)),
                frequencyStrip,     SLOT(setFrequency(uint,int)) );
    disconnect( frequencyStrip,     SIGNAL(frequencyChanged(uint,int)),
                &mUiEngine,         SLOT(tuneWithDelay(uint,int)) );
    disconnect( carousel,           SIGNAL(frequencyChanged(uint,int)),
                frequencyStrip,     SLOT(setFrequency(uint,int)) );
    disconnect( frequencyStrip,     SIGNAL(frequencyChanged(uint,int)),
                carousel,           SLOT(setFrequency(uint)) );

    if ( mInTuningView ) {
        bool ok = false;
        uiLoader.load( DOCML::FILE_TUNINGVIEW, "scanning", &ok );

        HbLabel* infoText = uiLoader.findWidget<HbLabel>( DOCML::TV_NAME_INFO_TEXT );
        infoText->setAlignment( Qt::AlignBottom | Qt::AlignHCenter );
        infoText->setTextWrapping( Hb::TextWordWrap );

        mStripScrollTime = frequencyStrip->autoScrollTime();
        mCarouselScrollTime = carousel->autoScrollTime();

        carousel->setScanningMode( true );
        carousel->setAutoScrollTime( 1000 );
        frequencyStrip->setAutoScrollTime( 1100 );

        connectAndTest( carousel,               SIGNAL(scanAnimationFinished()),
                        this,                   SLOT(continueScanning()) );

        static_cast<RadioTuningView*>( parent() )->setScanningMode( true );
        frequencyStrip->setScanningMode( true );
        frequencyStrip->setFrequency( mUiEngine.minFrequency() );
        frequencyStrip->setFrequency( mUiEngine.minFrequency() + 100 ); // scanning jamming
    } else {
        mScanningProgressNote->setModal( true );
        mScanningProgressNote->setAutoClose( true );

        carousel->setStationModel( NULL );

        // Add some extra to the maximum value to allow room for the station at the low band edge
        mScanningProgressNote->setRange( mUiEngine.minFrequency(), mUiEngine.maxFrequency() + KExtraRoomToMaxValue );
        mScanningProgressNote->setProgressValue( mUiEngine.minFrequency() );
        mScanningProgressNote->setText( hbTrId( "txt_rad_info_searching_local_stations_please_wait" ) );
        mScanningProgressNote->show();

        connectAndTest( mScanningProgressNote,  SIGNAL(cancelled()),
                        this,                   SLOT(scanAndSavePresetsCancelled()) );
    }

    connectAndTest( mScannerEngine.data(),  SIGNAL(stationFound(RadioStation)),
                    this,                   SLOT(updateScanAndSaveProgress(RadioStation)) );

    QTimer::singleShot( 1000, this, SLOT(delayedStart()) );
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
void RadioFrequencyScanner::updateScanAndSaveProgress( const RadioStation& station )
{
    if ( !station.isValid() ) {
        scanAndSavePresetsFinished();
        return;
    }

    const uint frequency = station.frequency();
    LOG_FORMAT( "RadioFrequencyScanner::updateScanAndSaveProgress frequency: %d", frequency );

    if ( mInTuningView ) {

        RadioUiUtilities::frequencyStrip()->setFrequency( frequency, TuneReason::Unspecified );
        RadioUiUtilities::carousel()->animateNewStation( station );

    } else {
        // Check for special case that can happen during scanning.
        // If there is a valid radio station at the low frequency band edge it will be reported last after
        // all of the higher frequencies. We don't update the progress value here because the value would
        // be lower than the previous one. The progress value is set to maximum when the scanner finishes.
        if ( frequency != mUiEngine.minFrequency() ) {
            mScanningProgressNote->setProgressValue( frequency );
        }

        ++mChannelCount;
//        mScanningProgressNote->setText( QString( TRANSLATE( KProgressTitleStationsFound ) ).arg( mChannelCount ) );
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
void RadioFrequencyScanner::scanAndSavePresetsCancelled()
{
    mScannerEngine->cancel();
    scanAndSavePresetsFinished();
    mScanningProgressNote = 0;
    mChannelCount = 0;
}

/*!
 * Private slot
 *
 */
void RadioFrequencyScanner::restoreUiControls()
{
    RadioUiUtilities::frequencyStrip()->setScanningMode( false );
    static_cast<RadioTuningView*>( parent() )->setScanningMode( false );
    RadioUiUtilities::carousel()->setScanningMode( false );
//    disconnect( RadioUiUtilities::carousel(),   SIGNAL(scrollingEnded()),
//                this,                           SLOT(restoreUiControls()) );
}

/*!
 *
 */
void RadioFrequencyScanner::scanAndSavePresetsFinished()
{
    RadioFrequencyStrip* frequencyStrip = RadioUiUtilities::frequencyStrip();
    RadioStationCarousel* carousel = RadioUiUtilities::carousel();

    connectAndTest( &mUiEngine,         SIGNAL(tunedToFrequency(uint,int)),
                    frequencyStrip,     SLOT(setFrequency(uint,int)) );
    connectAndTest( frequencyStrip,     SIGNAL(frequencyChanged(uint,int)),
                    &mUiEngine,         SLOT(tuneWithDelay(uint,int)), Qt::QueuedConnection );
    connectAndTest( carousel,           SIGNAL(frequencyChanged(uint,int)),
                    frequencyStrip,     SLOT(setFrequency(uint,int)) );
    connectAndTest( frequencyStrip,     SIGNAL(frequencyChanged(uint,int)),
                    carousel,           SLOT(setFrequency(uint)) );

    if ( mInTuningView ) {
        RadioStationModel& model = mUiEngine.model();

        // Scroll the carousel and frequency strip through all of the scanned stations
        const int stationCount = model.rowCount();
        if ( stationCount > 1 ) {
//            connectAndTest( carousel,       SIGNAL(scrollingEnded()),
//                            this,           SLOT(restoreUiControls()) );

            frequencyStrip->setAutoScrollTime( 1000 );
            carousel->setAutoScrollTime( 1000 );
            const uint frequency = model.data( model.index( 0, 0 ), RadioStationModel::RadioStationRole ).value<RadioStation>().frequency();
            frequencyStrip->setFrequency( frequency );

            frequencyStrip->setAutoScrollTime( mStripScrollTime );
            carousel->setAutoScrollTime( mCarouselScrollTime );
        } else {
            QTimer::singleShot( 100, this, SLOT(restoreUiControls()) );
        }

        QTimer::singleShot( 1000, this, SLOT(deleteLater()) );

    } else {
        mScannerEngine->cancel();

        mScanningProgressNote->setProgressValue( mScanningProgressNote->maximum() );
        mScanningProgressNote->deleteLater();
        deleteLater();

        disconnect( mScanningProgressNote,  SIGNAL(cancelled()),
                    this,                   SLOT(scanAndSavePresetsCancelled()) );

        carousel->setStationModel( &mUiEngine.model() );
    }

    disconnect( mScannerEngine.data(),  SIGNAL(stationFound(RadioStation)),
                this,                   SLOT(updateScanAndSaveProgress(RadioStation)) );

    emit frequencyScannerFinished();
}
