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
#include <hblistview.h>
#include <hbaction.h>

// User includes
#include "radioplaylogview.h"
#include "radiomainwindow.h"
#include "radiologger.h"
#include "radioxmluiloader.h"
#include "radiouiengine.h"
#include "radioplaylogmodel.h"

/*!
 *
 */
RadioPlayLogView::RadioPlayLogView( RadioXmlUiLoader* uiLoader ) :
    RadioViewBase( uiLoader ),
    mPlayLogList( 0 )
{
}

/*!
 * From RadioViewBase
 *
 */
void RadioPlayLogView::init( RadioMainWindow* aMainWindow, RadioStationModel* aModel )
{
    LOG_METHOD;
    mMainWindow = aMainWindow;
    mModel = aModel;

    RadioPlayLogModel* playLogModel = &mMainWindow->uiEngine().playLogModel();

    mPlayLogList = mUiLoader->findObject<HbListView>( DOCML_NAME_PLAYLOGLIST );
    mPlayLogList->setScrollingStyle( HbListView::PanOrFlick );
    mPlayLogList->setModel( playLogModel );
    mPlayLogList->setSelectionMode( HbListView::NoSelection );
    mPlayLogList->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    HbAction* removeAction = mUiLoader->findObject<HbAction>( DOCML_NAME_PLV_REMOVEALLACTION );
    connectAndTest( removeAction, SIGNAL(triggered()), playLogModel, SLOT(removeAll()) );

    // "Go to tuning view" menu item
    connectViewChangeMenuItem( DOCML_NAME_PLV_TUNINGVIEWACTION, SLOT(activateTuningView()) );

    // "Go to stations view" menu item
    connectViewChangeMenuItem( DOCML_NAME_PLV_STATIONSVIEWACTION, SLOT(activateStationsView()) );
}
