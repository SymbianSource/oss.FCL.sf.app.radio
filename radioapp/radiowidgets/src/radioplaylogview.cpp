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

// User includes
#include "radioplaylogview.h"
#include "radiomainwindow.h"
#include "radiologger.h"
#include "radioxmluiloader.h"
#include "radiouiengine.h"
#include "radiostationfiltermodel.h"
#include "radioplaylogmodel.h"

const char* SECTION_SHOW_LIST = "show_list";
const char* SECTION_HIDE_LIST = "hide_list";
const char* SECTION_HISTORY_MODE = "history_mode";
const char* SECTION_FAVORITE_MODE = "favorite_mode";

/*!
 *
 */
RadioPlayLogView::RadioPlayLogView( RadioXmlUiLoader* uiLoader ) :
    RadioViewBase( uiLoader ),
    mPlayLogList( 0 ),
    mAllSongsButton( 0 ),
    mFavoritesButton( 0 )
{
}

/*!
 * Private slot
 *
 */
void RadioPlayLogView::deckButtonPressed()
{
    bool ok = false;
    if ( sender() == mFavoritesButton ) {
        mUiLoader->load( DOCML_PLAYLOGVIEW_FILE, SECTION_FAVORITE_MODE, &ok );
    } else {
        mUiLoader->load( DOCML_PLAYLOGVIEW_FILE, SECTION_HISTORY_MODE, &ok );
    }

    const bool showFavorites = mFavoritesButton->isChecked();
//    mFilterModel->setTypeFilter( showFavorites ? RadioStation::Favorite
//                                               : RadioStation::LocalStation );

    updateVisibilities();
}

/*!
 * Private slot
 *
 */
void RadioPlayLogView::clearList()
{
    mMainWindow->uiEngine().playLogModel().removeAll();
    updateVisibilities();
}

/*!
 * Private slot
 *
 */
void RadioPlayLogView::updateVisibilities()
{
    const int itemCount = mMainWindow->uiEngine().playLogModel().rowCount();
    bool ok = false;
    mUiLoader->load( DOCML_PLAYLOGVIEW_FILE, itemCount ? SECTION_SHOW_LIST : SECTION_HIDE_LIST, &ok );
}

/*!
 * Private slot
 *
 */
void RadioPlayLogView::listItemClicked( const QModelIndex& index )
{
    showContextMenu( index );
}

/*!
 * Private slot
 *
 */
void RadioPlayLogView::listItemLongPressed( HbAbstractViewItem* item, const QPointF& coords )
{
    Q_UNUSED( coords );
    showContextMenu( item->modelIndex() );
}

/*!
 * \reimp
 *
 */
void RadioPlayLogView::init( RadioMainWindow* aMainWindow, RadioStationModel* aModel )
{
    LOG_METHOD;
    mMainWindow = aMainWindow;
    mModel = aModel;

    RadioPlayLogModel* playLogModel = &mMainWindow->uiEngine().playLogModel();
    playLogModel->setShowDetails( mOrientation == Qt::Horizontal );

    mPlayLogList = mUiLoader->findObject<HbListView>( DOCML_NAME_PLAYLOGLIST );
    mPlayLogList->setScrollingStyle( HbListView::PanOrFlick );
    mFilterModel = mMainWindow->uiEngine().createNewFilterModel( this );
    mFilterModel->setSourceModel( playLogModel );
    mPlayLogList->setModel( mFilterModel );
    mPlayLogList->setSelectionMode( HbListView::NoSelection );
    mPlayLogList->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    mAllSongsButton     = mUiLoader->findObject<HbAction>( DOCML_NAME_ALLSONGSBUTTON );
    mFavoritesButton    = mUiLoader->findObject<HbAction>( DOCML_NAME_FAVORITESONGSBUTTON );

    HbAction* removeAction = mUiLoader->findObject<HbAction>( DOCML_NAME_PLV_REMOVEALLACTION );
    connectAndTest( removeAction, SIGNAL(triggered()), this, SLOT(clearList()) );

    connectAndTest( mFavoritesButton,       SIGNAL(triggered() ),
                    this,                   SLOT(deckButtonPressed() ) );
    connectAndTest( mAllSongsButton,        SIGNAL(triggered() ),
                    this,                   SLOT(deckButtonPressed() ) );
    connectAndTest( playLogModel,           SIGNAL(itemAdded() ),
                    this,                   SLOT(updateVisibilities() ) );
    updateVisibilities();
    
    initBackAction();
}

/*!
 * \reimp
 *
 */
void RadioPlayLogView::setOrientation()
{
    RadioPlayLogModel& model = mMainWindow->uiEngine().playLogModel();
    model.setShowDetails( mOrientation == Qt::Horizontal );
}

/*!
 * \reimp
 *
 */
void RadioPlayLogView::showContextMenu( const QModelIndex& index )
{
    QModelIndex sourceIndex = mFilterModel->mapToSource( index );

    HbMenu* menu = new HbMenu();
    HbAction* action = menu->addAction( "Set favorite" );
    menu->exec();

//    RadioPlayLogItem item = mFilterModel->data( index, )

    //    QModelIndex sourceIndex = mFilterModel->mapToSource( item->modelIndex() );
    //    RadioStation station = mModel->stationAt( sourceIndex.row() );
    //    RADIO_ASSERT( station.isValid() , "FMRadio", "invalid RadioStation");
    //
    //    RadioContextMenu* menu = mUiLoader->findObject<RadioContextMenu>( DOCML_NAME_CONTEXT_MENU );
    //    menu->init( station, *mUiLoader );
    //    menu->setPos( QPointF( size().width() / 2 - menu->size().width() / 2, coords.y() - menu->size().height() / 2 ) );
    //    menu->exec();
}
