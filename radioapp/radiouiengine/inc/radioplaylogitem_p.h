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

#ifndef RADIOPLAYLOGITEM_P_H
#define RADIOPLAYLOGITEM_P_H

// System includes
#include <qstring>

// User includes

class RadioPlayLogItemPrivate
{
public:

    RadioPlayLogItemPrivate();
    RadioPlayLogItemPrivate( const QString& artist, const QString& title );

    void init( const QString& artist, const QString& title );

public: // data

    /**
     * Reference count used for implicit sharing.
     * Has to be named "ref" instead having the 'm' prefix like all other member variables in Radio
     * This is because convenience functions like qAtomicAssign() expect it to be named "ref"
     */
    QAtomicInt      ref;

    QString         mArtist;

    QString         mTitle;

    int             mPlayCount;

    bool            mFavorite;

};

#endif // RADIOPLAYLOGITEM_P_H
