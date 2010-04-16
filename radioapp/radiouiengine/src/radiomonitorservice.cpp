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

// User includes
#include "radiomonitorservice.h"
#include "radiouiengine.h"
#include "radioserviceconst.h"

/*!
 *
 */
RadioMonitorService::RadioMonitorService( RadioUiEngine& engine ) :
    XQServiceProvider( RADIO_MONITOR_SERVICE, &engine ),
    mUiEngine( engine ),
    mRequestIndex( 0 )
{
    publishAll();
}

/*!
 *
 */
RadioMonitorService::~RadioMonitorService()
{
}

/*!
 *
 */
void RadioMonitorService::notifyName( const QString& name )
{
    notify( RadioServiceNotification::Name, name );
}

/*!
 *
 */
void RadioMonitorService::notifyGenre( const QString& genre )
{
    notify( RadioServiceNotification::Genre, genre );
}

/*!
 *
 */
void RadioMonitorService::notifyRadioText( const QString& radioText )
{
    notify( RadioServiceNotification::RadioText, radioText );
}

/*!
 *
 */
void RadioMonitorService::notifyHomepage( const QString& homepage )
{
    notify( RadioServiceNotification::Homepage, homepage );
}

/*!
 *
 */
void RadioMonitorService::notifySong( const QString& song )
{
    notify( RadioServiceNotification::Song, song );
}

/*!
 * Public slot
 *
 */
void RadioMonitorService::requestNotifications()
{
    mRequestIndex = setCurrentRequestAsync();
}

/*!
 *
 */
void RadioMonitorService::notify( int notificationId, const QString& message )
{
    if ( mRequestIndex > 0 ) {
        completeRequest( mRequestIndex, QString( "%1 %2" ).arg( notificationId ).arg( message ) );
        mRequestIndex = 0;
    }
}
