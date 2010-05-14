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
#include <xqserviceutil.h>

// User includes
#include "radiocontrolservice.h"
#include "radiouiengine.h"
#include "radioservicedef.h"

/*!
 *
 */
RadioControlService::RadioControlService( RadioUiEngine& engine ) :
    XQServiceProvider( RADIO_CONTROL_SERVICE , &engine ),
    mUiEngine( engine )
{
    publishAll();
    if ( XQServiceUtil::isService() ) {
        XQServiceUtil::toBackground( true );
    }
}

/*!
 *
 */
RadioControlService::~RadioControlService()
{
}

/*!
 * Public slot
 */
void RadioControlService::command( int commandId )
{
    switch ( commandId )
    {
        case RadioServiceCommand::Play:
            mUiEngine.setMute( false );
            break;

        case RadioServiceCommand::Pause:
            mUiEngine.setMute( true );
            break;

        case RadioServiceCommand::Previous:
            mUiEngine.skipStation( StationSkip::PreviousFavorite );
            break;

        case RadioServiceCommand::Next:
            mUiEngine.skipStation( StationSkip::NextFavorite );
            break;

        case RadioServiceCommand::SeekUp:
            mUiEngine.seekStation( Seeking::Up );
            break;

        case RadioServiceCommand::SeekDown:
            mUiEngine.seekStation( Seeking::Down );
            break;

        case RadioServiceCommand::Foreground:
            XQServiceUtil::toBackground( false );
            break;

        case RadioServiceCommand::Background:
            XQServiceUtil::toBackground( true );
            break;

        default:
            break;
    }
}
