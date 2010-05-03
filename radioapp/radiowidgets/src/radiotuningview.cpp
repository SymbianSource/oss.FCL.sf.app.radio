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
#include <HbMessageBox>

// User includes
#include "radiomainwindow.h"
#include "radiotuningview.h"
#include "radiofrequencystrip.h"
#include "radiouiengine.h"
#include "radiologger.h"
#include "radiostationcarousel.h"
#include "radioxmluiloader.h"
#include "radiostationmodel.h"
#include "radiofrequencyscanner.h"

// Constants

/*!
 *
 */
RadioTuningView::RadioTuningView() :
    RadioViewBase( false ),
    mFrequencyScanner( 0 ),
    mCarousel( 0 )
{
}

/*!
 * From RadioViewBase
 *
 */
void RadioTuningView::setScanningMode( bool scanning )
{
    HbPushButton* scanButton = mUiLoader->findWidget<HbPushButton>( DOCML::TV_NAME_SCAN_BUTTON );
    if ( scanning ) {
        disconnect( scanButton,     SIGNAL(clicked()),
                    this,           SLOT(startScanning()) );
        connectAndTest( scanButton,         SIGNAL(clicked()),
                        mFrequencyScanner,  SLOT(scanAndSavePresetsCancelled()) );
        loadSection( DOCML::FILE_TUNINGVIEW, DOCML::TV_SECTION_SCANNING );
    } else {
        disconnect( scanButton, SIGNAL(clicked()) );
        connectAndTest( scanButton,     SIGNAL(clicked()),
                        this,           SLOT(startScanning()) );
        loadSection( DOCML::FILE_TUNINGVIEW, DOCML::TV_SECTION_NORMAL );
    }
}

/*!
 * From RadioViewBase
 *
 */
void RadioTuningView::init( RadioXmlUiLoader* uiLoader, RadioMainWindow* mainWindow )
{
    LOG_METHOD;
    mUiLoader.reset( uiLoader );
    mMainWindow = mainWindow;

    mCarousel = mUiLoader->findObject<RadioStationCarousel>( DOCML::TV_NAME_STATION_CAROUSEL );
    mCarousel->init( &mMainWindow->uiEngine() );

    // Note! UI connections are already made in the DocML file. Here we need to connect UI to engine
    RadioUiEngine* engine = &mMainWindow->uiEngine();
    RadioFrequencyStrip* frequencyStrip = mUiLoader->findObject<RadioFrequencyStrip>( DOCML::TV_NAME_FREQUENCY_STRIP );
    RadioStationModel* stationModel = &engine->model();

    connectAndTest( frequencyStrip,             SIGNAL(frequencyChanged(uint,int)),
                    engine,                     SLOT(tuneWithDelay(uint,int)) );
    connectAndTest( frequencyStrip,             SIGNAL(frequencyChanged(uint,int)),
                    mCarousel,                  SLOT(setFrequency(uint)) );
    connectAndTest( mCarousel,                  SIGNAL(frequencyChanged(uint,int)),
                    frequencyStrip,             SLOT(setFrequency(uint,int)) );
    connectAndTest( engine,                     SIGNAL(tunedToFrequency(uint,int)),
                    frequencyStrip,             SLOT(setFrequency(uint,int)) );
    connectAndTest( engine,                     SIGNAL(tunedToFrequency(uint,int)),
                    this,                       SLOT(seekingFinished()) );
    connectAndTest( stationModel,               SIGNAL(favoriteChanged(RadioStation)),
                    frequencyStrip,             SLOT(favoriteChanged(RadioStation)) );
    connectAndTest( stationModel,               SIGNAL(stationAdded(RadioStation)),
                    frequencyStrip,             SLOT(stationAdded(RadioStation)) );
    connectAndTest( stationModel,               SIGNAL(stationRemoved(RadioStation)),
                    frequencyStrip,             SLOT(stationRemoved(RadioStation)) );
    connectAndTest( frequencyStrip,             SIGNAL(swipedLeft()),
                    engine,                     SLOT(skipNext()) );
    connectAndTest( frequencyStrip,             SIGNAL(swipedRight()),
                    engine,                     SLOT(skipPrevious()) );
    connectAndTest( engine,                     SIGNAL(seekingStarted(int)),
                    this,                       SLOT(seekingStarted()) );
    connectAndTest( engine,                     SIGNAL(antennaStatusChanged(bool)),
                    mCarousel,                  SLOT(updateAntennaStatus(bool)) );
    connectAndTest( engine,                     SIGNAL(antennaStatusChanged(bool)),
                    this,                       SLOT(updateAntennaStatus(bool)) );
    connectAndTest( engine,                     SIGNAL(audioRouteChanged(bool)),
                    this,                       SLOT(updateAudioRoute(bool)) );

    HbPushButton* stationsButton = mUiLoader->findWidget<HbPushButton>( DOCML::TV_NAME_STATIONS_BUTTON );
    connectAndTest( stationsButton,             SIGNAL(clicked()),
                    mMainWindow,                SLOT(activateStationsView()) );

    HbPushButton* scanButton = mUiLoader->findWidget<HbPushButton>( DOCML::TV_NAME_SCAN_BUTTON );
    connectAndTest( scanButton,                 SIGNAL(clicked()),
                    this,                       SLOT(startScanning()) );

    HbPushButton* loudspeakerButton = mUiLoader->findWidget<HbPushButton>( DOCML::TV_NAME_SPEAKER_BUTTON );
    connectAndTest( loudspeakerButton,          SIGNAL(clicked()),
                    engine,                     SLOT(toggleAudioRoute()) );

    frequencyStrip->connectLeftButton( SIGNAL(clicked()), engine, SLOT(skipPrevious()) );
    frequencyStrip->connectRightButton( SIGNAL(clicked()), engine, SLOT(skipNext()) );
    frequencyStrip->connectLeftButton( SIGNAL(clicked()), this, SLOT(buttonPressed()) );
    frequencyStrip->connectRightButton( SIGNAL(clicked()), this, SLOT(buttonPressed()) );

    frequencyStrip->connectLeftButton( SIGNAL(longPress(QPointF)), engine, SLOT(seekDown()) );
    frequencyStrip->connectRightButton( SIGNAL(longPress(QPointF)), engine, SLOT(seekUp()) );

    // "Play history" menu item
    connectViewChangeMenuItem( DOCML::TV_NAME_HISTORYVIEW_ACTION, SLOT(activateHistoryView()) );

    const bool firsTimeStart = engine->isFirstTimeStart();
    const int rowCount = engine->model().rowCount();

    updateAudioRoute( mMainWindow->uiEngine().isUsingLoudspeaker() );

    if ( firsTimeStart && rowCount == 0 ){
        QTimer::singleShot( 100, this, SLOT(startScanning()) );
    }
}

/*!
 * From RadioViewBase
 *
 */
void RadioTuningView::setOrientation()
{
    loadSection( DOCML::FILE_TUNINGVIEW, mMainWindow->orientationSection() );
}

/*!
 * Private slot
 */
void RadioTuningView::toggleFavorite()
{
    bool favorite = false;
    uint frequency = mUiLoader->findObject<RadioFrequencyStrip>( DOCML::TV_NAME_FREQUENCY_STRIP )->frequency( &favorite );
    mMainWindow->uiEngine().model().setFavoriteByFrequency( frequency, !favorite );
}

/*!
 * Private slot
 */
void RadioTuningView::startScanning()
{
    const int rowCount =  mMainWindow->uiEngine().model().rowCount();
    bool scanAllowed = true;
    if ( rowCount > 0 ) {
        scanAllowed = HbMessageBox::question( hbTrId( "txt_rad_info_all_stations_in_stations_list_will_be" ) );
    }

    if ( scanAllowed ) {
        mFrequencyScanner = new RadioFrequencyScanner( mMainWindow->uiEngine(), this );

        connectAndTest( mFrequencyScanner,  SIGNAL(frequencyScannerFinished() ),
                        this,               SLOT(scanningFinished()) );

        mFrequencyScanner->startScanning( *mUiLoader );
    }
}

/*!
 * Private slot
 */
void RadioTuningView::scanningFinished()
{
    disconnect( mFrequencyScanner,  SIGNAL(frequencyScannerFinished() ),
                this,               SLOT(scanningFinished()) );

    loadSection( DOCML::FILE_TUNINGVIEW, DOCML::TV_SECTION_NORMAL );
    mFrequencyScanner = 0;
}

/*!
 * Private slot
 */
void RadioTuningView::seekingStarted()
{
    mCarousel->cleanRdsData();
    loadSection( DOCML::FILE_TUNINGVIEW, DOCML::TV_SECTION_SEEKING );
    HbLabel* infoText = mUiLoader->findWidget<HbLabel>( DOCML::TV_NAME_INFO_TEXT );
    infoText->setAlignment( Qt::AlignBottom | Qt::AlignHCenter );
}

/*!
 * Private slot
 */
void RadioTuningView::seekingFinished()
{
    if ( !mFrequencyScanner && mMainWindow->uiEngine().isAntennaAttached() ) {
        loadSection( DOCML::FILE_TUNINGVIEW, DOCML::TV_SECTION_NORMAL );
    }
}

/*!
 * Private slot
 */
void RadioTuningView::updateAntennaStatus( bool connected )
{
    if ( !connected ) {
        mCarousel->cleanRdsData();
        loadSection( DOCML::FILE_TUNINGVIEW, DOCML::TV_SECTION_NO_ANTENNA );

        HbLabel* infoText = mUiLoader->findWidget<HbLabel>( DOCML::TV_NAME_INFO_TEXT );
        infoText->setAlignment( Qt::AlignBottom | Qt::AlignHCenter );
        infoText->setTextWrapping( Hb::TextWordWrap );
    } else {
        loadSection( DOCML::FILE_TUNINGVIEW, DOCML::TV_SECTION_NORMAL );
    }
}

/*!
 * Private slot
 */
void RadioTuningView::updateAudioRoute( bool loudspeaker )
{
    HbPushButton* loudspeakerButton = mUiLoader->findWidget<HbPushButton>( DOCML::TV_NAME_SPEAKER_BUTTON );
    if ( loudspeaker ) {
        loudspeakerButton->setIcon( HbIcon( "qtg_mono_speaker_off.svg" ) );
        loudspeakerButton->setText( hbTrId( "txt_rad_button_deactivate_loudspeaker" ) );
    } else {
        loudspeakerButton->setIcon( HbIcon( "qtg_mono_speaker.svg" ) );
        loudspeakerButton->setText( hbTrId( "txt_rad_button_activate_loudspeaker" ) );
    }
}

/*!
 * Private slot
 */
void RadioTuningView::buttonPressed()
{
    if ( mMainWindow->uiEngine().model().favoriteCount() == 0 ) {

        mCarousel->setItemVisibility( false );

        loadSection( DOCML::FILE_TUNINGVIEW, DOCML::TV_SECTION_NO_FAVORITES );
        HbLabel* infoText = mUiLoader->findWidget<HbLabel>( DOCML::TV_NAME_INFO_TEXT );
        infoText->setAlignment( Qt::AlignCenter );
        infoText->setTextWrapping( Hb::TextWordWrap );

        QTimer::singleShot( 6000, this, SLOT(removeInfoText()) );
    }
}

/*!
 * Private slot
 */
void RadioTuningView::removeInfoText()
{
    mCarousel->setItemVisibility( true );
    loadSection( DOCML::FILE_TUNINGVIEW, DOCML::TV_SECTION_NORMAL );
}
