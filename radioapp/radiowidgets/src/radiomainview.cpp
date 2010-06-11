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
#include <HbLabel>
#include <HbPushButton>
#include <HbAction>
#include <HbMenu>
#include <QApplication>

// User includes
#include "radiowindow.h"
#include "radiomainview.h"
#include "radiofrequencystrip.h"
#include "radiouiengine.h"
#include "radiologger.h"
#include "radiostationcarousel.h"
#include "radiouiloader.h"
#include "radioutil.h"
#include "radiostationmodel.h"
#include "radiofrequencyscanner.h"

// Constants

/*!
 *
 */
RadioMainView::RadioMainView() :
    RadioViewBase( false ),
    mCarousel( NULL ),
    mFrequencyStrip( NULL ),
    mSkippingAction( NULL ),
    mAlternateSkipping( false )
{
}

/*!
 *
 */
RadioMainView::~RadioMainView()
{
}

/*!
 * \reimp
 *
 */
void RadioMainView::setScanningMode( bool scanning )
{
    if ( scanning ) {
        loadSection( DOCML::FILE_MAINVIEW, DOCML::MV_SECTION_SCANNING );
    } else {
        loadSection( DOCML::FILE_MAINVIEW, DOCML::MV_SECTION_NORMAL );
        mFrequencyScanner.take();

        const bool firsTimeStart = mUiEngine->isFirstTimeStart();
        const int rowCount = mUiEngine->stationModel().rowCount();
        if ( firsTimeStart && rowCount != 0 ) {
            mUiEngine->setFirstTimeStartPerformed( true );
        }
    }
}

/*!
 * \reimp
 *
 */
void RadioMainView::init()
{
    LOG_METHOD;
    loadSection( DOCML::FILE_MAINVIEW, DOCML::SECTION_LAZY_LOAD );
    mCarousel = mUiLoader->findObject<RadioStationCarousel>( DOCML::MV_NAME_STATION_CAROUSEL );
    mCarousel->init( *mUiLoader, mUiEngine.data() );

    // Note! UI connections are already made in the DocML file. Here we need to connect UI to engine
    mFrequencyStrip = mUiLoader->findObject<RadioFrequencyStrip>( DOCML::MV_NAME_FREQUENCY_STRIP );
    mFrequencyStrip->init( mUiEngine.data(), *mUiLoader );

    RadioStationModel* stationModel = &mUiEngine->stationModel();

    Radio::connect( mFrequencyStrip,            SIGNAL(frequencyChanged(uint,int,int)),
                    this,                       SLOT(setFrequencyFromWidget(uint,int,int)) );
    Radio::connect( mCarousel,                  SIGNAL(frequencyChanged(uint,int,int)),
                    this,                       SLOT(setFrequencyFromWidget(uint,int,int)) );
    Radio::connect( mUiEngine.data(),           SIGNAL(tunedToFrequency(uint,int)),
                    this,                       SLOT(setFrequencyFromEngine(uint,int)) );
    Radio::connect( mFrequencyStrip,            SIGNAL(manualSeekChanged(bool)),
                    this,                       SLOT(setManualSeekMode(bool)) );

    Radio::connect( mFrequencyStrip,            SIGNAL(skipRequested(int)),
                    this,                       SLOT(skip(int)) );
    Radio::connect( mCarousel,                  SIGNAL(skipRequested(int)),
                    this,                       SLOT(skip(int)) );
    Radio::connect( mFrequencyStrip,            SIGNAL(seekRequested(int)),
                    mUiEngine.data(),           SLOT(seekStation(int)) );

    Radio::connect( mUiEngine.data(),           SIGNAL(seekingStarted(int)),
                    this,                       SLOT(seekingStarted()) );
    Radio::connect( mUiEngine.data(),           SIGNAL(antennaStatusChanged(bool)),
                    this,                       SLOT(updateAntennaStatus(bool)) );
    Radio::connect( mUiEngine.data(),           SIGNAL(audioRouteChanged(bool)),
                    this,                       SLOT(updateAudioRoute(bool)) );

    Radio::connect( stationModel,               SIGNAL(favoriteChanged(RadioStation)),
                    this,                       SLOT(handleFavoriteChange(RadioStation)) );

    connectXmlElement( DOCML::MV_NAME_STATIONS_BUTTON,  SIGNAL(clicked()),
                       mMainWindow,                     SLOT(activateStationsView()) );

    connectXmlElement( DOCML::MV_NAME_SCAN_BUTTON,      SIGNAL(clicked()),
                       this,                            SLOT(toggleScanning()) );

    connectXmlElement( DOCML::MV_NAME_SPEAKER_BUTTON,   SIGNAL(clicked()),
                       mUiEngine.data(),                SLOT(toggleAudioRoute()) );

    connectCommonMenuItem( MenuItem::Exit );

    // "Play history" menu item
    connectViewChangeMenuItem( DOCML::MV_NAME_HISTORYVIEW_ACTION, SLOT(activateHistoryView()) );

    //TODO: REMOVE. THIS IS TEMPORARY TEST CODE
    toggleSkippingMode();
    menu()->addAction( "Reset start count", this, SLOT(resetFirstTimeCount()) );
    // END TEMPORARY TEST CODE

    updateAudioRoute( mUiEngine->isUsingLoudspeaker() );

    // Add "back" navigation action to put the application to background
    HbAction* backAction = new HbAction( Hb::BackNaviAction, this );
#ifdef BUILD_WIN32
    Radio::connect( backAction,     SIGNAL(triggered()),
                    this,           SLOT(quit()) );
#else
    Radio::connect( backAction,     SIGNAL(triggered()),
                    mMainWindow,    SLOT(lower()) );
#endif // BUILD_WIN32
    setNavigationAction( backAction );

    const bool firsTimeStart = mUiEngine->isFirstTimeStart();
    const int rowCount = mUiEngine->stationModel().rowCount();
    if ( firsTimeStart && rowCount == 0 ){
        QTimer::singleShot( 100, this, SLOT(toggleScanning()) );
    }

    emit applicationReady();
}

/*!
 * \reimp
 *
 */
void RadioMainView::setOrientation()
{
    loadSection( DOCML::FILE_MAINVIEW, mMainWindow->orientationSection() );
    if ( mCarousel && mFrequencyStrip ) {
        mCarousel->setFrequency( mFrequencyStrip->frequency(), TuneReason::Unspecified );
    }
}

/*!
 * \reimp
 *
 */
void RadioMainView::userAccepted()
{
    mFrequencyScanner.reset( new RadioFrequencyScanner( *mUiEngine, this ) );
    mFrequencyScanner->startScanning();
}

/*!
 * \reimp
 *
 */
bool RadioMainView::eventFilter( QObject* watched, QEvent* event )
{
    if ( event->type() == QEvent::ApplicationDeactivate ) {
        mFrequencyStrip->cancelManualSeek();
    }

    return RadioViewBase::eventFilter( watched, event );
}

/*!
 * Private slot
 */
void RadioMainView::setFrequencyFromWidget( uint frequency, int reason, int direction )
{
//    LOG_FORMAT( "RadioMainView::setFrequencyFromWidget: %u, reason = %d", frequency, reason );
    if ( !RadioUtil::isScannerAlive() ) {
        if ( reason == TuneReason::FrequencyStrip ) {
//            mCarousel->setFrequency( frequency, reason, direction );
//            mUiEngine->tuneWithDelay( frequency, reason );
        } else if ( reason == TuneReason::ManualSeekUpdate ) {
            mCarousel->setFrequency( frequency, reason, RadioUtil::scrollDirection( direction ) );
        } else if ( reason == TuneReason::ManualSeekTune ) {
            LOG_FORMAT( "--------Manual seek tune: %u", frequency );
            mUiEngine->setFrequency( frequency, reason );
        } else if ( reason == TuneReason::StationCarousel ) {
            mFrequencyStrip->setFrequency( frequency, reason, RadioUtil::scrollDirection( direction ) );
            mUiEngine->setFrequency( frequency, reason );
        }
    }
}

/*!
 * Private slot
 */
void RadioMainView::setFrequencyFromEngine( uint frequency, int reason )
{
    LOG_FORMAT( "RadioMainView::setFrequencyFromEngine reason: %d", reason );
    if ( !RadioUtil::isScannerAlive() && !mFrequencyStrip->isInManualSeekMode() ) {
        mCarousel->clearInfoText();
        if ( reason != TuneReason::FrequencyStrip &&
             reason != TuneReason::StationCarousel &&
             reason != TuneReason::Skip ) {
            mCarousel->setFrequency( frequency, reason, Scroll::Shortest );
            mFrequencyStrip->setFrequency( frequency, reason, Scroll::Shortest );
        }
    }
}

/*!
 * Private slot
 */
void RadioMainView::skip( int skipMode )
{
//    if ( !mAlternateSkipping && ( skipMode == StationSkip::PreviousFavorite || skipMode == StationSkip::NextFavorite ) &&
//        mUiEngine->stationModel().favoriteCount() == 0 ) {
//        mCarousel->setInfoText( CarouselInfoText::NoFavorites );
//    } else {
        const uint currentFrequency = mFrequencyStrip->frequency();
        RadioStation station;
        mUiEngine->stationModel().findFrequency( currentFrequency, station );

        if ( mAlternateSkipping ) { //TODO: Remove. Temporary test code
            if ( sender() == mFrequencyStrip ) {
                if ( skipMode == StationSkip::NextFavorite ) {
                    skipMode = StationSkip::Next;
                } else if ( skipMode == StationSkip::PreviousFavorite ) {
                    skipMode = StationSkip::Previous;
                }
            } else if ( sender() == mCarousel ) {
                if ( skipMode == StationSkip::Next ) {
                    skipMode = StationSkip::NextFavorite;
                } else if ( skipMode == StationSkip::Previous ) {
                    skipMode = StationSkip::PreviousFavorite;
                }
            }
        }

        const uint frequency = mUiEngine->skipStation( static_cast<StationSkip::Mode>( skipMode ),
                                                                currentFrequency);

        if ( currentFrequency != frequency || station.isFavorite() ) {
            const Scroll::Direction direction = RadioUtil::scrollDirectionFromSkipMode( skipMode );
            mCarousel->setFrequency( frequency, TuneReason::Skip, direction );
            mFrequencyStrip->setFrequency( frequency, TuneReason::Skip, direction );
        }
//    }
}

/*!
 * Private slot
 */
void RadioMainView::openStationsView()
{
    mFrequencyStrip->cancelManualSeek();
    mMainWindow->activateStationsView();
}

/*!
 * Private slot
 */
void RadioMainView::toggleScanning()
{
    mFrequencyStrip->cancelManualSeek();
    if ( mFrequencyScanner ) {
        mFrequencyScanner->cancelScanning();
    } else {
        const int rowCount =  mUiEngine->stationModel().rowCount();
        if ( rowCount > 0 ) {
            askQuestion( hbTrId( "txt_rad_info_all_stations_in_stations_list_will_be" ) );
        } else {
            userAccepted();
        }
    }
}

/*!
 * Private slot
 */
void RadioMainView::seekingStarted()
{
    if ( !RadioUtil::isScannerAlive() ) {
        mCarousel->setInfoText( CarouselInfoText::Seeking );
    }
}

/*!
 * Private slot
 */
void RadioMainView::updateAntennaStatus( bool connected )
{
    if ( !connected ) {
        mFrequencyStrip->cancelManualSeek();
    }

    HbPushButton* scanButton = mUiLoader->findWidget<HbPushButton>( DOCML::MV_NAME_SCAN_BUTTON );
    scanButton->setEnabled( connected );
    mCarousel->updateAntennaStatus( connected );
}

/*!
 * Private slot
 */
void RadioMainView::updateAudioRoute( bool loudspeaker )
{
    HbPushButton* loudspeakerButton = mUiLoader->findWidget<HbPushButton>( DOCML::MV_NAME_SPEAKER_BUTTON );
    if ( loudspeaker ) {
        loudspeakerButton->setIcon( HbIcon( "qtg_mono_speaker_off" ) );
        loudspeakerButton->setText( hbTrId( "txt_rad_button_deactivate_loudspeaker" ) );
    } else {
        loudspeakerButton->setIcon( HbIcon( "qtg_mono_speaker" ) );
        loudspeakerButton->setText( hbTrId( "txt_rad_button_activate_loudspeaker" ) );
    }
}

/*!
 * Private slot
 */
void RadioMainView::setManualSeekMode( bool manualSeekActive )
{
    if ( manualSeekActive ) {
        qApp->installEventFilter( this );
    } else {
        qApp->removeEventFilter( this );
    }

    mUiEngine->setManualSeekMode( manualSeekActive );

    mCarousel->setManualSeekMode( manualSeekActive );
}

/*!
 * Private slot
 */
void RadioMainView::handleFavoriteChange( const RadioStation& station )
{
    mFrequencyStrip->updateFavorite( station );
    if ( station.isFavorite() ) {
        RadioUtil::showDiscreetNote( "Station added to Favourites." );
    } else {
        RadioUtil::showDiscreetNote( "Station removed from Favourites." );
    }
}

/*!
 * Private slot
 */
void RadioMainView::toggleSkippingMode()
{
    if ( !mSkippingAction ) {
        mSkippingAction = menu()->addAction( "", this, SLOT(toggleSkippingMode()) );
    }

    mAlternateSkipping = !mAlternateSkipping;
    mCarousel->setAlternateSkippingMode( mAlternateSkipping );
    if ( mAlternateSkipping ) {
        mSkippingAction->setText( "Normal skipping mode" );
    } else {
        mSkippingAction->setText( "Alternate skipping mode" );
    }
}

/*!
 * Private slot
 */
void RadioMainView::resetFirstTimeCount()
{
    mUiEngine->setFirstTimeStartPerformed( false );
}
