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
#include <HbInputDialog>
#include <HbMenu>

// User includes
#include "radiostationsview.h"
#include "radiologger.h"
#include "radiowindow.h"
#include "radiouiengine.h"
#include "radiobannerlabel.h"
#include "radiofrequencyscanner.h"
#include "radiouiloader.h"
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
    mSelectedStation( new RadioStation ),
    mCurrentQuestion( NoQuestion )
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
    mMainWindow->uiEngine().tuneFrequency( mSelectedStation->frequency(), TuneReason::StationsList );
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
    const bool showFavorites = mFavoritesButton->isChecked();
    if ( showFavorites ) {
        loadSection( DOCML::FILE_STATIONSVIEW, DOCML::SV_SECTION_SHOW_FAVORITES );
    } else {
        loadSection( DOCML::FILE_STATIONSVIEW, DOCML::SV_SECTION_SHOW_ALL_STATIONS );
    }

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
    const int rowCount =  mMainWindow->uiEngine().stationModel().rowCount();
    mCurrentQuestion = StartScanning;
    if ( rowCount > 0 ) {
        askQuestion( hbTrId( "txt_rad_info_all_stations_in_stations_list_will_be" ) );
    } else {
        userAccepted();
    }
}

/*!
 * Private slot
 *
 */
void RadioStationsView::finishScanning()
{
    updateControlVisibilities();
    mFrequencyScanner.take();
}

/*!
 * Private slot
 *
 */
void RadioStationsView::updateControlVisibilities()
{
    LOG_SLOT_CALLER;
    bool listEmpty = mModel->rowCount() == 0;
    const bool localStationsMode = !mFavoritesButton->isChecked();

    if ( !localStationsMode ) {
        listEmpty = mModel->favoriteCount() == 0;
    }

    mClearListAction->setVisible( !listEmpty );

    const bool scanAvailable = mMainWindow->uiEngine().isAntennaAttached() && localStationsMode;
    mScanStationsAction->setVisible( scanAvailable );
    HbPushButton* scanButton = mUiLoader->findWidget<HbPushButton>( DOCML::SV_NAME_SCAN_BUTTON );
    scanButton->setEnabled( scanAvailable );

    loadSection( DOCML::FILE_STATIONSVIEW, listEmpty ? DOCML::SV_SECTION_SHOW_SCAN_TEXT : DOCML::SV_SECTION_HIDE_SCAN_TEXT );
}

/*!
 * Private slot
 *
 */
void RadioStationsView::clearList()
{
    const bool favoriteMode = mFavoritesButton->isChecked();
    mCurrentQuestion = ClearList;
    askQuestion( hbTrId( favoriteMode ? "txt_rad_info_clear_favourite_stations_list"
                                      : "txt_rad_info_clear_all_stations_list" ) );
}

/*!
 * Private slot
 *
 */
void RadioStationsView::rename()
{
    HbInputDialog* nameQuery = new HbInputDialog();
    nameQuery->setAttribute( Qt::WA_DeleteOnClose, true );
    nameQuery->setDismissPolicy( HbDialog::NoDismiss );
    nameQuery->setPromptText( hbTrId( "txt_rad_dialog_new_name" ) );
    nameQuery->setInputMode( HbInputDialog::TextInput );
    nameQuery->setValue( mSelectedStation->name() );
    nameQuery->setObjectName( DOCML::NAME_INPUT_QUERY );
    nameQuery->open( this, SLOT(renameDone(HbAction* )) );
}

/*!
 * Private slot
 *
 */
void RadioStationsView::toggleFavorite()
{
    mModel->setFavoriteByPreset( mSelectedStation->presetIndex(), !mSelectedStation->isFavorite() );
}

/*!
 * Private slot
 *
 */
void RadioStationsView::deleteStation()
{
    mCurrentQuestion = DeleteStation;
    askQuestion( hbTrId( "txt_rad_menu_delete_station" ) );
}

/*!
 * Private slot
 *
 */
void RadioStationsView::renameDone( HbAction* action )
{
    HbInputDialog* dlg = static_cast<HbInputDialog*>( sender() );

    if( action == dlg->primaryAction() ) {
        mModel->renameStation( mSelectedStation->presetIndex(), dlg->value().toString() );
    }
}

/*!
 * From RadioViewBase
 *
 */
void RadioStationsView::init()
{
    LOG_METHOD;
    mInitialized = true;
    mModel = &mMainWindow->uiEngine().stationModel();

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
                    this,               SLOT(clearList() ) );

    connectCommonMenuItem( MenuItem::UseLoudspeaker );

    initListView();
    
    initBackAction();

    updateControlVisibilities();
}

/*!
 * \reimp
 */
void RadioStationsView::userAccepted()
{
    if ( mCurrentQuestion == StartScanning ) {
        mFrequencyScanner.reset( new RadioFrequencyScanner( mMainWindow->uiEngine(), this ) );

        connectAndTest( mFrequencyScanner.data(),   SIGNAL(frequencyScannerFinished()),
                        this,                       SLOT(finishScanning()) );

        mFrequencyScanner->startScanning();
    } else if ( mCurrentQuestion == ClearList ){
        const bool favoriteMode = mFavoritesButton->isChecked();
        mModel->removeAll( favoriteMode ? RadioStationModel::RemoveFavorites : RadioStationModel::RemoveAll );
        updateControlVisibilities();
    } else if ( mCurrentQuestion == DeleteStation ) {
        mModel->removeStation( mModel->currentStation() );
    }

    mCurrentQuestion = NoQuestion;
}

/*!
 * \reimp
 */
void RadioStationsView::showEvent( QShowEvent* event )
{
    RadioViewBase::showEvent( event );
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
