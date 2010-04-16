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
#ifdef COMPILE_WITH_NEW_PRESET_UTILITY
#include <presetutility.h>
#include <preset.h>
#else
#   include <RadioFmPresetUtility.h>
#endif // COMPILE_WITH_NEW_PRESET_UTILITY

#include <QString>

// User includes
#include "radiopresetstorage.h"
#include "radiopresetstorage_p.h"
#include "radiostationif.h"

/*!
 * Converts a symbian descriptor to Qt string
 */
static QString convertString( const TDesC& aDesc )
{
    return QString( (QChar*)aDesc.Ptr(), aDesc.Length() );
}

/*!
 *
 */
RadioPresetStorage::RadioPresetStorage() :
    d_ptr( new RadioPresetStoragePrivate() )
{
    Q_D( RadioPresetStorage );
    d->init();
}

/*!
 *
 */
RadioPresetStorage::~RadioPresetStorage()
{
}

/*!
 *
 */
int RadioPresetStorage::maxNumberOfPresets() const
{
    Q_D( const RadioPresetStorage );
#ifdef COMPILE_WITH_NEW_PRESET_UTILITY
    return d->mPresetUtility->MaxNumberOfPresets();
#else
    TInt maxPresets = 0;
    d->mPresetUtility->GetMaxNumberOfPresets( maxPresets );
    return maxPresets;
#endif // COMPILE_WITH_NEW_PRESET_UTILITY
}

/*!
 *
 */
int RadioPresetStorage::presetCount() const
{
    Q_D( const RadioPresetStorage );
#ifdef COMPILE_WITH_NEW_PRESET_UTILITY
    return d->mPresetUtility->PresetCount();
#else
    TInt presetCount = 0;
    d->mPresetUtility->GetNumberOfPresets( presetCount );
    return presetCount;
#endif // COMPILE_WITH_NEW_PRESET_UTILITY
}

/*!
 *
 */
int RadioPresetStorage::firstPreset() const
{
    Q_D( const RadioPresetStorage );
#ifdef COMPILE_WITH_NEW_PRESET_UTILITY
    return d->mPresetUtility->FirstPreset();
#else
    TInt firstIndex = -1;
    TRAPD( err, d->mPresetUtility->GetFirstPresetL( firstIndex ) );
    if ( err ) {
        firstIndex = -1;
    }
    return firstIndex;
#endif // COMPILE_WITH_NEW_PRESET_UTILITY
}

/*!
 *
 */
int RadioPresetStorage::nextPreset( int fromIndex ) const
{
    Q_D( const RadioPresetStorage );
#ifdef COMPILE_WITH_NEW_PRESET_UTILITY
    return d->mPresetUtility->NextPreset( fromIndex );
#else
    TInt nextIndex = -1;
    TRAPD( err, d->mPresetUtility->GetNextPresetL( fromIndex, nextIndex ) );
    if ( err ) {
        nextIndex = -1;
    }
    return nextIndex;
#endif // COMPILE_WITH_NEW_PRESET_UTILITY
}

/*!
 *
 */
bool RadioPresetStorage::deletePreset( int presetIndex )
{
    Q_D( RadioPresetStorage );
#ifdef COMPILE_WITH_NEW_PRESET_UTILITY
    return d->mPresetUtility->DeletePreset( presetIndex ) == KErrNone;
#else
    TRAPD( err, d->mPresetUtility->DeletePresetL( presetIndex ) );
    return err;
#endif // COMPILE_WITH_NEW_PRESET_UTILITY
}

/*!
 *
 */
bool RadioPresetStorage::savePreset( const RadioStationIf& station )
{
    Q_D( RadioPresetStorage );
#ifdef COMPILE_WITH_NEW_PRESET_UTILITY
    TPreset preset;
    preset.SetFrequency( station.frequency() );
    TPresetName name( station.name().utf16() );
    preset.SetName( name );
    preset.SetRenamedByUser( station.isRenamedByUser() );
    preset.SetGenre( station.genre() );
    TRadioUrl url( station.url().utf16() );
    preset.SetUrl( url );
    preset.SetPiCode( station.piCode() );
    preset.SetFavorite( station.isFavorite() );
    preset.SetLocalStation( station.isLocalStation() );

    TRAPD( err, d->mPresetUtility->SavePresetL( preset, station.presetIndex() ) );
    return err == KErrNone;
#else
    TFmPresetName name( station.name().utf16() );
    TRAPD( err, d->mPresetUtility->SetPresetL( station.presetIndex(), name, station.frequency() ) );
    return err == KErrNone;
#endif // COMPILE_WITH_NEW_PRESET_UTILITY
}

/*!
 *
 */
bool RadioPresetStorage::readPreset( int index, RadioStationIf& station )
{
    Q_D( RadioPresetStorage );
#ifdef COMPILE_WITH_NEW_PRESET_UTILITY
    TPreset preset;
    TRAPD( err, d->mPresetUtility->ReadPresetL( index, preset ) );
    if ( !err ) {

        station.setPresetIndex( index );
        station.setFrequency( preset.Frequency() );
        station.setName( convertString( preset.Name() ) );
        station.setRenamedByUser( preset.RenamedByUser() );
        station.setGenre( preset.Genre() );
        station.setUrl( convertString( preset.Url() ) );
        station.setPiCode( preset.PiCode() );
        station.setFavorite( preset.Favorite() );
        station.setLocalStation( preset.LocalStation() );

        return true;
    }
    return false;
#else
    TFmPresetName nameDesc;
    TInt frequency = 0;
    TRAPD( err, d->mPresetUtility->GetPresetL( index, nameDesc, frequency ) );
    if ( !err )
    {
        station.setPresetIndex( index );
        station.setName( convertString( nameDesc ) );
        station.setFrequency( static_cast<TUint>( frequency ) );
        station.setLocalStation( true );
    }
    return err == KErrNone;
#endif // COMPILE_WITH_NEW_PRESET_UTILITY
}

/*!
 *
 */
RadioPresetStoragePrivate::RadioPresetStoragePrivate()
{
}

/*!
 *
 */
RadioPresetStoragePrivate::~RadioPresetStoragePrivate()
{
}

/*!
 *
 */
bool RadioPresetStoragePrivate::init()
{
#ifdef COMPILE_WITH_NEW_PRESET_UTILITY
    TRAPD( err, mPresetUtility.reset( CPresetUtility::NewL() ) );
    
#else
    TRAPD( err, mPresetUtility.reset( CRadioFmPresetUtility::NewL( *this ) ) );
#endif // COMPILE_WITH_NEW_PRESET_UTILITY
    return err == KErrNone;
}
