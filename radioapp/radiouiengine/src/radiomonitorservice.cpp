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
#include "radiouiengine_p.h"
#include "radiostationmodel.h"
#include "radioenginewrapper.h"
#include "radioscannerengine.h"
#include "radiostation.h"
#include "radioservicedef.h"
#include "radionotificationdata.h"
#include "radiologger.h"

#define RUN_NOTIFY( type, data ) \
    do { \
        QVariant variant; \
        variant.setValue( RadioNotificationData( RadioServiceNotification::type, data ) ); \
        notify( variant ); \
    } while ( 0 )

/*!
 *
 */
RadioMonitorService::RadioMonitorService( RadioUiEnginePrivate& engine ) :
    XQServiceProvider( RADIO_MONITOR_SERVICE, &engine.api() ),
    mUiEngine( engine ),
    mRadioStatus( RadioStatus::UnSpecified )
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
void RadioMonitorService::init()
{
    RadioStationModel* stationModel = &mUiEngine.api().stationModel();
    connectAndTest( stationModel,   SIGNAL(rowsRemoved(QModelIndex,int,int)),
                    this,           SLOT(notifyFavoriteCount()) );
    connectAndTest( stationModel,   SIGNAL(favoriteChanged(RadioStation)),
                    this,           SLOT(notifyFavoriteCount()) );
    connectAndTest( stationModel,   SIGNAL(stationDataChanged(RadioStation)),
                    this,           SLOT(notifyStationChange(RadioStation)) );
    connectAndTest( stationModel,   SIGNAL(radioTextReceived(RadioStation)),
                    this,           SLOT(notifyStationChange(RadioStation)) );

    RadioUiEngine* uiEngine = &mUiEngine.api();
    connectAndTest( uiEngine,       SIGNAL(seekingStarted(int)),
                    this,           SLOT(notifyRadioStatus()) );
    connectAndTest( uiEngine,       SIGNAL(muteChanged(bool)),
                    this,           SLOT(notifyRadioStatus()) );
    connectAndTest( uiEngine,       SIGNAL(antennaStatusChanged(bool)),
                    this,           SLOT(notifyRadioStatus()) );

    mUiEngine.wrapper().addObserver( this );

    notifyRadioStatus();
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
    RadioStationModel& stationModel = mUiEngine.api().stationModel();
    const RadioStation station = stationModel.currentStation();

    QVariantList notificationList;
    QVariant notification;

    RadioStatus::Status radioStatus = determineRadioStatus();
    notification.setValue( RadioNotificationData( RadioServiceNotification::RadioStatus, radioStatus ) );
    notificationList.append( notification );

    notification.setValue( RadioNotificationData( RadioServiceNotification::FavoriteCount, stationModel.favoriteCount() ) );
    notificationList.append( notification );

    notification.setValue( RadioNotificationData( RadioServiceNotification::Frequency, station.frequency() ) );
    notificationList.append( notification );

    if ( !station.name().isEmpty() ) {
        notification.setValue( RadioNotificationData( RadioServiceNotification::Name, station.name() ) );
        notificationList.append( notification );
    }

    if ( station.genre() > 0 ) {
        notification.setValue( RadioNotificationData( RadioServiceNotification::Genre,
                        mUiEngine.api().genreToString( station.genre(), GenreTarget::HomeScreen ) ) );
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
 * Private slot
 */
void RadioMonitorService::notifyRadioStatus()
{
    RadioStatus::Status radioStatus = determineRadioStatus();

    if ( radioStatus != mRadioStatus ) {
        if ( radioStatus == RadioStatus::Seeking ) {
            connectAndTest( mUiEngine.api().scannerEngine(),    SIGNAL(destroyed()),
                            this,                               SLOT(notifyRadioStatus()) );
        }

        mRadioStatus = radioStatus;
        RUN_NOTIFY( RadioStatus, radioStatus );
    }
}

/*!
 * Private slot
 *
 */
void RadioMonitorService::notifyFavoriteCount()
{
    const int favoriteCount = mUiEngine.api().stationModel().favoriteCount();
    RUN_NOTIFY( FavoriteCount, favoriteCount );
}

/*!
 * Private slot
 *
 */
void RadioMonitorService::notifyStationChange( const RadioStation& station )
{
    RadioUiEngine& uiEngine = mUiEngine.api();
    if ( uiEngine.isScanning() ) {
        return;
    }

    QVariantList list;
    QVariant notification;

    if ( station.hasDataChanged( RadioStation::GenreChanged ) ) {
        const QString genre = uiEngine.genreToString( station.genre(), GenreTarget::HomeScreen );
        notification.setValue( RadioNotificationData( RadioServiceNotification::Genre, genre ) );
        list.append( notification );
    }

    if ( station.hasDataChanged( RadioStation::NameChanged ) ) {
        notification.setValue( RadioNotificationData( RadioServiceNotification::Name, station.name() ) );
        list.append( notification );
    }

    if ( station.hasDataChanged( RadioStation::UrlChanged ) ) {
        notification.setValue( RadioNotificationData( RadioServiceNotification::HomePage, station.url() ) );
        list.append( notification );
    }

    if ( station.hasDataChanged( RadioStation::RadioTextChanged ) ) {
        notification.setValue( RadioNotificationData( RadioServiceNotification::RadioText, station.radioText() ) );
        list.append( notification );
    }

    notifyList( list );
}

/*!
 * \reimp
 */
void RadioMonitorService::tunedToFrequency( uint frequency, int reason )
{
    Q_UNUSED( reason );
    if ( !mUiEngine.api().isScanning() ) {
        RUN_NOTIFY( Frequency, frequency );
        RadioStation station;
        if ( mUiEngine.api().stationModel().findFrequency( frequency, station ) && !station.name().isEmpty() ) {
            RUN_NOTIFY( Name, station.name() );
        }
    }
}

/*!
 *
 */
RadioStatus::Status RadioMonitorService::determineRadioStatus() const
{
    RadioUiEngine& uiEngine = mUiEngine.api();
    if ( uiEngine.isScanning() ) {
        return RadioStatus::Seeking;
    } else if ( !uiEngine.isAntennaAttached() ) {
        return RadioStatus::NoAntenna;
    } else if ( uiEngine.isMuted() ) {
        return RadioStatus::Muted;
    }

    return RadioStatus::Playing;
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
    if ( mRequestIndexes.count() > 0 && list.count() > 0 ) {
        foreach ( int requestIndex, mRequestIndexes ) {
            completeRequest( requestIndex, list );
        }
        mRequestIndexes.clear();
    }
}
