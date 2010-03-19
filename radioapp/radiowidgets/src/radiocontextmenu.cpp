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
#include <hbaction.h>
#include <hbinputdialog.h>
#include <qgraphicssceneresizeevent>

// User includes
#include "radiocontextmenu.h"
#include "radioxmluiloader.h"
#include "radiolocalization.h"
#include "radiouiengine.h"
#include "radiostationmodel.h"
#include "radiologger.h"

/*!
 *
 */
RadioContextMenu::RadioContextMenu( RadioUiEngine& uiEngine, QGraphicsItem* parent ) :
    HbMenu( parent ),
    mUiEngine( uiEngine )
{
}

/*!
 * Initializes the menu
 */
void RadioContextMenu::init( const RadioStation& station, RadioXmlUiLoader& uiLoader )
{
    mStation = station;

    if ( station.isRenamed() )
    {
        uiLoader.findObject<HbAction>( DOCML_NAME_CONTEXT_RENAME )->setText( TRANSLATE( KMenuItemRemoveRenaming ) );
    }
    else
    {
        uiLoader.findObject<HbAction>( DOCML_NAME_CONTEXT_RENAME )->setText( TRANSLATE( KMenuItemRenameStation ) );
    }

    if ( station.isFavorite() )
    {
        uiLoader.findObject<HbAction>( DOCML_NAME_CONTEXT_FAVORITE )->setText( TRANSLATE( KMenuItemRemoveFavorite ) );
    }
    else
    {
        uiLoader.findObject<HbAction>( DOCML_NAME_CONTEXT_FAVORITE )->setText( TRANSLATE( KMenuItemSetAsFavorite ) );
    }
}

/*!
 * Initializes the menu
 */
void RadioContextMenu::init( const RadioStation& station, QPointF pos )
{
    mStation = station;
    mPos = pos;

    if ( station.isRenamed() )
    {
        HbAction* action = addAction( TRANSLATE( KMenuItemRemoveRenaming ) );
        connectAndTest( action, SIGNAL(triggered()), this, SLOT(rename()) );
    }
    else
    {
        HbAction* action = addAction( TRANSLATE( KMenuItemRenameStation ) );
        connectAndTest( action, SIGNAL(triggered()), this, SLOT(rename()) );
    }

    if ( station.isFavorite() )
    {
        HbAction* action = addAction( TRANSLATE( KMenuItemRemoveFavorite ) );
        connectAndTest( action, SIGNAL(triggered()), this, SLOT(toggleFavorite()) );
    }
    else
    {
        HbAction* action = addAction( TRANSLATE( KMenuItemSetAsFavorite ) );
        connectAndTest( action, SIGNAL(triggered()), this, SLOT(toggleFavorite()) );
    }

    HbAction* deleteAction = addAction( TRANSLATE( KMenuItemRemoveStation ) );
    connectAndTest( deleteAction, SIGNAL(triggered()), this, SLOT(deleteStation()) );

    connectAndTest( this, SIGNAL(aboutToClose()), this, SLOT(deleteLater()) );
}

/*!
 * Private slot
 *
 */
void RadioContextMenu::rename()
{
    if ( mStation.isRenamed() )
    {
        mUiEngine.model().renameStation( mStation.presetIndex(), "" );
    }
    else
    {
        HbInputDialog nameQuery;
        nameQuery.setPromptText( TRANSLATE( KQueryEnterStationName ) );
        nameQuery.setInputMode( HbInputDialog::TextInput );
        nameQuery.setTextValue( mStation.name() );
        if ( nameQuery.exec() == nameQuery.primaryAction() )
        {
            mUiEngine.model().renameStation( mStation.presetIndex(), nameQuery.textValue().toString() );
        }
    }
}

/*!
 * Private slot
 *
 */
void RadioContextMenu::toggleFavorite()
{
    mUiEngine.model().setFavoriteByPreset( mStation.presetIndex(), !mStation.isFavorite() );
}

/*!
 * Private slot
 *
 */
void RadioContextMenu::deleteStation()
{
    mUiEngine.model().removeStation( mStation );
}

/*!
 * \reimp
 */
void RadioContextMenu::resizeEvent( QGraphicsSceneResizeEvent* event )
{
    HbMenu::resizeEvent( event );
    setPos( QPointF( mPos.x() - event->newSize().width() / 2, mPos.y() - event->newSize().height() / 2 ) );
}
