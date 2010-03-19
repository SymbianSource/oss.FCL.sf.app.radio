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

// User includes
#include "radiomainwindow.h"
#include "radiotuningview.h"
#include "radiofrequencystrip.h"
#include "radiolocalization.h"
#include "radiouiengine.h"
#include "radiologger.h"
#include "radiostationcarousel.h"
#include "radiostationcontrolwidget.h"
#include "radioxmluiloader.h"
#include "radiostationmodel.h"

/*!
 *
 */
RadioTuningView::RadioTuningView( RadioXmlUiLoader* uiLoader ) :
    RadioViewBase( uiLoader, false )
{
}

/*!
 * From RadioViewBase
 *
 */
void RadioTuningView::init( RadioMainWindow* aMainWindow, RadioStationModel* aModel )
{
    LOG_METHOD;
    mMainWindow = aMainWindow;
    mModel = aModel;

    RadioStationCarousel* carousel = mUiLoader->findObject<RadioStationCarousel>( DOCML_NAME_STATIONCAROUSEL );

    RadioStationControlWidget* stationControlWidget = mUiLoader->findObject<RadioStationControlWidget>( DOCML_NAME_STATIONCONTROLWIDGET );
    stationControlWidget->init( mMainWindow );

    // Note! UI connections are already made in the DocML file. Here we need to connect UI to engine
    RadioUiEngine* engine = &mMainWindow->uiEngine();
    RadioFrequencyStrip* frequencyStrip = mUiLoader->findObject<RadioFrequencyStrip>( DOCML_NAME_FREQUENCYSTRIP );
    RadioStationModel* stationModel = &engine->model();

    connectAndTest( frequencyStrip,     SIGNAL(frequencyChanged(uint,int)),
                    engine,             SLOT(tuneWithDelay(uint,int)) );
    connectAndTest( frequencyStrip,     SIGNAL(frequencyChanged(uint,int)),
                    carousel,           SLOT(setFrequency(uint)) );
    connectAndTest( carousel,           SIGNAL(frequencyChanged(uint,int)),
                    frequencyStrip,     SLOT(setFrequency(uint,int)) );
    connectAndTest( engine,             SIGNAL(tunedToFrequency(uint,int)),
                    frequencyStrip,     SLOT(setFrequency(uint,int)) );
    connectAndTest( stationModel,       SIGNAL(favoriteChanged(RadioStation)),
                    frequencyStrip,     SLOT(favoriteChanged(RadioStation)) );
    connectAndTest( frequencyStrip,     SIGNAL(swipedLeft()),
                    engine,             SLOT(skipNext()) );
    connectAndTest( frequencyStrip,     SIGNAL(swipedRight()),
                    engine,             SLOT(skipPrevious()) );
    connectAndTest( engine,             SIGNAL(seekingStarted(int)),
                    carousel,           SLOT(setSeekingText()) );
    connectAndTest( engine,             SIGNAL(headsetStatusChanged(bool)),
                    carousel,           SLOT(updateHeadsetStatus(bool)) );

    frequencyStrip->connectLeftButton( SIGNAL(clicked()), engine, SLOT(skipPrevious()) );
    frequencyStrip->connectRightButton( SIGNAL(clicked()), engine, SLOT(skipNext()) );

    frequencyStrip->connectLeftButton( SIGNAL(longPress(QPointF)), engine, SLOT(seekUp()) );
    frequencyStrip->connectRightButton( SIGNAL(longPress(QPointF)), engine, SLOT(seekDown()) );

    // "Play log" menu item
    connectViewChangeMenuItem( DOCML_NAME_PLAYLOGVIEWACTION, SLOT(activatePlayLogView()) );

    // "StationsView" menu item
    connectViewChangeMenuItem( DOCML_NAME_STATIONSVIEWACTION, SLOT(activateStationsView()) );

    connectCommonMenuItem( MenuItem::UseLoudspeaker );
}

/*!
 * From RadioViewBase
 *
 */
void RadioTuningView::initSecondarySoftkey()
{
    // Intentionally empty so that the tuning view won't have the back button
}

/*!
 * From RadioViewBase
 *
 */
void RadioTuningView::setOrientation()
{
    bool ok = false;
    QString section = mMainWindow->orientationSection();
    mUiLoader->load( DOCML_TUNINGVIEW_FILE, section, &ok );
//    LOG_ASSERT( ok, LOG_FORMAT( "Failed to set orientation %1 from docml %2", section, *DOCML_TUNINGVIEW_FILE ) );
}

/*!
 * Private slot
 */
void RadioTuningView::toggleFavorite()
{
    bool favorite = false;
    uint frequency = mUiLoader->findObject<RadioFrequencyStrip>( DOCML_NAME_FREQUENCYSTRIP )->frequency( &favorite );
    mModel->setFavoriteByFrequency( frequency, !favorite );
}
