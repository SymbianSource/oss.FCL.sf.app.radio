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
#include <HbStyleLoader>
#include <HbPushButton>
#include <HbMessageBox>
#include <HbAnchorLayout>

// User includes
#include "radiostationcontrolwidget.h"
#include "radiolocalization.h"
#include "radiomainwindow.h"
#include "radiologger.h"
#include "radiostation.h"
#include "radiouiengine.h"
#include "radiostationmodel.h"
#include "radioplaylogmodel.h"


#ifdef USE_LAYOUT_FROM_E_DRIVE
    const QString KBlinkEffectFile = "e:/radiotest/effects/blink_in_out_in.fxml";
#else
    const QString KBlinkEffectFile = ":/effects/blink_in_out_in.fxml";
#endif
const QString KBlinkEffect = "blink_in_out_in";

static const char* FILE_PATH_WIDGETML = ":/layout/radiostationcontrolwidget.widgetml";
static const char* FILE_PATH_CSS = ":/layout/radiostationcontrolwidget.css";
static const char* TAG_BUTTON = "tag_song_button";
static const char* RECOGNIZE_BUTTON = "recognize_button";
static const char* STATIONS_BUTTON = "stations_button";

/*!
 *
 */
RadioStationControlWidget::RadioStationControlWidget( RadioUiEngine& uiEngine, QGraphicsItem* parent ) :
    HbWidget( parent ),
    mUiEngine( uiEngine ),
    mTagSongButton( new HbPushButton( this ) ),
    mStationsViewButton( new HbPushButton( this ) ),
    mRecognizeButton( new HbPushButton( this ) ),
    mMainWindow( 0 )
{
    bool registered = HbStyleLoader::registerFilePath( FILE_PATH_WIDGETML );
    LOG_FORMAT( "registered: %d", registered );
    registered = HbStyleLoader::registerFilePath( FILE_PATH_CSS );
    LOG_FORMAT( "registered: %d", registered );

    mTagSongButton->setStretched( true );
    mStationsViewButton->setStretched( true );
    mRecognizeButton->setStretched( true );
    
    HbStyle::setItemName( mTagSongButton, TAG_BUTTON );
    mTagSongButton->setObjectName( TAG_BUTTON );
    HbStyle::setItemName( mRecognizeButton, RECOGNIZE_BUTTON );
    mRecognizeButton->setObjectName( RECOGNIZE_BUTTON );
    HbStyle::setItemName( mStationsViewButton, STATIONS_BUTTON );
    mStationsViewButton->setObjectName( STATIONS_BUTTON );
}

/*!
 *
 */
RadioStationControlWidget::~RadioStationControlWidget()
{
    HbStyleLoader::unregisterFilePath( FILE_PATH_WIDGETML );
    HbStyleLoader::unregisterFilePath( FILE_PATH_CSS );
}

/*!
 * Property
 *
 */
void RadioStationControlWidget::setBackground( const HbIcon& background )
{
    mBackground = background;
}

/*!
 * Property
 */
HbIcon RadioStationControlWidget::background() const
{
    return mBackground;
}

/*!
 * Property
 */
void RadioStationControlWidget::setTagBtnText( const QString& text )
{
    mTagSongButton->setText( text );
}

/*!
 * Property
 */
QString RadioStationControlWidget::tagBtnText() const
{
    return mTagSongButton->text();
}

/*!
 * Property
 */
void RadioStationControlWidget::setRecognizeBtnText( const QString& text )
{
    mRecognizeButton->setText( text );
}

/*!
 * Property
 */
QString RadioStationControlWidget::recognizeBtnText() const
{
    return mRecognizeButton->text();
}

/*!
 * Property
 */
void RadioStationControlWidget::setStationsBtnText( const QString& text )
{
    mStationsViewButton->setText( text );
}

/*!
 * Property
 */
QString RadioStationControlWidget::stationsBtnText() const
{
    return mStationsViewButton->text();
}

/*!
 *
 */
void RadioStationControlWidget::init( RadioMainWindow* aMainWindow )
{
    mMainWindow = aMainWindow;

    mTagSongButton->setBackground( HbIcon( " " ) );
    mRecognizeButton->setBackground( HbIcon( " " ) );
    mStationsViewButton->setBackground( HbIcon( " " ) );

    mTagSongButton->setIcon( HbIcon( ":/images/tagsongbuttonicon.png" ) );
    mRecognizeButton->setIcon( HbIcon( ":/images/identifysongbuttonicon.png" ) );
    mStationsViewButton->setIcon( HbIcon( ":/images/stationsbuttonicon.png" ) );

    RadioPlayLogModel* playLogModel = &mUiEngine.playLogModel();
    if ( !playLogModel->isCurrentSongRecognized() ) {
        disableTagButton();
    } else {
        enableTagButton();
    }

    connectAndTest( playLogModel,           SIGNAL(currentSongReset()),
                    this,                   SLOT(disableTagButton()) );
    connectAndTest( playLogModel,           SIGNAL(itemAdded()),
                    this,                   SLOT(enableTagButton()) );

//    if ( RadioUiUtilities::uiEngine().isSongRecognitionAppAvailable() ) {
        connectAndTest( mRecognizeButton,       SIGNAL(clicked()),
                        this,                   SLOT(recognizePressed()) );
//    } else {
//        mRecognizeButton->setOpacity( 0.4 );
//    }

    connectAndTest( mStationsViewButton,    SIGNAL(clicked()),
                    this,                   SLOT(updateStationsButton()) );

    HbEffect::add( mTagSongButton,      KBlinkEffectFile, KBlinkEffect );
    HbEffect::add( mRecognizeButton,    KBlinkEffectFile, KBlinkEffect );
    HbEffect::add( mStationsViewButton, KBlinkEffectFile, KBlinkEffect );
}

/*!
 * Private slot
 *
 */
void RadioStationControlWidget::updateStationsButton()
{
    HbEffect::start( mStationsViewButton, KBlinkEffect, this, "effectFinished" );
}

/*!
 * Private slot
 *
 */
void RadioStationControlWidget::recognizePressed()
{
    LOG_METHOD;
    HbEffect::start( mRecognizeButton, KBlinkEffect );
    mUiEngine.launchSongRecognition();
}

/*!
 * Private slot
 *
 */
void RadioStationControlWidget::effectFinished( HbEffect::EffectStatus status )
{
    Q_UNUSED( status );
    mMainWindow->activateStationsView();
}

/*!
 * Private slot
 *
 */
void RadioStationControlWidget::disableTagButton()
{
    mTagSongButton->setOpacity( 0.5 );
    disconnect( mTagSongButton, SIGNAL(clicked()) );
}

/*!
 * Private slot
 *
 */
void RadioStationControlWidget::enableTagButton()
{
    mTagSongButton->setOpacity( 1 );
    RadioPlayLogModel* playLogModel = &mUiEngine.playLogModel();
    connectAndTest( mTagSongButton, SIGNAL(clicked()),
                    playLogModel,   SLOT(setFavorite()) );
}

/*!
 * \reimp
 *
 */
void RadioStationControlWidget::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    HbWidget::paint( painter, option, widget );
    mBackground.paint( painter, QRectF( QPoint( 0, 0 ), size() ), Qt::IgnoreAspectRatio );
}
