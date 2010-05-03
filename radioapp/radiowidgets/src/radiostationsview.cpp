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
#include <HbMessageBox>
#include <HbInputDialog>
#include <HbMenu>

// User includes
#include "radiostationsview.h"
#include "radiologger.h"
#include "radiomainwindow.h"
#include "radiouiengine.h"
#include "radiobannerlabel.h"
#include "radiofrequencyscanner.h"
#include "radioxmluiloader.h"
#include "radiostationmodel.h"
#include "radiostation.h"
#include "radiostationfiltermodel.h"

/*!
 *
 */
RadioStationsView::RadioStationsView() :
    RadioViewBase( false ),
    mModel( 0 ),
    mFilterModel( 0 ),
    mScanStationsAction( 0 ),
    mClearListAction( 0 ),
    mStationsList( 0 ),
    mHeadingBanner( 0 ),
    mFavoritesButton( 0 ),
    mLocalStationsButton( 0 ),
    mSelectedStation( new RadioStation )
{
}

/*!
 *
 */
RadioStationsView::~RadioStationsView()
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
    *mSelectedStation = mModel->stationAt( sourceIndex.row() );
    RADIO_ASSERT( station.isValid(), "FMRadio", "invalid RadioStation");
    mMainWindow->uiEngine().tunePreset( mSelectedStation->presetIndex() );
}

/*!
 * Private slot
 *
 */
void RadioStationsView::listItemLongPressed( HbAbstractViewItem* item, const QPointF& coords )
{
    Q_UNUSED( item );

    HbMenu* menu = mUiLoader->findObject<HbMenu>( DOCML::NAME_CONTEXT_MENU );

    *mSelectedStation = mFilterModel->data( item->modelIndex(), RadioStationModel::RadioStationRole ).value<RadioStation>();

    HbAction* favoriteAction = mUiLoader->findObject<HbAction>( DOCML::NAME_CONTEXT_FAVORITE );
    if ( mSelectedStation->isFavorite() ) {
        favoriteAction->setText( hbTrId( "txt_rad_menu_remove_favourite" ) );
    } else {
        favoriteAction->setText( hbTrId( "txt_rad_menu_add_to_favourites" ) );
    }

    menu->setPreferredPos( QPointF( size().width() / 2 - menu->size().width() / 2, coords.y() - menu->size().height() / 2 ) );
    menu->show();
}

/*!
 * Private slot
 *
 */
void RadioStationsView::updateAntennaStatus( bool connected )
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
    if ( sender() == mFavoritesButton ) {
        loadSection( DOCML::FILE_STATIONSVIEW, DOCML::SV_SECTION_SHOW_FAVORITES );
    } else {
        loadSection( DOCML::FILE_STATIONSVIEW, DOCML::SV_SECTION_SHOW_ALL_STATIONS );
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
void RadioStationsView::startScanning()
{
    const int rowCount =  mMainWindow->uiEngine().model().rowCount();
    bool scanAllowed = true;
    if ( rowCount > 0 ) {
        scanAllowed = HbMessageBox::question( hbTrId( "txt_rad_info_all_stations_in_stations_list_will_be" ) );
    }

    if ( scanAllowed ) {
        RadioFrequencyScanner* scanner = new RadioFrequencyScanner( mMainWindow->uiEngine(), this );

        connectAndTest( scanner,    SIGNAL(frequencyScannerFinished()),
                        this,       SLOT(updateControlVisibilities()) );

        scanner->startScanning( *mUiLoader );
    }
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
                                     && localStationsMode );
    mClearListAction->setVisible( !listEmpty && localStationsMode );

    HbPushButton* scanButton = mUiLoader->findWidget<HbPushButton>( DOCML::SV_NAME_SCAN_BUTTON );

    if ( !mMainWindow->uiEngine().isAntennaAttached() ) {
        scanButton->setEnabled( false );
    } else {
        scanButton->setEnabled( true );
    }

    loadSection( DOCML::FILE_STATIONSVIEW, listEmpty ? DOCML::SV_SECTION_SHOW_SCAN_TEXT : DOCML::SV_SECTION_HIDE_SCAN_TEXT );
}

/*!
 * Private slot
 *
 */
void RadioStationsView::rename()
{
    HbInputDialog nameQuery;
    nameQuery.setPromptText( hbTrId( "txt_rad_dialog_new_name" ) );
    nameQuery.setInputMode( HbInputDialog::TextInput );
    nameQuery.setValue( mSelectedStation->name() );
    nameQuery.setObjectName( DOCML::NAME_INPUT_QUERY );

    if ( nameQuery.exec() == nameQuery.primaryAction() ) {
        mModel->renameStation( mSelectedStation->presetIndex(), nameQuery.value().toString() );
    }
}

/*!
 * Private slot
 *
 */
void RadioStationsView::toggleFavorite()
{
    if ( mSelectedStation->isFavorite() ) {
        const bool answer = HbMessageBox::question( hbTrId( "txt_rad_info_remove_station_from_favorites" ) );

        if ( answer ){
            mModel->setFavoriteByPreset( mSelectedStation->presetIndex(), !mSelectedStation->isFavorite() );
        }
    } else {
        mModel->setFavoriteByPreset( mSelectedStation->presetIndex(), !mSelectedStation->isFavorite() );
    }
}

/*!
 * Private slot
 *
 */
void RadioStationsView::deleteStation()
{
    const bool answer = HbMessageBox::question( hbTrId( "txt_rad_menu_delete_station" ) );

    if ( answer ) {
        mModel->removeStation( mModel->currentStation() );
    }
}

/*!
 * From RadioViewBase
 *
 */
void RadioStationsView::init( RadioXmlUiLoader* uiLoader, RadioMainWindow* mainWindow )
{
    LOG_METHOD;
    mUiLoader.reset( uiLoader );
    mMainWindow = mainWindow;
    mModel = &mMainWindow->uiEngine().model();

    RadioUiEngine* engine = &mMainWindow->uiEngine();

    mFilterModel = engine->createNewFilterModel( this );
    mFilterModel->setTypeFilter( RadioStation::LocalStation );

    loadSection( DOCML::FILE_STATIONSVIEW, DOCML::SV_SECTION_SHOW_ALL_STATIONS );

    if ( !mFavoriteIcon.isNull() && !mNowPlayingIcon.isNull() ) {
        mModel->setIcons( mFavoriteIcon.qicon(), mNowPlayingIcon.qicon() );
    }
    mModel->setDetail( RadioStationModel::ShowIcons | RadioStationModel::ShowGenre );

    mStationsList           = mUiLoader->findObject<HbListView>( DOCML::SV_NAME_STATIONS_LIST );
    mHeadingBanner          = mUiLoader->findWidget<RadioBannerLabel>( DOCML::SV_NAME_HEADING_BANNER );
    mFavoritesButton        = mUiLoader->findObject<HbAction>( DOCML::SV_NAME_FAVORITES_BUTTON );
    mLocalStationsButton    = mUiLoader->findObject<HbAction>( DOCML::SV_NAME_LOCALS_BUTTON );

    connectAndTest( engine,                 SIGNAL(antennaStatusChanged(bool)),
                    this,                   SLOT(updateAntennaStatus(bool)) );
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

    // "Scan local stations" menu item
    mScanStationsAction = mUiLoader->findObject<HbAction>( DOCML::SV_NAME_SCAN_ACTION );
    
    // "Remove all presets" menu item
    mClearListAction = mUiLoader->findObject<HbAction>( DOCML::SV_NAME_CLEAR_LIST_ACTION );
    connectAndTest( mClearListAction,   SIGNAL(triggered() ),
                    mModel,             SLOT(removeAll() ) );

    connectCommonMenuItem( MenuItem::UseLoudspeaker );

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
