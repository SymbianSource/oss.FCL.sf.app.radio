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
#include <HbAction>
#include <HbInputDialog>
#include <QGraphicsSceneResizeEvent>

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

    if ( station.isFavorite() )
    {
        uiLoader.findObject<HbAction>( DOCML_NAME_CONTEXT_FAVORITE )->setText( hbTrId( "txt_rad_menu_remove_favourite" ) );
    }
    else
    {
        uiLoader.findObject<HbAction>( DOCML_NAME_CONTEXT_FAVORITE )->setText( hbTrId( "txt_rad_menu_add_to_favourites" ) );
    }
}

/*!
 * Private slot
 *
 */
void RadioContextMenu::rename()
{
    HbInputDialog nameQuery;
    nameQuery.setPromptText( hbTrId( "txt_rad_dialog_new_name" ) );
    nameQuery.setInputMode( HbInputDialog::TextInput );
    nameQuery.setValue( mStation.name() );
    if ( nameQuery.exec() == nameQuery.primaryAction() )
    {
        mUiEngine.model().renameStation( mStation.presetIndex(), nameQuery.value().toString() );
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
