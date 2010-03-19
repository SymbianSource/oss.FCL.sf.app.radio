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

#include "radiostationfiltermodel.h"
#include "radiouiengine.h"
#include "radiostationmodel.h"
#include "radiologger.h"

#include "radiostation.h" // Remove

/*!
 *
 */
RadioStationFilterModel::RadioStationFilterModel( RadioUiEngine& uiEngine, QObject* parent ) :
    QSortFilterProxyModel( parent ),
    mUiEngine( uiEngine ),
    mIsCyclic( false )
{
    setSourceModel( &mUiEngine.model() );
    setDynamicSortFilter( true );
}

/*!
 *
 */
RadioStationFilterModel::~RadioStationFilterModel()
{
}

/*!
 * \reimp
 */
int RadioStationFilterModel::rowCount( const QModelIndex& parent ) const
{
    const int count = QSortFilterProxyModel::rowCount( parent );
    return mIsCyclic ? count * 2 : count;
}

/*!
 * \reimp
 */
QModelIndex RadioStationFilterModel::index( int row, int column,
                                            const QModelIndex &parent ) const
{
    const int count = QSortFilterProxyModel::rowCount();
    if ( row < count ) {
        return QSortFilterProxyModel::index( row, column, parent );
    } else {
        foreach ( const QModelIndex& shadowIndex, mShadowIndexes.keys() ) {
            if ( shadowIndex.row() == row ) {
                return shadowIndex;
            }
        }

        return QModelIndex();
    }
}

/*!
 * \reimp
 */
QVariant RadioStationFilterModel::data( const QModelIndex& index, int role ) const
{
    QModelIndex dataIndex = index;
    if ( mShadowIndexes.contains( index ) ) {
        dataIndex = mShadowIndexes.value( index );
    }

    if ( !index.isValid() || !dataIndex.isValid() ) {
        return QVariant();
    }

    return QSortFilterProxyModel::data( dataIndex, role );
}

/*!
 *
 */
void RadioStationFilterModel::setTypeFilter( RadioStation::Type filter )
{
    mFilter = filter;
    filterChanged();
}

/*!
 * Returns the model index corresponding to the given frequency
 */
QModelIndex RadioStationFilterModel::modelIndexFromFrequency( uint frequency )
{
    QModelIndex index = static_cast<RadioStationModel*>( sourceModel() )->modelIndexFromFrequency( frequency );
    return mapFromSource( index );
}

/*!
 *
 */
void RadioStationFilterModel::setCyclic( bool cyclic )
{
    mIsCyclic = cyclic;
    if ( mIsCyclic ) {
        const int realCount = QSortFilterProxyModel::rowCount();
        LOG_FORMAT( "Station count: %d", realCount );
        for ( int i = 0; i < realCount; ++i ) {
            QModelIndex realIndex = QSortFilterProxyModel::index( i, 0 );
            QModelIndex shadowIndex = createIndex( i + realCount, 0, realIndex.internalPointer() );
            const uint freq = realIndex.data( RadioStationModel::RadioStationRole ).value<RadioStation>().frequency();
            LOG_FORMAT( "Adding shadow index %d for index %d. Freq: %u", shadowIndex.row(), realIndex.row(), freq );
            mShadowIndexes.insert( shadowIndex, realIndex );
        }
    }
}

/*!
 *
 */
bool RadioStationFilterModel::hasLooped( const QModelIndex& index ) const
{
    return mShadowIndexes.contains( index );
}

/*!
 *
 */
QModelIndex RadioStationFilterModel::realIndex( const QModelIndex& shadowIndex ) const
{
    return mShadowIndexes.value( shadowIndex );
}

/*!
 *
 */
bool RadioStationFilterModel::isEqual( const QModelIndex& first, const QModelIndex& second ) const
{
    if ( first == second ) {
        return true;
    }

    QModelIndex realFirst = first;
    if ( mShadowIndexes.contains( first ) ) {
        realFirst = mShadowIndexes.value( first );
    }

    QModelIndex realSecond = second;
    if ( mShadowIndexes.contains( second ) ) {
        realSecond = mShadowIndexes.value( second );
    }

    if ( realFirst == realSecond ) {
        return true;
    }

    return false;
}

/*!
 *
 */
bool RadioStationFilterModel::filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent ) const
{
    QAbstractItemModel* source = sourceModel();
    QModelIndex index = source->index( sourceRow, 0, sourceParent );
    const RadioStation station = source->data( index, RadioStationModel::RadioStationRole ).value<RadioStation>();
    const bool isType = station.isType( mFilter );
    return isType;
}
