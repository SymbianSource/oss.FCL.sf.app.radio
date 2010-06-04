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

#ifndef RADIOSTATIONFILTERMODEL_H_
#define RADIOSTATIONFILTERMODEL_H_

// System includes
#include <QSortFilterProxyModel>
#include <QMap>

// User includes
#include "radiouiengineexport.h"
#include "radiostation.h"

// Forward declarations
class RadioUiEngine;

class UI_ENGINE_DLL_EXPORT RadioStationFilterModel : public QSortFilterProxyModel
{
public:

    RadioStationFilterModel( RadioUiEngine& uiEngine, QObject* parent = 0 );

    ~RadioStationFilterModel();

// from QAbstractItemModel

    int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    QModelIndex index( int row, int column,
                       const QModelIndex &parent = QModelIndex() ) const;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;

// New functions

    void setTypeFilter( RadioStation::Type filter );

    /*!
     * Returns the model index corresponding to the given frequency
     */
    QModelIndex modelIndexFromFrequency( uint frequency );

    void setCyclic( bool cyclic );

    bool hasLooped( const QModelIndex& index ) const;

    QModelIndex realIndex( const QModelIndex& shadowIndex ) const;

    bool isEqual( const QModelIndex& first, const QModelIndex& second ) const;

private:

// from base class QSortFilterProxyModel

    bool filterAcceptsRow( int sourceRow, const QModelIndex& sourceParent ) const;

private: // data

    RadioUiEngine&      mUiEngine;

    RadioStation::Type  mFilter;

    bool                mIsCyclic;

    QMap<QModelIndex,QModelIndex>   mShadowIndexes;

};

#endif // RADIOSTATIONFILTERMODEL_H_
