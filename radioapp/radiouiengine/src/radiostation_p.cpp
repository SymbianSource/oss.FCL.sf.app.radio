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
#include "radiostation_p.h"

/*!
 *
 */
RadioStationPrivate::RadioStationPrivate( int presetIndex, uint frequency )
{
    init( presetIndex, frequency );

    if ( presetIndex == RadioStation::SharedNull ) {
        ref = 2;
    }
}

/*!
 *
 */
RadioStationPrivate::RadioStationPrivate( const RadioStationPrivate& other ) :
    QSharedData( other ),
    mPresetIndex( other.mPresetIndex ),
    mFrequency( other.mFrequency ),
    mName( other.mName ),
    mRenamedByUser( other.mRenamedByUser ),
    mGenre( other.mGenre ),
    mUrl( other.mUrl ),
    mPiCode( other.mPiCode ),
    mType( other.mType ),
    mPsType( other.mPsType ),
    mRadioText( other.mRadioText ),
    mDynamicPsText( other.mDynamicPsText ),
    mChangeFlags( other.mChangeFlags ),
    mCallSignCheckDone( other.mCallSignCheckDone ),
    mLastPsNameChangeTime( other.mLastPsNameChangeTime )
{
    // Protect the shared null preset index to make debugging easier
    if ( mPresetIndex == RadioStation::SharedNull ) {
        mPresetIndex = RadioStation::Invalid;
    }
}

/*!
 *
 */
RadioStationPrivate::~RadioStationPrivate()
{
}

/*!
 *
 */
void RadioStationPrivate::init( int presetIndex, uint frequency )
{
    mPresetIndex       = presetIndex;
    mFrequency         = frequency;
    mRenamedByUser     = false;
    mGenre             = -1;
    mPiCode            = -1;
    mType              = 0;
    mPsType            = RadioStation::Unknown;
    mChangeFlags       = RadioStation::NoChange;
    mCallSignCheckDone = false;
}

/*!
 * \reimp
 */
int RadioStationPrivate::presetIndex() const
{
    return mPresetIndex;
}

/*!
 * \reimp
 */
void RadioStationPrivate::setPresetIndex( int presetIndex )
{
    mPresetIndex = presetIndex;
}

/*!
 * \reimp
 */
uint RadioStationPrivate::frequency() const
{
    return mFrequency;
}

/*!
 * \reimp
 */
void RadioStationPrivate::setFrequency( uint frequency )
{
    mFrequency = frequency;
}

/*!
 * \reimp
 */
QString RadioStationPrivate::name() const
{
    return mName;
}

/*!
 * \reimp
 */
void RadioStationPrivate::setName( QString name )
{
    mName = name;
}

/*!
 * \reimp
 */
bool RadioStationPrivate::isRenamedByUser() const
{
    return mRenamedByUser;
}

/*!
 * \reimp
 */
void RadioStationPrivate::setRenamedByUser( bool renamed )
{
    mRenamedByUser = renamed;
}

/*!
 * \reimp
 */
int RadioStationPrivate::genre() const
{
    return mGenre;
}

/*!
 * \reimp
 */
void RadioStationPrivate::setGenre( int genre )
{
    mGenre = genre;
}

/*!
 * \reimp
 */
QString RadioStationPrivate::url() const
{
    return mUrl;
}

/*!
 * \reimp
 */
void RadioStationPrivate::setUrl( QString url )
{
    mUrl = url;
}

/*!
 * \reimp
 */
int RadioStationPrivate::piCode() const
{
    return mPiCode;
}

/*!
 * \reimp
 */
void RadioStationPrivate::setPiCode( int piCode )
{
    mPiCode = piCode;
}

/*!
 * \reimp
 */
bool RadioStationPrivate::isFavorite() const
{
    return mType.testFlag( RadioStation::Favorite );
}

/*!
 * \reimp
 */
void RadioStationPrivate::setFavorite( bool favorite )
{
    if ( favorite ) {
        mType |= RadioStation::Favorite;
    } else {
        mType &= ~RadioStation::Favorite;
    }
}

/*!
 * \reimp
 */
bool RadioStationPrivate::isLocalStation() const
{
    return mType.testFlag( RadioStation::LocalStation );
}

/*!
 * \reimp
 */
void RadioStationPrivate::setLocalStation( bool localStation )
{
    if ( localStation ) {
        mType |= RadioStation::LocalStation;
    } else {
        mType &= ~RadioStation::LocalStation;
    }
}
