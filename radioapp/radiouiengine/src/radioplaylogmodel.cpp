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
#include <QStringList>

// User includes
#include "radioplaylogmodel.h"
#include "radioplaylogmodel_p.h"
#include "radiouiengine.h"
#include "radioplaylogitem.h"
#include "radiostation.h"
#include "radiouiengine.h"
#include "radio_global.h"
#include "radiologger.h"

/*!
 *
 */
RadioPlayLogModel::RadioPlayLogModel( RadioUiEngine& uiEngine ) :
    QAbstractListModel( &uiEngine ),
    d_ptr( new RadioPlayLogModelPrivate( this, uiEngine ) )
{
    connectAndTest( &uiEngine,  SIGNAL(tunedToFrequency(uint,int)),
                    this,       SLOT(resetCurrentSong()) );
    connectAndTest( &uiEngine,  SIGNAL(seekingStarted(int)),
                    this,       SLOT(resetCurrentSong()) );
}

/*!
 *
 */
RadioPlayLogModel::~RadioPlayLogModel()
{
    Q_D( RadioPlayLogModel );
    d->mItems.clear();
}

/*!
 * \reimp
 */
Qt::ItemFlags RadioPlayLogModel::flags ( const QModelIndex& index ) const
{
    Qt::ItemFlags flags = QAbstractListModel::flags( index );
    flags |= Qt::ItemIsEditable;
    return flags;
}

/*!
 * \reimp
 */
int RadioPlayLogModel::rowCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent );
    Q_D( const RadioPlayLogModel );
    return d->mItems.count();
}

/*!
 * \reimp
 */
QVariant RadioPlayLogModel::data( const QModelIndex& index, int role ) const
{
    if ( !index.isValid() ) {
        return QVariant();
    }

    Q_D( const RadioPlayLogModel );
    if ( role == Qt::DisplayRole ) {
        RadioPlayLogItem item = d->mItems.at( index.row() );

        QStringList list;
        if ( d->mShowDetails ) {
            list.append( item.artist() + " - " + item.title() );
            list.append( item.time() + " " + item.station() + " " + RadioUiEngine::parseFrequency( item.frequency() ) );
        } else {
            list.append( item.artist() );
            list.append( item.title() );
        }

        return list;
    }

    return QVariant();
}

/*!
 * \reimp
 */
bool RadioPlayLogModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
    Q_UNUSED( value );
    if ( !index.isValid() ) {
        return false;
    }

    if ( role == RadioPlayLogModel::SetFavoriteRole ) {
        Q_D( RadioPlayLogModel );
        RadioPlayLogItem item = d->mItems.at( index.row() );
        item.setFavorite();
        updateItem( index.row(), item );
        return true;
    }

    return false;
}

/*!
 * Public slot
 */
void RadioPlayLogModel::resetCurrentSong()
{
    Q_D( RadioPlayLogModel );
    d->mTopItemIsPlaying = false;
    emit currentSongReset();
}

/*!
 * Public slot
 */
void RadioPlayLogModel::setFavorite()
{
    Q_D( RadioPlayLogModel );
    RadioPlayLogItem item = d->mItems.first();
    item.setFavorite();
    updateItem( 0, item );
}

/*!
 * Public slot
 */
void RadioPlayLogModel::removeAll()
{
    Q_D( RadioPlayLogModel );

    beginRemoveRows( QModelIndex(), 0, rowCount() - 1 );

    d->mItems.clear();

    endRemoveRows();
}

/*!
 *
 */
bool RadioPlayLogModel::isCurrentSongRecognized() const
{
    Q_D( const RadioPlayLogModel );
    return d->mTopItemIsPlaying;
}

/*!
 *
 */
void RadioPlayLogModel::setShowDetails( bool showDetails )
{
    Q_D( RadioPlayLogModel );
    d->mShowDetails = showDetails;
    reset();
}

/*!
 *
 */
void RadioPlayLogModel::addItem( const QString& artist, const QString& title, const RadioStation& station )
{
    Q_D( RadioPlayLogModel );

    RadioPlayLogItem item;
    const int itemIndex = findItem( artist, title, item );
    if ( itemIndex != -1 ) {
        item.increasePlayCount();
        updateItem( itemIndex, item, true );
    } else {
        item.setArtist( artist );
        item.setTitle( title );
        item.setStation( station.name() );
        item.setFrequency( station.frequency() );
        item.setCurrentTime();

        beginInsertRows( QModelIndex(), 0, 0 );

        d->mItems.prepend( item );

        endInsertRows();
    }

    d->mTopItemIsPlaying = true;
    emit itemAdded();
}

/*!
 *
 */
void RadioPlayLogModel::clearRadioTextPlus()
{
    Q_D( RadioPlayLogModel );
    d->mRtItemHolder = "";
    resetCurrentSong();
}

/*!
 *
 */
void RadioPlayLogModel::addRadioTextPlus( int rtClass, const QString& rtItem, const RadioStation& station )
{
    if ( rtClass == RtPlus::Artist || rtClass == RtPlus::Title ) {
        Q_D( RadioPlayLogModel );
        if ( d->mRtItemHolder.isEmpty() ) {
            d->mRtItemHolder = rtItem;
        } else {
            if ( rtClass == RtPlus::Title ) {
                addItem( d->mRtItemHolder, rtItem, station );
            } else {
                addItem( rtItem, d->mRtItemHolder, station );
            }
            d->mRtItemHolder = "";
        }
    }
}

/*!
 *
 */
int RadioPlayLogModel::findItem( const QString& artist, const QString& title, RadioPlayLogItem& item )
{
    Q_D( RadioPlayLogModel );
    const int itemCount = d->mItems.count();
    for ( int i = 0; i < itemCount; ++i ) {
        RadioPlayLogItem existingItem = d->mItems.at( i );
        if ( existingItem.artist().compare( artist ) == 0
             && existingItem.title().compare( title ) == 0 ) {
            item = existingItem;
            return i;
        }
    }

    return -1;
}

/*!
 *
 */
void RadioPlayLogModel::updateItem( int index, const RadioPlayLogItem& item, bool prepend )
{
    Q_D( RadioPlayLogModel );
    d->mItems.removeAt( index );

    if ( prepend ) {
        d->mItems.prepend( item );
    } else {
        d->mItems.insert( index, item );
    }
}
