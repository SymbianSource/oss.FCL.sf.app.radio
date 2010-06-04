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

// User includes
#include "radiocarouselmodel.h"
#include "radiocarouselmodel_p.h"
#include "radiostationmodel.h"
#include "radiouiengine.h"
#include "radiostation.h"
#include "radio_global.h"
#include "radiologger.h"

/*!
 *
 */
RadioCarouselModel::RadioCarouselModel( RadioUiEngine& uiEngine, RadioStationModel& stationModel ) :
    QAbstractListModel( &uiEngine ),
    d_ptr( new RadioCarouselModelPrivate( this, uiEngine, stationModel ) )
{
    connectAndTest( &stationModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                    this,          SIGNAL(dataChanged(QModelIndex,QModelIndex)) );
    connectAndTest( &stationModel, SIGNAL(layoutAboutToBeChanged()),
                    this,          SIGNAL(layoutAboutToBeChanged()) );
    connectAndTest( &stationModel, SIGNAL(layoutChanged()),
                    this,          SIGNAL(layoutChanged()) );
    connectAndTest( &stationModel, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
                    this,          SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)) );
    connectAndTest( &stationModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
                    this,          SIGNAL(rowsInserted(QModelIndex,int,int)) );
    connectAndTest( &stationModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                    this,          SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)) );
    connectAndTest( &stationModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),
                    this,          SIGNAL(rowsRemoved(QModelIndex,int,int)) );
    connectAndTest( &stationModel, SIGNAL(modelAboutToBeReset()),
                    this,          SIGNAL(modelAboutToBeReset()) );
    connectAndTest( &stationModel, SIGNAL(modelReset()),
                    this,          SIGNAL(modelReset()) );

//    connectAndTest( model(),        SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
//                    this,           SLOT(removeFrequency(QModelIndex,int,int)) );
}

/*!
 *
 */
RadioCarouselModel::~RadioCarouselModel()
{
    delete d_ptr;
}

/*!
 * \reimp
 */
int RadioCarouselModel::rowCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent );
    Q_D( const RadioCarouselModel );
    const int rowCount = d->mStationModel.rowCount();
    if ( rowCount == 0 ) {
        return 1;
    }
    return rowCount;
}

/*!
 * \reimp
 */
QVariant RadioCarouselModel::data( const QModelIndex& index, int role ) const
{
    if ( !index.isValid() ) {
        return QVariant();
    }

    Q_D( const RadioCarouselModel );
    if ( role == RadioStationModel::RadioStationRole ) {
        const int rowCount = d->mStationModel.rowCount();
        if ( rowCount == 0 ) {
            QVariant variant;
            variant.setValue( d->mStationModel.currentStation() );
            return variant;
        } else {
            return d->mStationModel.data( index, role );
        }
    }

    return QVariant();
}

/*!
 * Finds the closest station from the given frequency
 */
RadioStation RadioCarouselModel::findClosest( const uint frequency, StationSkip::Mode mode )
{
    Q_D( RadioCarouselModel );
    return d->mStationModel.findClosest( frequency, mode );
}

/*!
 * Returns the model index corresponding to the given frequency
 */
QModelIndex RadioCarouselModel::modelIndexFromFrequency( uint frequency )
{
    Q_D( RadioCarouselModel );
    if ( d->mStationModel.rowCount() == 0 ) {
        return index( 0, 0 );
    } else {
        RadioStation station;
        if ( d->mStationModel.findFrequency( frequency, station ) ) {
            return index( d->mStationModel.findPresetIndex( station.presetIndex() ), 0 );
        }
    }

    return QModelIndex();
}

/*!
 * \reimp
 */
void RadioCarouselModel::removeFrequency( const QModelIndex& parent, int first, int last )
{
    Q_UNUSED( parent );
    Q_UNUSED( first );
    Q_UNUSED( last );
}
