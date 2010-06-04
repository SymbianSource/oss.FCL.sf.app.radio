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

#ifndef RADIOCAROUSELMODEL_H
#define RADIOCAROUSELMODEL_H

// System includes
#include <QAbstractListModel>

// User includes
#include "radiouiengineexport.h"
#include "radio_global.h"

// Forward declarations
class RadioCarouselModelPrivate;
class RadioUiEngine;
class RadioStation;
class RadioStationModel;

class UI_ENGINE_DLL_EXPORT RadioCarouselModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D( d_ptr, RadioCarouselModel )
    Q_DISABLE_COPY( RadioCarouselModel )

    friend class RadioUiEngine;
    friend class RadioStationModelPrivate;
    
public:

    ~RadioCarouselModel();

// from base class QAbstractListModel

    int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;

// New functions

    /*!
     * Finds the closest station from the given frequency
     */
    RadioStation findClosest( const uint frequency, StationSkip::Mode mode );

    /*!
     * Returns the model index corresponding to the given frequency
     */
    QModelIndex modelIndexFromFrequency( uint frequency );

private slots:

    void removeFrequency( const QModelIndex& parent, int first, int last );

private:

    explicit RadioCarouselModel( RadioUiEngine& uiEngine, RadioStationModel& stationModel );

private: // data

    /**
     * Unmodifiable pointer to the private implementation
     */
    RadioCarouselModelPrivate* const d_ptr;

};

#endif // RADIOCAROUSELMODEL_H
