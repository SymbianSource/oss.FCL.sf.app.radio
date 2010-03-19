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
#include "radioplaylogitem.h"
#include "radioplaylogitem_p.h"

/**
 * Static shared data instance that is used by all default-constructed RadioStation instances
 */
Q_GLOBAL_STATIC( RadioPlayLogItemPrivate, shared_null )


/*!
 *
 */
RadioPlayLogItem::RadioPlayLogItem() :
    QObject( 0 )
{
    mData = shared_null();
    mData->ref.ref();
}

/*!
 *
 */
RadioPlayLogItem::RadioPlayLogItem( const QString& artist, const QString& title ) :
    QObject( 0 )
{
    mData = new RadioPlayLogItemPrivate( artist, title );
}

/*!
 *
 */
RadioPlayLogItem::RadioPlayLogItem( const RadioPlayLogItem& other ) :
    QObject( 0 )
{
    mData = other.mData;
    mData->ref.ref();
}

/*!
 *
 */
RadioPlayLogItem::~RadioPlayLogItem()
{
    decrementReferenceCount();
}

/*!
 *
 */
RadioPlayLogItem& RadioPlayLogItem::operator=( const RadioPlayLogItem& other )
{
    qAtomicAssign( mData, other.mData );
    return *this;
}

/*!
 *
 */
QString RadioPlayLogItem::artist() const
{
    return mData->mArtist;
}

/*!
 *
 */
void RadioPlayLogItem::setArtist( const QString& artist )
{
    if ( artist.compare( mData->mArtist ) != 0 ) {
        detach();
        mData->mArtist = artist;
    }
}

/*!
 *
 */
QString RadioPlayLogItem::title() const
{
    return mData->mTitle;
}

/*!
 *
 */
void RadioPlayLogItem::setTitle( const QString& title )
{
    if ( title.compare( mData->mTitle ) != 0 ) {
        detach();
        mData->mTitle = title;
    }
}

/*!
 *
 */
bool RadioPlayLogItem::isFavorite() const
{
    return mData->mFavorite;
}

/*!
 *
 */
void RadioPlayLogItem::setFavorite()
{
    if ( !mData->mFavorite ) {
        detach();
        mData->mFavorite = true;
    }
}

/*!
 *
 */
void RadioPlayLogItem::increasePlayCount()
{
    detach();
    ++mData->mPlayCount;
}

/*!
 *
 */
int RadioPlayLogItem::playCount() const
{
    return mData->mPlayCount;
}

/**
 * Decrements the reference count of the implicitly shared data.
 */
void RadioPlayLogItem::decrementReferenceCount()
{
    if ( !mData->ref.deref() ) {
        delete mData;
        mData = 0;
    }
}

/**
 * Detach from the implicitly shared data
 */
void RadioPlayLogItem::detach()
{
    if ( !isDetached() ) {
        RadioPlayLogItemPrivate* newData = new RadioPlayLogItemPrivate( *mData );

        decrementReferenceCount();

        newData->ref = 1;
        mData = newData;
    }
}

/**
 * Checks if the class is detached from implicitly shared data
 */
bool RadioPlayLogItem::isDetached() const
{
    return mData->ref == 1;
}
