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
#include <QSortFilterProxyModel>

// User includes
#include "radiostationsview.h"
#include "radiologger.h"
#include "radiowindow.h"
#include "radiouiengine.h"
#include "radiofrequencyscanner.h"
#include "radiouiloader.h"
#include "radiostationmodel.h"
#include "radiostation.h"

// Constants
const char* REGEX_SHOW_FAVORITES = "true";
const char* REGEX_SHOW_ALL = ".+";

/*!
 *
 */
RadioStationsView::RadioStationsView() :
    RadioViewBase( false ),
    mModel( 0 ),
    mScanStationsAction( 0 ),
    mClearListAction( 0 ),
    mStationsList( 0 ),
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
void RadioStationsView::handleClick( const QModelIndex& index )
{
    LOG_TIMESTAMP( "Channel change started" );
    QModelIndex sourceIndex = mFilterModel->mapToSource( index );
    *mSelectedStation = mModel->stationAt( sourceIndex.row() );
    mUiEngine->setFrequency( mSelectedStation->frequency(), TuneReason::StationsList );
}

/*!
 * Private slot
 *
 */
void RadioStationsView::handleLongPress( HbAbstractViewItem* item, const QPointF& coords )
{
    Q_UNUSED( item );

    HbMenu* menu = mUiLoader->findObject<HbMenu>( DOCML::NAME_CONTEXT_MENU );

    *mSelectedStation = mFilterModel->data( item->modelIndex(), RadioRole::RadioStationRole ).value<RadioStation>();

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
    updateVisibilities();
}

/*!
 * Private slot
 *
 */
void RadioStationsView::updateViewMode()
{
    QString section = DOCML::SV_SECTION_SHOW_ALL_STATIONS;
    QString filter = REGEX_SHOW_ALL;

    if ( sender() == mFavoritesButton ) {
        section = DOCML::SV_SECTION_SHOW_FAVORITES;
        filter = REGEX_SHOW_FAVORITES;
    }

    loadSection( DOCML::FILE_STATIONSVIEW, section );
    mFilterModel->setFilterRegExp( filter );

    updateVisibilities();
}

/*!
 * Private slot
 *
 */
void RadioStationsView::startScanning()
{
    const int rowCount =  mUiEngine->stationModel().rowCount();
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
    updateVisibilities();
    mFrequencyScanner.take();
}

/*!
 * Private slot
 *
 */
void RadioStationsView::updateVisibilities()
{
    LOG_SLOT_CALLER;
    bool listEmpty = mModel->rowCount() == 0;
    const bool localStationsMode = !mFavoritesButton->isChecked();

    if ( !localStationsMode ) {
        listEmpty = mModel->favoriteCount() == 0;
    }

    mClearListAction->setEnabled( !listEmpty );

    const bool scanAvailable = mUiEngine->isAntennaAttached() && localStationsMode;
    mScanStationsAction->setEnabled( scanAvailable );
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
    nameQuery->setDismissPolicy( HbDialog::TapOutside );
    nameQuery->setPromptText( hbTrId( "txt_rad_dialog_new_name" ) );
    nameQuery->setInputMode( HbInputDialog::TextInput );
    nameQuery->setValue( mSelectedStation->name() );
    nameQuery->setObjectName( DOCML::NAME_INPUT_QUERY );
    nameQuery->open( this, SLOT(renameDone(HbAction*)) );
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

    if( action ) {
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
    mModel = &mUiEngine->stationModel();

    mFilterModel = new QSortFilterProxyModel( this );
    mFilterModel->setDynamicSortFilter( true );
    mFilterModel->setFilterRole( RadioRole::IsFavoriteRole );
    mFilterModel->setSourceModel( &mUiEngine->stationModel() );

    loadSection( DOCML::FILE_STATIONSVIEW, DOCML::SV_SECTION_SHOW_ALL_STATIONS );

    if ( !mFavoriteIcon.isNull() && !mNowPlayingIcon.isNull() ) {
        mModel->setIcons( mFavoriteIcon.qicon(), mNowPlayingIcon.qicon() );
    }
    mModel->setDetail( RadioStationModel::ShowIcons | RadioStationModel::ShowGenre );

    mStationsList           = mUiLoader->findObject<HbListView>( DOCML::SV_NAME_STATIONS_LIST );
    mFavoritesButton        = mUiLoader->findObject<HbAction>( DOCML::SV_NAME_FAVORITES_BUTTON );
    mLocalStationsButton    = mUiLoader->findObject<HbAction>( DOCML::SV_NAME_LOCALS_BUTTON );

    Radio::connect( mUiEngine.data(),       SIGNAL(antennaStatusChanged(bool)),
                    this,                   SLOT(updateAntennaStatus(bool)) );
    Radio::connect( mModel,                 SIGNAL(rowsInserted(QModelIndex,int,int)),
                    this,                   SLOT(updateVisibilities()) );
    Radio::connect( mModel,                 SIGNAL(rowsRemoved(QModelIndex,int,int)),
                    this,                   SLOT(updateVisibilities()) );
    Radio::connect( mModel,                 SIGNAL(modelReset()),
                    this,                   SLOT(updateVisibilities()) );
    Radio::connect( mFavoritesButton,       SIGNAL(triggered()),
                    this,                   SLOT(updateViewMode()) );
    Radio::connect( mLocalStationsButton,   SIGNAL(triggered() ),
                    this,                   SLOT(updateViewMode()) );
    Radio::connect( mStationsList,          SIGNAL(activated(QModelIndex)),
                    this,                   SLOT(handleClick(QModelIndex)) );
    Radio::connect( mStationsList,          SIGNAL(longPressed(HbAbstractViewItem*,QPointF)),
                    this,                   SLOT(handleLongPress(HbAbstractViewItem*,QPointF)) );

    connectCommonMenuItem( MenuItem::Exit );
    connectXmlElement( DOCML::SV_NAME_SCAN_BUTTON,      SIGNAL(clicked()),
                       this,                            SLOT(startScanning()) );

    // Context menu actions
    connectXmlElement( DOCML::SV_NAME_RENAME_ACTION,    SIGNAL(triggered()),
                       this,                            SLOT(rename()) );
    connectXmlElement( DOCML::SV_NAME_FAVORITE_ACTION,  SIGNAL(triggered()),
                       this,                            SLOT(toggleFavorite()) );
    connectXmlElement( DOCML::SV_NAME_DELETE_ACTION,    SIGNAL(triggered()),
                       this,                            SLOT(deleteStation()) );

    // "Scan local stations" menu item
    mScanStationsAction = mUiLoader->findObject<HbAction>( DOCML::SV_NAME_SCAN_ACTION );
    Radio::connect( mScanStationsAction,   SIGNAL(triggered() ),
                    this,                   SLOT(startScanning() ) );

    // "Remove all presets" menu item
    mClearListAction = mUiLoader->findObject<HbAction>( DOCML::SV_NAME_CLEAR_LIST_ACTION );
    Radio::connect( mClearListAction,   SIGNAL(triggered() ),
                    this,               SLOT(clearList() ) );

    connectCommonMenuItem( MenuItem::UseLoudspeaker );

    initListView();

    initBackAction();

    updateViewMode();
}

/*!
 * \reimp
 */
void RadioStationsView::userAccepted()
{
    if ( mCurrentQuestion == StartScanning ) {
        mFrequencyScanner.reset( new RadioFrequencyScanner( *mUiEngine, this ) );

        Radio::connect( mFrequencyScanner.data(),   SIGNAL(frequencyScannerFinished()),
                        this,                       SLOT(finishScanning()) );

        mFrequencyScanner->startScanning();
    } else if ( mCurrentQuestion == ClearList ){
        const bool favoriteMode = mFavoritesButton->isChecked();
        mModel->removeAll( favoriteMode ? RadioStationModel::RemoveFavorites : RadioStationModel::RemoveAll );
        updateVisibilities();
    } else if ( mCurrentQuestion == DeleteStation ) {
        mModel->removeStation( mModel->currentStation() );
    }

    mCurrentQuestion = NoQuestion;
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
