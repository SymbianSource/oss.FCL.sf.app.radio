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
#include "radiouiengine_p.h"
#include "radioservicedef.h"

/*!
 *
 */
RadioMonitorService::RadioMonitorService( RadioUiEnginePrivate& engine ) :
    QObject(),
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
void RadioMonitorService::init()
{
}

/*!
 *
 */
void RadioMonitorService::notifySong( const QString& song )
{
    Q_UNUSED( song )
}
