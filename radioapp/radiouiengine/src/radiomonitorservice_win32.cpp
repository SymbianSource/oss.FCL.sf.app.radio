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
#include "radiomonitorservice_win32.h"
#include "radiouiengine.h"
#include "radioservicedef.h"

/*!
 *
 */
RadioMonitorService::RadioMonitorService( RadioUiEngine& engine ) :
    QObject( &engine ),
    mUiEngine( engine )
{
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
    Q_UNUSED( favoriteCount )
}

/*!
 *
 */
void RadioMonitorService::notifyAntennaStatus( bool connected )
{
    Q_UNUSED( connected )
}

/*!
 *
 */
void RadioMonitorService::notifyRadioStatus( RadioStatus::Status radioStatus )
{
    Q_UNUSED( radioStatus )
}

/*!
 *
 */
void RadioMonitorService::notifyFrequency( const uint frequency )
{
    Q_UNUSED( frequency )
}

/*!
 *
 */
void RadioMonitorService::notifyName( const QString& name )
{
    Q_UNUSED( name )
}

/*!
 *
 */
void RadioMonitorService::notifyGenre( const QString& genre )
{
    Q_UNUSED( genre )
}

/*!
 *
 */
void RadioMonitorService::notifyRadioText( const QString& radioText )
{
    Q_UNUSED( radioText )
}

/*!
 *
 */
void RadioMonitorService::notifyHomePage( const QString& homePage )
{
    Q_UNUSED( homePage )
}

/*!
 *
 */
void RadioMonitorService::notifySong( const QString& song )
{
    Q_UNUSED( song )
}
