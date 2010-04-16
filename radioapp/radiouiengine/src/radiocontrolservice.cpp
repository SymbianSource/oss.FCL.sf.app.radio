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
#include "radioserviceconst.h"

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
            mUiEngine.toggleMute();
            break;

        case RadioServiceCommand::Pause:
            mUiEngine.toggleMute();
            break;

        case RadioServiceCommand::Previous:
            mUiEngine.skipPrevious();
            break;

        case RadioServiceCommand::Next:
            mUiEngine.skipNext();
            break;

        case RadioServiceCommand::SeekUp:
            mUiEngine.seekUp();
            break;

        case RadioServiceCommand::SeekDown:
            mUiEngine.seekDown();
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
