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

#include "radioplaylogitem.h"
#include "radioplaylogitem_p.h"

/*!
 *
 */
RadioPlayLogItemPrivate::RadioPlayLogItemPrivate()
{
    init( "", "" );
}

/*!
 *
 */
RadioPlayLogItemPrivate::RadioPlayLogItemPrivate( const QString& artist,
                                                  const QString& title )
{
    init( artist, title );
}

/*!
 *
 */
void RadioPlayLogItemPrivate::init( const QString& artist, const QString& title )
{
    ref = 1;
    mArtist = artist;
    mTitle = title;
    mFavorite = false;
    mPlayCount = 1;
}
