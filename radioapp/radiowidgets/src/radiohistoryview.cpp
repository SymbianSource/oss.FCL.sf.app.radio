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
#include <HbAction>
#include <HbAbstractViewItem>
#include <HbMenu>
#include <HbMessageBox>

// User includes
#include "radiohistoryview.h"
#include "radiomainwindow.h"
#include "radiologger.h"
#include "radioxmluiloader.h"
#include "radiouiengine.h"
#include "radiostationfiltermodel.h"
#include "radiohistorymodel.h"

/*!
 *
 */
RadioHistoryView::RadioHistoryView() :
    RadioViewBase(),
    mHistoryList( 0 ),
    mAllSongsButton( 0 ),
    mTaggedSongsButton( 0 ),
    mFilterModel( 0 )
{
}

/*!
 * Private slot
 *
 */
void RadioHistoryView::deckButtonPressed()
{
    if ( sender() == mTaggedSongsButton ) {
        loadSection( DOCML::FILE_HISTORYVIEW, DOCML::HV_SECTION_FAVORITE_MODE );
    } else {
        loadSection( DOCML::FILE_HISTORYVIEW, DOCML::HV_SECTION_HISTORY_MODE );
    }

    const bool showFavorites = mTaggedSongsButton->isChecked();
//    mFilterModel->setTypeFilter( showFavorites ? RadioStation::Favorite
//                                               : RadioStation::LocalStation );

    updateVisibilities();
}

/*!
 * Private slot
 *
 */
void RadioHistoryView::clearList()
{
    const bool answer = HbMessageBox::question( hbTrId( "txt_rad_info_clear_recently_played_songs_list" ) );

    if ( answer ) {
        mMainWindow->uiEngine().historyModel().removeAll();
        updateVisibilities();
    }
}

/*!
 * Private slot
 *
 */
void RadioHistoryView::updateVisibilities()
{
    const int itemCount = mMainWindow->uiEngine().historyModel().rowCount();
    loadSection( DOCML::FILE_HISTORYVIEW, itemCount ? DOCML::HV_SECTION_SHOW_LIST : DOCML::HV_SECTION_HIDE_LIST );
}

/*!
 * Private slot
 *
 */
void RadioHistoryView::listItemClicked( const QModelIndex& index )
{
    showContextMenu( index );
}

/*!
 * Private slot
 *
 */
void RadioHistoryView::listItemLongPressed( HbAbstractViewItem* item, const QPointF& coords )
{
    Q_UNUSED( coords );
    showContextMenu( item->modelIndex() );
}

/*!
 * \reimp
 *
 */
void RadioHistoryView::init( RadioXmlUiLoader* uiLoader, RadioMainWindow* mainWindow )
{
    LOG_METHOD;
    mUiLoader.reset( uiLoader );
    mMainWindow = mainWindow;

    RadioHistoryModel* historyModel = &mMainWindow->uiEngine().historyModel();
    historyModel->setShowDetails( mOrientation == Qt::Horizontal );

    mHistoryList = mUiLoader->findObject<HbListView>( DOCML::HV_NAME_HISTORY_LIST );
    mHistoryList->setScrollingStyle( HbListView::PanOrFlick );
    mFilterModel = mMainWindow->uiEngine().createNewFilterModel( this );
    mFilterModel->setSourceModel( historyModel );
    mHistoryList->setModel( mFilterModel );
    mHistoryList->setSelectionMode( HbListView::NoSelection );
    mHistoryList->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    mAllSongsButton     = mUiLoader->findObject<HbAction>( DOCML::HV_NAME_ALL_SONGS_BUTTON );
    mTaggedSongsButton    = mUiLoader->findObject<HbAction>( DOCML::HV_NAME_TAGGED_SONGS_BUTTON );

    HbAction* clearListAction = mUiLoader->findObject<HbAction>( DOCML::HV_NAME_CLEAR_LIST_ACTION );
    connectAndTest( clearListAction, SIGNAL(triggered()), this, SLOT(clearList()) );

    connectAndTest( mTaggedSongsButton,     SIGNAL(triggered() ),
                    this,                   SLOT(deckButtonPressed() ) );
    connectAndTest( mAllSongsButton,        SIGNAL(triggered() ),
                    this,                   SLOT(deckButtonPressed() ) );
    connectAndTest( historyModel,           SIGNAL(itemAdded() ),
                    this,                   SLOT(updateVisibilities() ) );
    updateVisibilities();
    
    initBackAction();
}

/*!
 * \reimp
 *
 */
void RadioHistoryView::setOrientation()
{
    RadioHistoryModel& model = mMainWindow->uiEngine().historyModel();
    model.setShowDetails( mOrientation == Qt::Horizontal );
}

/*!
 * \reimp
 *
 */
void RadioHistoryView::showContextMenu( const QModelIndex& index )
{
    QModelIndex sourceIndex = mFilterModel->mapToSource( index );

    HbMenu* menu = new HbMenu();
    HbAction* action = menu->addAction( "Set favorite" );
    menu->exec();

//    RadioHistoryItem item = mFilterModel->data( index, )
}
