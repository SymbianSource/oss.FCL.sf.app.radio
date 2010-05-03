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

#ifndef RADIOHISTORYITEM_P_H
#define RADIOHISTORYITEM_H

// System includes
#include <QString>
#include <QDateTime>

// User includes

class RadioHistoryItemPrivate
{
public:

    RadioHistoryItemPrivate();
    RadioHistoryItemPrivate( const QString& artist, const QString& title );

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

    QString         mStation;

    QDateTime       mTime;

    uint            mFrequency;

    int             mPlayCount;

    bool            mFavorite;

};

#endif // RADIOHISTORYITEM_H