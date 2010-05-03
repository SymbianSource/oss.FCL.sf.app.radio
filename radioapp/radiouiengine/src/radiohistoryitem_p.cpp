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

#include "radiohistoryitem.h"
#include "radiohistoryitem_p.h"

/*!
 *
 */
RadioHistoryItemPrivate::RadioHistoryItemPrivate()
{
    init( "", "" );
}

/*!
 *
 */
RadioHistoryItemPrivate::RadioHistoryItemPrivate( const QString& artist,
                                                  const QString& title )
{
    init( artist, title );
}

/*!
 *
 */
void RadioHistoryItemPrivate::init( const QString& artist, const QString& title )
{
    ref = 1;
    mArtist = artist;
    mTitle = title;
    mFrequency = 0;
    mFavorite = false;
    mPlayCount = 1;
}
