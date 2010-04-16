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

#ifndef RADIOPLAYLOGMODEL_H
#define RADIOPLAYLOGMODEL_H

// System includes
#include <QAbstractListModel>

// User includes
#include "radiouiengineexport.h"
#include "radioplaylogitem.h"

// Forward declarations
class RadioPlayLogModelPrivate;
class RadioUiEngine;
class RadioStation;

class UI_ENGINE_DLL_EXPORT RadioPlayLogModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D( d_ptr, RadioPlayLogModel )
    Q_DISABLE_COPY( RadioPlayLogModel )

    friend class RadioUiEngine;
    friend class RadioStationModelPrivate;
    friend class TestRadioUiEngine;
    
public:

    enum PlayLogRole
    {
        PlayLogItemRole = Qt::UserRole + 100,
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

    explicit RadioPlayLogModel( RadioUiEngine& uiEngine );

    ~RadioPlayLogModel();

    void addItem( const QString& artist, const QString& title, const RadioStation& station );

    void clearRadioTextPlus();
    void addRadioTextPlus( int rtClass, const QString& rtItem, const RadioStation& station );

    int findItem( const QString& artist, const QString& title, RadioPlayLogItem& item );

    void updateItem( int index, const RadioPlayLogItem& item, bool prepend = false );

private: // data

    /**
     * Unmodifiable pointer to the private implementation
     */
    RadioPlayLogModelPrivate* const d_ptr;

};

#endif // RADIOPLAYLOGMODEL_H
