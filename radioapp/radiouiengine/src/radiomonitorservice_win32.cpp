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
#include "radioserviceconst.h"

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
void RadioMonitorService::notifyHomepage( const QString& homepage )
{
    Q_UNUSED( homepage )
}

/*!
 *
 */
void RadioMonitorService::notifySong( const QString& song )
{
    Q_UNUSED( song )
}
