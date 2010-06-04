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

// User includes
#include "radiowindow.h"
#include "radiomainview.h"
#include "radiofrequencystrip.h"
#include "radiouiengine.h"
#include "radiologger.h"
#include "radiostationcarousel.h"
#include "radiouiloader.h"
#include "radiouiutilities.h"
#include "radiostationmodel.h"
#include "radiofrequencyscanner.h"

// Constants

/*!
 *
 */
RadioMainView::RadioMainView() :
    RadioViewBase( false ),
    mCarousel( NULL ),
    mFrequencyStrip( NULL )
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
    }
}

/*!
 * \reimp
 *
 */
void RadioMainView::init()
{
    LOG_METHOD;
    mInitialized = true;
    mCarousel = mUiLoader->findObject<RadioStationCarousel>( DOCML::MV_NAME_STATION_CAROUSEL );
    mCarousel->init( *mUiLoader, &mMainWindow->uiEngine() );

    // Note! UI connections are already made in the DocML file. Here we need to connect UI to engine
    RadioUiEngine* engine = &mMainWindow->uiEngine();
    mFrequencyStrip = mUiLoader->findObject<RadioFrequencyStrip>( DOCML::MV_NAME_FREQUENCY_STRIP );
    mFrequencyStrip->init( engine );

    RadioStationModel* stationModel = &engine->stationModel();

    connectAndTest( mFrequencyStrip,            SIGNAL(frequencyChanged(uint,int)),
                    this,                       SLOT(setFrequencyFromWidget(uint,int)) );
    connectAndTest( mCarousel,                  SIGNAL(frequencyChanged(uint,int)),
                    this,                       SLOT(setFrequencyFromWidget(uint,int)) );
    connectAndTest( engine,                     SIGNAL(tunedToFrequency(uint,int)),
                    this,                       SLOT(setFrequencyFromEngine(uint,int)) );

    connectAndTest( mFrequencyStrip,            SIGNAL(skipRequested(int)),
                    this,                       SLOT(skip(int)) );
    connectAndTest( mFrequencyStrip,            SIGNAL(seekRequested(int)),
                    engine,                     SLOT(seekStation(int)) );

    connectAndTest( stationModel,               SIGNAL(favoriteChanged(RadioStation)),
                    mFrequencyStrip,            SLOT(updateFavorite(RadioStation)) );

    connectAndTest( engine,                     SIGNAL(seekingStarted(int)),
                    this,                       SLOT(seekingStarted()) );
    connectAndTest( engine,                     SIGNAL(antennaStatusChanged(bool)),
                    mCarousel,                  SLOT(updateAntennaStatus(bool)) );
    connectAndTest( engine,                     SIGNAL(audioRouteChanged(bool)),
                    this,                       SLOT(updateAudioRoute(bool)) );

    HbPushButton* stationsButton = mUiLoader->findWidget<HbPushButton>( DOCML::MV_NAME_STATIONS_BUTTON );
    connectAndTest( stationsButton,             SIGNAL(clicked()),
                    mMainWindow,                SLOT(activateStationsView()) );

    HbPushButton* scanButton = mUiLoader->findWidget<HbPushButton>( DOCML::MV_NAME_SCAN_BUTTON );
    connectAndTest( scanButton,                 SIGNAL(clicked()),
                    this,                       SLOT(toggleScanning()) );

    HbPushButton* loudspeakerButton = mUiLoader->findWidget<HbPushButton>( DOCML::MV_NAME_SPEAKER_BUTTON );
    connectAndTest( loudspeakerButton,          SIGNAL(clicked()),
                    engine,                     SLOT(toggleAudioRoute()) );

    // "Play history" menu item
    connectViewChangeMenuItem( DOCML::MV_NAME_HISTORYVIEW_ACTION, SLOT(activateHistoryView()) );

    updateAudioRoute( mMainWindow->uiEngine().isUsingLoudspeaker() );

    // Add "back" navigation action to put the application to background
    HbAction* backAction = new HbAction( Hb::BackNaviAction, this );
    connectAndTest( backAction,     SIGNAL(triggered()),
                    mMainWindow,    SLOT(lower()) );
    setNavigationAction( backAction );

    const bool firsTimeStart = engine->isFirstTimeStart();
    const int rowCount = engine->stationModel().rowCount();
    if ( firsTimeStart && rowCount == 0 ){
        QTimer::singleShot( 100, this, SLOT(toggleScanning()) );
    }
}

/*!
 * \reimp
 *
 */
void RadioMainView::setOrientation()
{
    loadSection( DOCML::FILE_MAINVIEW, mMainWindow->orientationSection() );
}

/*!
 * \reimp
 *
 */
void RadioMainView::userAccepted()
{
    mFrequencyScanner.reset( new RadioFrequencyScanner( mMainWindow->uiEngine(), this ) );
    mFrequencyScanner->startScanning();
}

/*!
 * Private slot
 */
void RadioMainView::setFrequencyFromWidget( uint frequency, int reason )
{
    LOG_FORMAT( "RadioMainView::setFrequencyFromWidget: %u, reason = %d", frequency, reason );
    if ( !RadioUiUtilities::isScannerAlive() ) {
        if ( reason == TuneReason::FrequencyStrip ) {
            mCarousel->setFrequency( frequency, reason );
            mMainWindow->uiEngine().tuneWithDelay( frequency, reason );
        } else if ( reason == TuneReason::StationCarousel ) {
            mFrequencyStrip->setFrequency( frequency, reason );
            mMainWindow->uiEngine().tuneFrequency( frequency, reason );
        }
    }
}

/*!
 * Private slot
 */
void RadioMainView::setFrequencyFromEngine( uint frequency, int reason )
{
    if ( !RadioUiUtilities::isScannerAlive() ) {
        mCarousel->clearInfoText();
        if ( reason != TuneReason::FrequencyStrip &&
             reason != TuneReason::StationCarousel &&
             reason != TuneReason::Skip ) {
            mCarousel->setFrequency( frequency, reason );
            mFrequencyStrip->setFrequency( frequency, reason );
        }
    }
}

/*!
 * Private slot
 */
void RadioMainView::skip( int skipMode )
{
    if ( ( skipMode == StationSkip::PreviousFavorite || skipMode == StationSkip::NextFavorite ) &&
        mMainWindow->uiEngine().stationModel().favoriteCount() == 0 ) {
        mCarousel->setInfoText( CarouselInfoText::NoFavorites );
    } else {
        const uint currentFrequency = mFrequencyStrip->frequency();
        const uint frequency = mMainWindow->uiEngine().skipStation( static_cast<StationSkip::Mode>( skipMode ),
                                                                    currentFrequency);
        mCarousel->setFrequency( frequency, TuneReason::Skip );
        mFrequencyStrip->setFrequency( frequency, TuneReason::Skip );
    }
}

/*!
 * Private slot
 */
void RadioMainView::toggleScanning()
{
    if ( mFrequencyScanner ) {
        mFrequencyScanner->cancelScanning();
    } else {
        const int rowCount =  mMainWindow->uiEngine().stationModel().rowCount();
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
    if ( !RadioUiUtilities::isScannerAlive() ) {
        mCarousel->setInfoText( CarouselInfoText::Seeking );
    }
}

/*!
 * Private slot
 */
void RadioMainView::updateAudioRoute( bool loudspeaker )
{
    HbPushButton* loudspeakerButton = mUiLoader->findWidget<HbPushButton>( DOCML::MV_NAME_SPEAKER_BUTTON );
    if ( loudspeaker ) {
        loudspeakerButton->setIcon( HbIcon( "qtg_mono_speaker_off.svg" ) );
        loudspeakerButton->setText( hbTrId( "txt_rad_button_deactivate_loudspeaker" ) );
    } else {
        loudspeakerButton->setIcon( HbIcon( "qtg_mono_speaker.svg" ) );
        loudspeakerButton->setText( hbTrId( "txt_rad_button_activate_loudspeaker" ) );
    }
}
