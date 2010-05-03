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

#ifndef RADIOHISTORYMODEL_H
#define RADIOHISTORYMODEL_H

// System includes
#include <QAbstractListModel>

// User includes
#include "radiouiengineexport.h"
#include "radiohistoryitem.h"

// Forward declarations
class RadioHistoryModelPrivate;
class RadioUiEngine;
class RadioStation;

class UI_ENGINE_DLL_EXPORT RadioHistoryModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D( d_ptr, RadioHistoryModel )
    Q_DISABLE_COPY( RadioHistoryModel )

    friend class RadioUiEngine;
    friend class RadioStationModelPrivate;
    friend class TestRadioUiEngine;
    
public:

    enum HistoryRole
    {
        HistoryItemRole = Qt::UserRole + 100,
        SetFavoriteRole
    };

// from base class QAbstractListModel

    Qt::ItemFlags flags ( const QModelIndex& index ) const;
    int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

// New functions

    bool isCurrentSongRecognized() const;
    void setShowDetails( bool showDetails );

signals:

    void itemAdded();
    void currentSongReset();

public slots:

    void resetCurrentSong();
    void setFavorite();
    void removeAll();

private:

    explicit RadioHistoryModel( RadioUiEngine& uiEngine );

    ~RadioHistoryModel();

    void addItem( const QString& artist, const QString& title, const RadioStation& station );

    void clearRadioTextPlus();
    void addRadioTextPlus( int rtClass, const QString& rtItem, const RadioStation& station );

    int findItem( const QString& artist, const QString& title, RadioHistoryItem& item );

    void updateItem( int index, const RadioHistoryItem& item, bool prepend = false );

private: // data

    /**
     * Unmodifiable pointer to the private implementation
     */
    RadioHistoryModelPrivate* const d_ptr;

};

#endif // RADIOHISTORYMODEL_H
