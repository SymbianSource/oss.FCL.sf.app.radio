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

#ifndef RADIOSTATIONMODEL_H_
#define RADIOSTATIONMODEL_H_

// System includes
#include <QAbstractListModel>
#include <QMap>

// User includes
#include "radiouiengineexport.h"
#include "radiostation.h"

// Forward declarations
class RadioStationModelPrivate;
class RadioPresetStorage;
class RadioStationHandlerIf;
class RadioEngineWrapper;
class RadioUiEngine;
class RadioStation;
class QIcon;

// Constants
typedef QMap<uint,RadioStation> Stations;

// Class declaration
class UI_ENGINE_DLL_EXPORT RadioStationModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D( d_ptr, RadioStationModel )
    Q_DISABLE_COPY( RadioStationModel )

public:

    enum RadioRole
    {
        RadioStationRole = Qt::UserRole + 1,
        ToggleFavoriteRole
    };

    enum DetailFlag
    {
        Minimal     = 1 << 0,
        ShowIcons   = 1 << 1,
        ShowGenre   = 1 << 2
    };
    Q_DECLARE_FLAGS( Detail, DetailFlag )

    explicit RadioStationModel( RadioUiEngine& uiEngine );

    ~RadioStationModel();

// from base class QAbstractListModel

    Qt::ItemFlags flags ( const QModelIndex& index ) const;
    int rowCount( const QModelIndex& parent = QModelIndex() ) const;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    bool setData( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

// New functions

    /*!
     * Called by the wrapper to initialize the list with given amount of presets
     */
    void initialize( RadioPresetStorage* storage, RadioEngineWrapper* engine );

    /*!
     * Sets the icons to be used in the lists
     */
    void setIcons( const QIcon& favoriteIcon, const QIcon& nowPlayingIcon );

    /*!
     * Returns a reference to the station handler interface
     */
    RadioStationHandlerIf& stationHandlerIf();

    /*!
     * Returns a reference to the underlying QList so that it can be easily looped
     */
    const Stations& list() const;

    /*!
     * Returns the station at the given index.
     * The station is returned by value, but it is cheap because it is implicitly shared
     */
    RadioStation stationAt( int index ) const;

    /*!
     * Functions to find stations by frequency
     */
    bool findFrequency( uint frequency, RadioStation& station );

    /*!
     * Functions to find stations by preset index
     */
    int findPresetIndex( int presetIndex );
    int findPresetIndex( int presetIndex, RadioStation& station );

    /*!
     * Functions to remove stations
     */
    void removeByFrequency( uint frequency );
    void removeByPresetIndex( int presetIndex );
    void removeStation( const RadioStation& station );

    /*!
     * Functions to add and save stations
     */
    void addStation( const RadioStation& station );
    void saveStation( RadioStation& station );

    /*!
     * Convenience functions to change common settings
     */
    void setFavoriteByFrequency( uint frequency, bool favorite );
    void setFavoriteByPreset( int presetIndex, bool favorite );
    void renameStation( int presetIndex, const QString& name );
    void setFavorites( const QModelIndexList& favorites );

    /*!
     * Functions to init and access the currently tuned station
     */
    RadioStation& currentStation();
    const RadioStation& currentStation() const;

    /*!
     * Sets the model detail level
     */
    void setDetail( Detail level );

    /*!
     * Returns a list of radio stations in the given frequency range
     */
    QList<RadioStation> stationsInRange( uint minFrequency, uint maxFrequency );

    /*!
     * Returns the model index corresponding to the given frequency
     */
    QModelIndex modelIndexFromFrequency( uint frequency );

signals:

    void stationDataChanged( const RadioStation& station );
    void radioTextReceived( const RadioStation& station );
    void dynamicPsChanged( const RadioStation& station );
    void favoriteChanged( const RadioStation& station );
    void stationRemoved( const RadioStation& station );
    void stationAdded( const RadioStation& station );

public slots:

    /**
     * Removes all stations
     */
    void removeAll();

private slots:

    /*!
     * Timer timeout slot to indicate that the dynamic PS check has ended
     */
    void dynamicPsCheckEnded();

private:

// New functions

    /*!
     * Checks the given station and emits signals based on what member variables had been changed
     * since the last save or reset.
     */
    void emitChangeSignals( const RadioStation& station, RadioStation::Change flags );

    void emitDataChanged( const RadioStation& station );

    /*!
     * Finds an unused preset index
     */
    int findUnusedPresetIndex();

    /**
     * Used by the RDS data setters to find the correct station where the data is set
     * First tries the currentStation variable and if the frequency doesn't match, finds the right one
     */
    RadioStation findCurrentStation( uint frequency );

private: // data

    /**
     * Unmodifiable pointer to the private implementation
     */
    RadioStationModelPrivate* const d_ptr;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( RadioStationModel::Detail )

#endif // RADIOSTATIONMODEL_H_
