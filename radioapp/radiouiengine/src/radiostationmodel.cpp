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
#include <qstringlist>

#include "radiostationmodel.h"
#include "radiostationmodel_p.h"
#include "radiopresetstorage.h"
#include "radioenginewrapper.h"
#include "radiouiengine.h"
#include "radiostation.h"
#include "radiostation_p.h"
#include "radiologger.h"

/*!
 *
 */
static QString parseLine( const RadioStation& station )
{
    QString line = "";
    line.append( RadioUiEngine::parseFrequency( station.frequency() ) );

    QString name = station.name();
    if ( !name.isEmpty() )
    {
        line.append( " - " );
        line.append( name.trimmed() );
    }

    LOG_FORMAT( "RadioStationModel: Returning line %s", GETSTRING(line) );
    return line;
}

/*!
 *
 */
RadioStationModel::RadioStationModel( RadioUiEngine& uiEngine ) :
    QAbstractListModel( &uiEngine ),
    d_ptr( new RadioStationModelPrivate( this, uiEngine ) )
{
}

/*!
 *
 */
RadioStationModel::~RadioStationModel()
{
}

/*!
 *
 */
Qt::ItemFlags RadioStationModel::flags ( const QModelIndex& index ) const
{
    Qt::ItemFlags flags = QAbstractListModel::flags( index );
    flags |= Qt::ItemIsEditable;
    return flags;
}

/*!
 *
 */
int RadioStationModel::rowCount( const QModelIndex& parent ) const
{
    Q_UNUSED( parent );
    Q_D( const RadioStationModel );
    const int count = d->mStations.keys().count();
    return count;
}

/*!
 * Checks the given station and emits signals based on what member variables had been changed
 */
QVariant RadioStationModel::data( const QModelIndex& index, int role ) const
{
    if ( !index.isValid() ) {
        return QVariant();
    }

    Q_D( const RadioStationModel );
    if ( role == Qt::DisplayRole ) {
        RadioStation station = stationAt( index.row() );
        QString firstLine = parseLine( station );
        if ( d->mDetailLevel.testFlag( RadioStationModel::ShowGenre ) ) {
            QStringList list;
            list.append( firstLine );
            QString genre = " "; // Empty space so that the listbox generates the second row
            if ( station.genre() != -1 ) {
                genre = d->mUiEngine.genreToString( station.genre() );
            }
            list.append( genre );

            return list;
        }

        return firstLine;
    } else if ( role == RadioStationModel::RadioStationRole ) {
        QVariant variant;
        variant.setValue( stationAt( index.row() ) );
        return variant;
    } else if ( role == Qt::DecorationRole &&
                d->mDetailLevel.testFlag( RadioStationModel::ShowIcons ) ) {
        RadioStation station = stationAt( index.row() );
        QVariantList list;
        if ( station.isFavorite() && !d->mFavoriteIcon.isNull() ) {
            list.append( d->mFavoriteIcon );
        } else {
            list.append( QIcon() );
        }
        if ( currentStation().frequency() == station.frequency() && !d->mNowPlayingIcon.isNull() ) {
            list.append( d->mNowPlayingIcon );
        }
        return list;
    }

    return QVariant();
}

/*!
 * Checks the given station and emits signals based on what member variables had been changed
 */
bool RadioStationModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
    Q_UNUSED( index );

    if ( role == RadioStationModel::ToggleFavoriteRole ) {
        const uint frequency = value.toUInt();
        RadioStation station;
        if ( findFrequency( frequency, station ) ) {
            setFavoriteByPreset( station.presetIndex(), !station.isFavorite() );
        } else {
            setFavoriteByFrequency( frequency, true );
        }

        return true;
    }

    return false;
}

/*!
 * Called by the wrapper to initialize the list with given amount of presets
 */
void RadioStationModel::initialize( RadioPresetStorage* storage, RadioEngineWrapper* engine )
{
    Q_D( RadioStationModel );
    d->mPresetStorage = storage;
    d->mEngine = engine;
    const int presetCount = d->mPresetStorage->presetCount();
    int index = d->mPresetStorage->firstPreset();
    LOG_FORMAT( "RadioStationModelPrivate::initialize: presetCount: %d, firstIndex: %d", presetCount, index );

#ifdef COMPILE_WITH_NEW_PRESET_UTILITY
    while ( index >= 0 ) {
#else
    index = 0;
    while ( index < presetCount ) {
#endif // COMPILE_WITH_NEW_PRESET_UTILITY

        RadioStation station;
        station.detach();

        RadioStationIf* preset = static_cast<RadioStationIf*>( station.data_ptr() );
        if ( d->mPresetStorage->readPreset( index, *preset ) ) {
            RADIO_ASSERT( station.isValid(), "RadioStationModelPrivate::initialize", "Invalid station" );
			// TODO: Remove this if when new Preset utility is taken into use
            if ( station.frequency() != 87500000 )
                {
                d->mStations.insert( station.frequency(), station );
                }
        }
        ++index;
#ifdef COMPILE_WITH_NEW_PRESET_UTILITY
        index = d->mPresetStorage->nextPreset( index );
#endif
    }

    d->setCurrentStation( d->mEngine->currentFrequency() );
}

/*!
 * Sets the icons to be used in the lists
 */
void RadioStationModel::setIcons( const QIcon& favoriteIcon, const QIcon& nowPlayingIcon )
{
    Q_D( RadioStationModel );
    d->mFavoriteIcon = favoriteIcon;
    d->mNowPlayingIcon = nowPlayingIcon;
}

/*!
 * Returns a reference to the station handler interface
 */
RadioStationHandlerIf& RadioStationModel::stationHandlerIf()
{
    Q_D( RadioStationModel );
    return *d;
}

/*!
 * Returns a reference to the underlying QList so that it can be easily looped
 */
const Stations& RadioStationModel::list() const
{
    Q_D( const RadioStationModel );
    return d->mStations;
}

/*!
 * Returns the station at the given index.
 */
RadioStation RadioStationModel::stationAt( int index ) const
{
    // Get the value from the keys list instead of directly accessing the values list
    // because QMap may have added a default-constructed value to the values list
    Q_D( const RadioStationModel );
    if ( index < d->mStations.keys().count() ) {
        uint frequency = d->mStations.keys().at( index );
        return d->mStations.value( frequency );
    }
    return RadioStation();
}

/*!
 * Finds a station by frequency
 */
bool RadioStationModel::findFrequency( uint frequency, RadioStation& station )
{
    Q_D( RadioStationModel );
    if ( d->mStations.contains( frequency ) ) {
        station = d->mStations.value( frequency );
        return true;
    }
    return false;
}

/*!
 * Finds a station by preset index
 */
int RadioStationModel::findPresetIndex( int presetIndex )
{
    Q_D( RadioStationModel );
    int index = 0;
    foreach( const RadioStation& tempStation, d->mStations ) {
        if ( tempStation.presetIndex() == presetIndex ) {
            return index;
        }
        ++index;
    }

    return RadioStation::NotFound;
}

/*!
 * Finds a station by preset index
 */
int RadioStationModel::findPresetIndex( int presetIndex, RadioStation& station )
{
    Q_D( RadioStationModel );
    const int index = findPresetIndex( presetIndex );
    if ( index != RadioStation::NotFound ) {
        station = d->mStations.values().at( index );
    }
    return index;
}

/*!
 * Removes a station by frequency
 */
void RadioStationModel::removeByFrequency( uint frequency )
{
    RadioStation station;
    if ( findFrequency( frequency, station ) ) {
        removeStation( station );
    }
}

/*!
 * Removes a station by preset index
 */
void RadioStationModel::removeByPresetIndex( int presetIndex )
{
    RadioStation station;
    const int index = findPresetIndex( presetIndex, station );
    if ( index >= 0 ) {
        removeStation( station );
    }
}

/*!
 * Removes the given station
 */
void RadioStationModel::removeStation( const RadioStation& station )
{
    Q_D( RadioStationModel );
    const uint frequency = station.frequency();
    if ( d->mStations.contains( frequency ) ) {

        // If we are removing the current station, copy its data to the current station pointer
        // to keep all of the received RDS data still available. They will be discarded when
        // the user tunes to another frequency, but they are available if the user decides to add it back.
        if ( d->mCurrentStation->frequency() == frequency ) {
            *d->mCurrentStation = station;
        }

        // Copy the station to a temporary variable that can be used as signal parameter
        RadioStation tempStation = station;

        const int row = modelIndexFromFrequency( tempStation.frequency() ).row();
        beginRemoveRows( QModelIndex(), row, row );

        d->mPresetStorage->deletePreset( tempStation.presetIndex() );
        d->mStations.remove( frequency );

        endRemoveRows();

        d->setCurrentStation( d->mEngine->currentFrequency() );

        if ( tempStation.isFavorite() ) {
            tempStation.setFavorite( false );
            emit favoriteChanged( tempStation );
        }
    }
}

/*!
 * Adds a new station to the list
 */
void RadioStationModel::addStation( const RadioStation& station )
{
    Q_D( RadioStationModel );
    const int newIndex = findUnusedPresetIndex();
    LOG_FORMAT( "RadioStationModelPrivate::addStation: Adding station to index %d", newIndex );

    RadioStation newStation = station;
    newStation.setPresetIndex( newIndex );
    newStation.unsetType( RadioStation::Temporary );

    // We have to call beginInsertRows() BEFORE the addition is actually done so we must figure out where
    // the new station will go in the sorted frequency order
//    int row = 0;
//    const int count = rowCount();
//    if ( count > 1 ) {
//        Stations::const_iterator iter = d->mStations.upperBound( newStation.frequency() );
//        uint iterFreq = iter.key();
//        if ( d->mStations.contains( iter.key() ) ) {
//            row = d->mStations.keys().indexOf( iter.key() );
//        } else {
//            row = count;
//        }
//    } else if ( count == 1 ) {
//        uint existingFreq = d->mStations.keys().first();
//        if ( station.frequency() > existingFreq ) {
//            row = 1;
//        }
//    }
//
    emit layoutAboutToBeChanged();
//    beginInsertRows( QModelIndex(), row, row );
    // We must add the station here because saveStation() will only update an existing station
    d->mStations.insert( newStation.frequency(), newStation );

    saveStation( newStation );
    d->setCurrentStation( d->mEngine->currentFrequency() );

//    endInsertRows();

    emit layoutChanged();
    emit stationAdded( station );
}

/*!
 * Saves the given station. It is expected to already exist in the list
 */
void RadioStationModel::saveStation( RadioStation& station )
{
    Q_D( RadioStationModel );
    const bool stationHasChanged = station.hasChanged();
    RadioStation::Change changeFlags = station.changeFlags();
    station.resetChangeFlags();

    if ( station.isType( RadioStation::Temporary ) ) {

        emitChangeSignals( station, changeFlags );

    } else if ( station.isValid() && stationHasChanged && d->mStations.contains( station.frequency() )) {

        d->mStations.insert( station.frequency(), station );

        if ( changeFlags.testFlag( RadioStation::PersistentDataChanged ) ) {
            const bool success = d->mPresetStorage->savePreset( *station.data_ptr() );
            RADIO_ASSERT( success, "RadioStationModelPrivate::saveStation", "Failed to add station" );
        }

        emitChangeSignals( station, changeFlags );
    }
}

/*!
 * Changes the favorite status of a station by its frequency. If the station does
 * not yet exist, it is added.
 */
void RadioStationModel::setFavoriteByFrequency( uint frequency, bool favorite )
{
    Q_D( RadioStationModel );
    if ( d->mEngine->isFrequencyValid( frequency ) )
    {
        LOG_FORMAT( "RadioStationModelPrivate::setFavoriteByFrequency, frequency: %d", frequency );
        RadioStation station;
        if ( findFrequency( frequency, station ) )  // Update existing preset
        {
            if ( station.isFavorite() != favorite )
            {
                station.setFavorite( favorite );
                saveStation( station );
            }
        }
        else if ( favorite )                    // Add new preset if setting as favorite
        {
            RadioStation newStation;
            if ( d->mCurrentStation->frequency() == frequency ) {
                newStation = *d->mCurrentStation;
            } else {
                LOG( "CurrentStation frequency mismatch!" );
                newStation.setFrequency( frequency );
            }

            newStation.setType( RadioStation::LocalStation | RadioStation::Favorite );

            // If PI code has been received, it is a local station
            if ( newStation.hasPiCode() ) {
                newStation.setType( RadioStation::LocalStation );
            }

            // Emit the signals only after adding the preset and reinitializing the current station
            // because the UI will probably query the current station in its slots that get called.
            addStation( newStation );
            d->setCurrentStation( frequency );
        }
    }
}

/*!
 * Changes the favorite status of a station by its preset index
 */
void RadioStationModel::setFavoriteByPreset( int presetIndex, bool favorite )
{
    LOG_FORMAT( "RadioStationModelPrivate::setFavoriteByPreset, presetIndex: %d", presetIndex );
    RadioStation station;
    if ( findPresetIndex( presetIndex, station ) != RadioStation::NotFound )
    {
        station.setFavorite( favorite );
        saveStation( station );
    }
}

/*!
 * Renames a station by its preset index
 */
void RadioStationModel::renameStation( int presetIndex, const QString& name )
{
    LOG_FORMAT( "RadioStationModelPrivate::renameStation, presetIndex: %d, name: %s", presetIndex, GETSTRING(name) );
    RadioStation station;
    if ( findPresetIndex( presetIndex, station ) != RadioStation::NotFound )
    {
        station.setUserDefinedName( name );
        saveStation( station );
    }
}

/*!
 *
 */
void RadioStationModel::setFavorites( const QModelIndexList& favorites )
{
    foreach ( const QModelIndex& index, favorites ) {
        RadioStation station = stationAt( index.row() );
        RADIO_ASSERT( station.isValid() , "RadioStationModel::setFavorites", "invalid RadioStation");
        setFavoriteByPreset( station.presetIndex(), true );
    }
}

/*!
 * Returns the currently tuned station
 */
RadioStation& RadioStationModel::currentStation()
{
    Q_D( RadioStationModel );
    return *d->mCurrentStation;
}

/*!
 * Returns the currently tuned station
 */
const RadioStation& RadioStationModel::currentStation() const
{
    Q_D( const RadioStationModel );
    return *d->mCurrentStation;
}

/*!
 * Sets the model detail level
 */
void RadioStationModel::setDetail( Detail level )
{
    Q_D( RadioStationModel );
    d->mDetailLevel = level;
}

/*!
 * Returns a list of radio stations in the given frequency range
 */
QList<RadioStation> RadioStationModel::stationsInRange( uint minFrequency, uint maxFrequency )
{
    Q_D( RadioStationModel );
    QList<RadioStation> stations;
    foreach( const RadioStation& station, d->mStations ) {
        if ( station.frequency() >= minFrequency && station.frequency() <= maxFrequency ) {
            stations.append( station );
        }
    }

    return stations;
}

/*!
 * Returns the model index corresponding to the given frequency
 */
QModelIndex RadioStationModel::modelIndexFromFrequency( uint frequency )
{
    RadioStation station;
    if ( findFrequency( frequency, station ) ) {
        return index( findPresetIndex( station.presetIndex() ), 0 );
    }
    return QModelIndex();
}

/*!
 * Public slot
 * Removes all stations
 */
void RadioStationModel::removeAll()
{
    Q_D( RadioStationModel );
	
    if ( d->mStations.count() == 0 ) {
        return;
    }
	
    QList<RadioStation> favorites;
    foreach( const RadioStation& station, d->mStations ) {
        if ( station.isFavorite() ) {
            favorites.append( station );
        }
    }

    beginRemoveRows( QModelIndex(), 0, rowCount() - 1 );

    // Preset utility deletes all presets with index -1
#ifdef COMPILE_WITH_NEW_PRESET_UTILITY
    bool success = d->mPresetStorage->deletePreset( -1 );
#else
    bool success = d->mPresetStorage->deletePreset( 0 );
#endif // COMPILE_WITH_NEW_PRESET_UTILITY
    RADIO_ASSERT( success, "FMRadio", "Failed to remove station" );

    d->mStations.clear();
    d->setCurrentStation( d->mEngine->currentFrequency() );

    endRemoveRows();

    foreach( RadioStation station, favorites ) {
        station.setFavorite( false );
        emit favoriteChanged( station );
    }
}

/*!
 * Private slot
 * Timer timeout slot to indicate that the dynamic PS check has ended
 */
void RadioStationModel::dynamicPsCheckEnded()
{
    Q_D( RadioStationModel );
    LOG_TIMESTAMP( "Finished dynamic PS check." );
    if ( d->mCurrentStation->psType() != RadioStation::Dynamic && !d->mCurrentStation->dynamicPsText().isEmpty() )
    {
        d->mCurrentStation->setPsType( RadioStation::Static );
        d->mCurrentStation->setName( d->mCurrentStation->dynamicPsText() );
        d->mCurrentStation->setDynamicPsText( "" );
        saveStation( *d->mCurrentStation );
    }
}

/*!
 * Checks the given station and emits signals based on what member variables had been changed
 */
void RadioStationModel::emitChangeSignals( const RadioStation& station, RadioStation::Change flags )
{
    if ( flags.testFlag( RadioStation::NameChanged ) ||
         flags.testFlag( RadioStation::GenreChanged ) ||
         flags.testFlag( RadioStation::UrlChanged ) ||
         flags.testFlag( RadioStation::TypeChanged ) ||
         flags.testFlag( RadioStation::PiCodeChanged ) ) {

        // Create a temporary RadioStation for the duration of the signal-slot processing
        // The receivers can ask the station what data has changed and update accordingly
        RadioStation tempStation( station );
        tempStation.setChangeFlags( flags );
        emit stationDataChanged( tempStation );

        emitDataChanged( tempStation );
    }

    if ( flags.testFlag( RadioStation::RadioTextChanged ) ) {
        emit radioTextReceived( station );
        emitDataChanged( station );
    }

    if ( flags.testFlag( RadioStation::DynamicPsChanged ) ) {
        emit dynamicPsChanged( station );
        emitDataChanged( station );
    }

    if ( flags.testFlag( RadioStation::FavoriteChanged ) && station.isValid() ) {
        emit favoriteChanged( station );
        emitDataChanged( station );
    }
}

/*!
 *
 */
void RadioStationModel::emitDataChanged( const RadioStation& station )
{
    const int row = findPresetIndex( station.presetIndex() );
    QModelIndex top = index( row, 0, QModelIndex() );
    QModelIndex bottom = index( row, 0, QModelIndex() );
    emit dataChanged( top, bottom );
}

/*!
 * Finds an unused preset index
 */
int RadioStationModel::findUnusedPresetIndex()
{
    Q_D( RadioStationModel );
    QList<int> indexes;
    foreach( const RadioStation& station, d->mStations ) {
        if ( station.isValid() ) {
            indexes.append( station.presetIndex() );
        }
    }

    int index = 0;
    for ( ; indexes.contains( index ); ++index ) {
        // Nothing to do here
    }

    LOG_FORMAT( "RadioStationModelPrivate::findUnusedPresetIndex, index: %d", index );
    return index;
}

/*!
 * Used by the RDS data setters to find the correct station where the data is set
 */
RadioStation RadioStationModel::findCurrentStation( uint frequency )
{
    Q_D( RadioStationModel );
    RadioStation station = *d->mCurrentStation;
    if ( station.frequency() != frequency ) {
        if ( !findFrequency( frequency, station ) ) {
            return RadioStation();
        }
    }
    return station;
}
