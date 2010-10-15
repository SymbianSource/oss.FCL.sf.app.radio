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

// System includes
#include <QTimer>
#include <QMetaMethod>

// User includes
#include "radiotimerpool.h"
#include "radiologger.h"

const int TIMER_BUFFER_SIZE = 2;

/*!
 *
 */
RadioTimerPool::RadioTimerItem::RadioTimerItem( RadioTimerPool* parent ) :
    mTimer( new QTimer() )
{
    Radio::connect( mTimer.data(),  SIGNAL(timeout()),
                    parent,         SLOT(timerFired()) );
    mTimer->setSingleShot( true );
}

/*!
 *
 */
RadioTimerPool::RadioTimerItem::~RadioTimerItem()
{
}

/*!
 *
 */
void RadioTimerPool::RadioTimerItem::reset()
{
    mTimer->stop();
    mId = -1;
    mParam = QVariant();
    mReceiver = NULL;
    mReceiverMember.clear();
}

/*!
 *
 */
RadioTimerPool::RadioTimerPool( QObject* parent ) :
    QObject( parent )
{
    createItem();
}

/*!
 *
 */
RadioTimerPool::~RadioTimerPool()
{
    qDeleteAll( mTimerMap.values() );
}

/*!
 *
 */
void RadioTimerPool::startTimer( int msec, int id, QObject* receiver, const char* member, QVariant param )
{
    LOG_METHOD;
    RadioTimerItem* availableItem = NULL;
    foreach ( RadioTimerItem* item, mTimerMap.values() ) {
        if ( !item->mTimer->isActive() ) {
            availableItem = item;
        } else {
            if ( item->mReceiver == receiver && item->mId == id ) {
                LOG_FORMAT( "RadioTimerPool::startTimer. Timer %d already running for class %s", id, receiver->metaObject()->className() );
            }
        }
    }

    if ( !availableItem ) {
        availableItem = createItem();
        LOG( "RadioTimerPool::startTimer. Creating new timer" );
    }
    LOG_FORMAT( "Amount of timers: %d", mTimerMap.count() );

    availableItem->mId = id;
    availableItem->mParam = param;
    availableItem->mReceiver = receiver;

    // SIGNAL() and SLOT() macros add a number in the beginning of member function name for some internal housekeeping
    // That causes the method to not be found from the meta object so we need to trim it off
    availableItem->mReceiverMember = member;
    availableItem->mReceiverMember = availableItem->mReceiverMember.right( availableItem->mReceiverMember.length() - 1 );

    availableItem->mTimer->start( msec );
}

/*!
 *
 */
void RadioTimerPool::cancelTimer( int id, const QObject* receiver )
{
    foreach ( RadioTimerItem* item, mTimerMap.values() ) {
        if ( item->mId == id && item->mReceiver == receiver ) {
            item->reset();
            break;
        }
    }
    freeUnusedTimers();
}

/*!
 *
 */
bool RadioTimerPool::isTimerActive( int id, const QObject* receiver ) const
{
    foreach ( const RadioTimerItem* item, mTimerMap.values() ) {
        if ( item->mId == id &&
             item->mReceiver == receiver &&
             item->mTimer->isActive() ) {
            return true;
        }
    }
    return false;
}

/*!
 *
 */
void RadioTimerPool::freeUnusedTimers()
{
    QList<RadioTimerItem*> unusedItems;
    foreach ( RadioTimerItem* item, mTimerMap.values() ) {
        if ( !item->mTimer->isActive() ) {
            unusedItems.append( item );
        }
    }

    if ( unusedItems.count() > TIMER_BUFFER_SIZE ) {
        foreach( const RadioTimerItem* item, unusedItems ) {
            mTimerMap.remove( item->mTimer.data() );
            delete item;
        }
    }
}

/*!
 * Private slot
 */
void RadioTimerPool::timerFired()
{
    QTimer* timer = static_cast<QTimer*>( sender() );
    if ( mTimerMap.contains( timer ) ) {
        RadioTimerItem* item = mTimerMap.value( timer );
        if ( item->mReceiver ) {
            QByteArray normalizedSignature = QMetaObject::normalizedSignature( item->mReceiverMember.toAscii().constData() );
            int methodIndex = item->mReceiver->metaObject()->indexOfMethod( normalizedSignature );
            if ( methodIndex != -1 ) {
                QMetaMethod method = item->mReceiver->metaObject()->method( methodIndex );
                const int paramCount = method.parameterTypes().count();
                if ( paramCount == 0 ) {
                    method.invoke( item->mReceiver, Qt::QueuedConnection );
                } else if ( paramCount == 1 ) {
                    method.invoke( item->mReceiver, Qt::QueuedConnection, Q_ARG( int, item->mId ) );
                } else if ( paramCount == 2 ) {
                    method.invoke( item->mReceiver, Qt::QueuedConnection,
                                   Q_ARG( int, item->mId ), Q_ARG( QVariant, item->mParam ) );
                }
            }

            item->reset();
        }
    }
}

/*!
 *
 */
RadioTimerPool::RadioTimerItem* RadioTimerPool::createItem()
{
    RadioTimerItem* item = new RadioTimerItem( this );
    mTimerMap.insert( item->mTimer.data(), item );
    return item;
}
