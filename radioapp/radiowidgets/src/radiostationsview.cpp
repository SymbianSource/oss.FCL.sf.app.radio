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
#include <HbListView>
#include <HbAbstractViewItem>
#include <HbPushButton>
#include <HbEffect>
#include <HbAction>

// User includes
#include "radiostationsview.h"
#include "radiologger.h"
#include "radiomainwindow.h"
#include "radiolocalization.h"
#include "radiouiengine.h"
#include "radiobannerlabel.h"
#include "radiofrequencyscanner.h"
#include "radiocontextmenu.h"
#include "radioxmluiloader.h"
#include "radiostationmodel.h"
#include "radiostationfiltermodel.h"

const char* SECTION_SHOW_ALL_STATIONS = "show_all_stations";
const char* SECTION_SHOW_FAVORITES = "show_favorites";
const char* SECTION_SHOW_SCAN_TEXT = "show_scan_text";
const char* SECTION_HIDE_SCAN_TEXT = "hide_scan_text";

/*!
 *
 */
RadioStationsView::RadioStationsView( RadioXmlUiLoader* uiLoader ) :
    RadioViewBase( uiLoader, false ),
    mStationsList( 0 ),
    mHeadingBanner( 0 ),
    mFavoritesButton( 0 ),
    mLocalStationsButton( 0 )
{
}

/*!
 *
 */
void RadioStationsView::setNowPlayingIcon( const HbIcon& nowPlayingIcon )
{
    mNowPlayingIcon = nowPlayingIcon;
}

/*!
 *
 */
HbIcon RadioStationsView::nowPlayingIcon() const
{
    return mNowPlayingIcon;
}

/*!
 *
 */
void RadioStationsView::setFavoriteIcon( const HbIcon& favoriteIcon )
{
    mFavoriteIcon = favoriteIcon;
}

/*!
 *
 */
HbIcon RadioStationsView::favoriteIcon() const
{
    return mFavoriteIcon;
}

/*!
 * Private slot
 *
 */
void RadioStationsView::listItemClicked( const QModelIndex& index )
{
    LOG_TIMESTAMP( "Channel change started" );
    QModelIndex sourceIndex = mFilterModel->mapToSource( index );
    RadioStation station = mModel->stationAt( sourceIndex.row() );
    RADIO_ASSERT( station.isValid(), "FMRadio", "invalid RadioStation");
    mMainWindow->uiEngine().tunePreset( station.presetIndex() );
}

/*!
 * Private slot
 *
 */
void RadioStationsView::listItemLongPressed( HbAbstractViewItem* item, const QPointF& coords )
{
    QModelIndex sourceIndex = mFilterModel->mapToSource( item->modelIndex() );
    RadioStation station = mModel->stationAt( sourceIndex.row() );
    RADIO_ASSERT( station.isValid() , "FMRadio", "invalid RadioStation");

    //TODO: Remove this. This is a temporary workaround for an Orbit bug in HbMenu
//    RadioContextMenu* menu = new RadioContextMenu( mMainWindow->uiEngine() );
//    menu->init( station, QPointF( size().width() / 2, coords.y() ) );

    RadioContextMenu* menu = mUiLoader->findObject<RadioContextMenu>( DOCML_NAME_CONTEXT_MENU );
    menu->init( station, *mUiLoader );
    menu->setPreferredPos( QPointF( size().width() / 2 - menu->size().width() / 2, coords.y() - menu->size().height() / 2 ) );
    menu->show();
}

/*!
 * Private slot
 *
 */
void RadioStationsView::updateHeadsetStatus( bool connected )
{
    Q_UNUSED( connected );
    updateControlVisibilities();
}

/*!
 * Private slot
 *
 */
void RadioStationsView::updateCurrentStation()
{
    mFilterModel->invalidate();
}

/*!
 * Private slot
 *
 */

void RadioStationsView::deckButtonPressed()
{    
    bool ok = false;
    if ( sender() == mFavoritesButton ) {
        mUiLoader->load( DOCML_STATIONSVIEW_FILE, SECTION_SHOW_FAVORITES, &ok );
    } else {
        mUiLoader->load( DOCML_STATIONSVIEW_FILE, SECTION_SHOW_ALL_STATIONS, &ok );
    }

    const bool showFavorites = mFavoritesButton->isChecked();
    mFilterModel->setTypeFilter( showFavorites ? RadioStation::Favorite
                                               : RadioStation::LocalStation );

    updateControlVisibilities();
}

/*!
 * Private slot
 *
 */
void RadioStationsView::startScanAndSavePresets()
{
    RadioFrequencyScanner* scanner = new RadioFrequencyScanner( mMainWindow->uiEngine(), this );
    scanner->startScanning();
    connectAndTest( scanner,    SIGNAL(frequencyScannerFinished()),
                    this,       SLOT(updateControlVisibilities()) );
}

/*!
 * Private slot
 *
 */
void RadioStationsView::updateControlVisibilities()
{
    LOG_SLOT_CALLER;
    const bool listEmpty = mModel->rowCount() == 0;
    const bool localStationsMode = !mFavoritesButton->isChecked();

    mScanStationsAction->setVisible( mMainWindow->uiEngine().isAntennaAttached()
                                     && localStationsMode
                                     && !mMainWindow->uiEngine().isScanning() );
    mRemoveAllPresetsAction->setVisible( !listEmpty && localStationsMode );

    bool ok = false;
    mUiLoader->load( DOCML_STATIONSVIEW_FILE, listEmpty ? SECTION_SHOW_SCAN_TEXT : SECTION_HIDE_SCAN_TEXT, &ok );
}

/*!
 * From RadioViewBase
 *
 */
void RadioStationsView::init( RadioMainWindow* aMainWindow, RadioStationModel* aModel )
{
    LOG_METHOD;
    mMainWindow = aMainWindow;
    mModel = aModel;

    RadioUiEngine* engine = &mMainWindow->uiEngine();

    mFilterModel = engine->createNewFilterModel( this );
    mFilterModel->setTypeFilter( RadioStation::LocalStation );

    if ( !mFavoriteIcon.isNull() && !mNowPlayingIcon.isNull() ) {
        mModel->setIcons( mFavoriteIcon.qicon(), mNowPlayingIcon.qicon() );
    }
    mModel->setDetail( RadioStationModel::ShowIcons | RadioStationModel::ShowGenre );

    mStationsList           = mUiLoader->findObject<HbListView>( DOCML_NAME_STATIONSLIST );
    mHeadingBanner          = mUiLoader->findWidget<RadioBannerLabel>( DOCML_NAME_HEADINGBANNER );
    mFavoritesButton        = mUiLoader->findObject<HbAction>( DOCML_NAME_FAVORITESBUTTON );
    mLocalStationsButton    = mUiLoader->findObject<HbAction>( DOCML_NAME_LOCALSBUTTON );

    connectAndTest( engine,                 SIGNAL(headsetStatusChanged(bool)),
                    this,                   SLOT(updateHeadsetStatus(bool)) );
    connectAndTest( mModel,                 SIGNAL(rowsInserted(QModelIndex,int,int)),
                    this,                   SLOT(updateControlVisibilities() ) );
    connectAndTest( mModel,                 SIGNAL(rowsRemoved(QModelIndex,int,int)),
                    this,                   SLOT(updateControlVisibilities()) );
    connectAndTest( mModel,                 SIGNAL(modelReset() ),
                    this,                   SLOT(updateControlVisibilities() ) );
    connectAndTest( mFavoritesButton,       SIGNAL(triggered() ),
                    this,                   SLOT(deckButtonPressed() ) );
    connectAndTest( mLocalStationsButton,   SIGNAL(triggered() ),
                    this,                   SLOT(deckButtonPressed() ) );

    connectAndTest( engine, SIGNAL(tunedToFrequency(uint,int)), this, SLOT(updateCurrentStation()) );

    // "Go to tuning view" menu item
    connectViewChangeMenuItem( DOCML_NAME_TUNINGVIEWACTION, SLOT(activateTuningView()) );

    // "Scan local stations" menu item
    mScanStationsAction = mUiLoader->findObject<HbAction>( DOCML_NAME_SCANSTATIONSACTION );
    
    // "Remove all presets" menu item
    mRemoveAllPresetsAction = mUiLoader->findObject<HbAction>( DOCML_NAME_REMOVESTATIONSACTION );

    connectCommonMenuItem( MenuItem::UseLoudspeaker );

    // Connect the "Remove all presets" menu item
    connectXmlElement( DOCML_NAME_REMOVESTATIONSACTION, SIGNAL(triggered()), mModel, SLOT(removeAll()) );

    initListView();
    
    initBackAction();
}

/*!
 * \reimp
 */
void RadioStationsView::showEvent( QShowEvent* event )
{
    RadioViewBase::showEvent( event );
    mModel->setDetail( RadioStationModel::ShowIcons | RadioStationModel::ShowGenre );
    updateControlVisibilities();
    updateHeading();
}

/*!
 *
 */
void RadioStationsView::initListView()
{
    mStationsList->setScrollingStyle( HbListView::PanOrFlick );
    mStationsList->setModel( mFilterModel );
    mStationsList->setSelectionMode( HbListView::NoSelection );
    mStationsList->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

/*!
 *
 */
void RadioStationsView::updateHeading()
{
//    mHeadingBanner->setPlainText( mFavoritesButton->isChecked() ? TRANSLATE( KHeadingTextFavorites )
//                                                                : TRANSLATE( KHeadingTextLocalStations ) );
}
