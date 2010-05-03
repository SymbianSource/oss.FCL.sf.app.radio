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
#include "radiostationmodel.h"
#include "radiostation.h"
#include "radioservicedef.h"
#include "radionotificationdata.h"

#define RUN_NOTIFY( type, data ) \
    do { \
        QVariant variant; \
        variant.setValue( RadioNotificationData( RadioServiceNotification::type, data ) ); \
        notify( variant ); \
    } while ( 0 )

/*!
 *
 */
RadioMonitorService::RadioMonitorService( RadioUiEngine& engine ) :
    XQServiceProvider( RADIO_MONITOR_SERVICE, &engine ),
    mUiEngine( engine )
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
void RadioMonitorService::notifyFavoriteCount( const int favoriteCount )
{
    RUN_NOTIFY( FavoriteCount, favoriteCount );
}

/*!
 *
 */
void RadioMonitorService::notifyAntennaStatus( bool connected )
{
    RUN_NOTIFY( AntennaConnected, connected );
}

/*!
 *
 */
void RadioMonitorService::notifyRadioStatus( RadioStatus::Status radioStatus )
{
    RUN_NOTIFY( RadioStatus, radioStatus );
}

/*!
 *
 */
void RadioMonitorService::notifyFrequency( const uint frequency )
{
    RUN_NOTIFY( Frequency, frequency );
}

/*!
 *
 */
void RadioMonitorService::notifyName( const QString& name )
{
    RUN_NOTIFY( Name, name );
}

/*!
 *
 */
void RadioMonitorService::notifyGenre( const QString& genre )
{
    RUN_NOTIFY( Genre, genre );
}

/*!
 *
 */
void RadioMonitorService::notifyRadioText( const QString& radioText )
{
    RUN_NOTIFY( RadioText, radioText );
}

/*!
 *
 */
void RadioMonitorService::notifyHomePage( const QString& homePage )
{
    RUN_NOTIFY( HomePage, homePage );
}

/*!
 *
 */
void RadioMonitorService::notifySong( const QString& song )
{
    RUN_NOTIFY( Song, song );
}

/*!
 * Public slot
 *
 */
void RadioMonitorService::requestNotifications()
{
    mRequestIndexes.append( setCurrentRequestAsync() );
}

/*!
 * Public slot
 *
 */
void RadioMonitorService::requestAllData()
{
    const RadioStation station = mUiEngine.model().currentStation();

    QVariantList notificationList;

    QVariant notification;
    notification.setValue( RadioNotificationData( RadioServiceNotification::FavoriteCount, mUiEngine.model().favoriteCount() ) );
    notificationList.append( notification );

    notification.setValue( RadioNotificationData( RadioServiceNotification::Frequency, station.frequency() ) );
    notificationList.append( notification );

    if ( !station.name().isEmpty() ) {
        notification.setValue( RadioNotificationData( RadioServiceNotification::Name, station.name() ) );
        notificationList.append( notification );
    }

    if ( station.genre() > 0 ) {
        notification.setValue( RadioNotificationData( RadioServiceNotification::Genre, mUiEngine.genreToString( station.genre(), GenreTarget::HomeScreen ) ) );
        notificationList.append( notification );
    }

    if ( !station.radioText().isEmpty() ) {
        notification.setValue( RadioNotificationData( RadioServiceNotification::RadioText, station.radioText() ) );
        notificationList.append( notification );
    }

    if ( !station.url().isEmpty() ) {
        notification.setValue( RadioNotificationData( RadioServiceNotification::HomePage, station.url() ) );
        notificationList.append( notification );
    }

    //TODO: To be implemented
//    notification.setValue( RadioNotificationData( RadioServiceNotification::Song,  ) );
//    notificationList.append( notification );

    completeRequest( setCurrentRequestAsync(), notificationList );
}

/*!
 *
 */
void RadioMonitorService::notify( const QVariant& notification )
{
    QVariantList list;
    list.append( notification );
    notifyList( list );
}

/*!
 *
 */
void RadioMonitorService::notifyList( const QVariantList& list )
{
    if ( mRequestIndexes.count() > 0 ) {
        foreach ( int requestIndex, mRequestIndexes ) {
            completeRequest( requestIndex, list );
        }
        mRequestIndexes.clear();
    }
}
