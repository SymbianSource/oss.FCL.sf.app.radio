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
#include "radiohistoryitem.h"
#include "radiohistoryitem_p.h"

/**
 * Static shared data instance that is used by all default-constructed RadioStation instances
 */
Q_GLOBAL_STATIC( RadioHistoryItemPrivate, shared_null )


/*!
 *
 */
RadioHistoryItem::RadioHistoryItem() :
    QObject( 0 )
{
    mData = shared_null();
    mData->ref.ref();
}

/*!
 *
 */
RadioHistoryItem::RadioHistoryItem( const QString& artist, const QString& title ) :
    QObject( 0 )
{
    mData = new RadioHistoryItemPrivate( artist, title );
}

/*!
 *
 */
RadioHistoryItem::RadioHistoryItem( const RadioHistoryItem& other ) :
    QObject( 0 )
{
    mData = other.mData;
    mData->ref.ref();
}

/*!
 *
 */
RadioHistoryItem::~RadioHistoryItem()
{
    decrementReferenceCount();
}

/*!
 *
 */
RadioHistoryItem& RadioHistoryItem::operator=( const RadioHistoryItem& other )
{
    qAtomicAssign( mData, other.mData );
    return *this;
}

/*!
 *
 */
bool RadioHistoryItem::isValid() const
{
    return id() != 0 && !title().isEmpty();
}

/*!
 *
 */
void RadioHistoryItem::reset()
{
    decrementReferenceCount();
    mData = shared_null();
    mData->ref.ref();
}

/*!
 *
 */
int RadioHistoryItem::id() const
{
    return mData->mId;
}

/*!
 *
 */
QString RadioHistoryItem::artist() const
{
    return mData->mArtist;
}

/*!
 *
 */
void RadioHistoryItem::setArtist( const QString& artist )
{
    if ( artist.compare( mData->mArtist ) != 0 ) {
        detach();
        mData->mArtist = artist;
    }
}

/*!
 *
 */
QString RadioHistoryItem::title() const
{
    return mData->mTitle;
}

/*!
 *
 */
void RadioHistoryItem::setTitle( const QString& title )
{
    if ( title.compare( mData->mTitle ) != 0 ) {
        detach();
        mData->mTitle = title;
    }
}

/*!
 *
 */
QString RadioHistoryItem::station() const
{
    return mData->mStation;
}

/*!
 *
 */
void RadioHistoryItem::setStation( const QString& station )
{
    if ( station.compare( mData->mStation ) != 0 ) {
        detach();
        mData->mStation = station;
    }
}

/*!
 *
 */
uint RadioHistoryItem::frequency() const
{
    return mData->mFrequency;
}

/*!
 *
 */
void RadioHistoryItem::setFrequency( uint frequency )
{
    if ( frequency != mData->mFrequency ) {
        detach();
        mData->mFrequency = frequency;
    }
}

/*!
 *
 */
QString RadioHistoryItem::time() const
{
    return mData->mTime.toString();
}

/*!
 *
 */
void RadioHistoryItem::setCurrentTime()
{
    detach();
    mData->mTime.currentDateTime();
}


/*!
 *
 */
bool RadioHistoryItem::isTagged() const
{
    return mData->mTagged;
}

/*!
 *
 */
bool RadioHistoryItem::isRecognizedByRds() const
{
    return mData->mFromRds;
}

/**
 * Decrements the reference count of the implicitly shared data.
 */
void RadioHistoryItem::decrementReferenceCount()
{
    if ( !mData->ref.deref() ) {
        delete mData;
        mData = 0;
    }
}

/**
 * Detach from the implicitly shared data
 */
void RadioHistoryItem::detach()
{
    if ( !isDetached() ) {
        RadioHistoryItemPrivate* newData = new RadioHistoryItemPrivate( *mData );

        decrementReferenceCount();

        newData->ref = 1;
        mData = newData;
    }
}

/**
 * Checks if the class is detached from implicitly shared data
 */
bool RadioHistoryItem::isDetached() const
{
    return mData->ref == 1;
}
