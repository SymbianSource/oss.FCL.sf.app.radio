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
#include <QTime>

// User includes
#include "radiostation.h"
#include "radiostation_p.h"
#include "radiologger.h"
#include "radio_global.h"

// Constants
const QString KTagArtist = "artist";
const QString KTagTitle = "title";
const QString KLinkArtist = "<a href=\"" + KTagArtist + "\">";
const QString KLinkTitle = "<a href=\"" + KTagTitle + "\">";
const QString KLinkClose = "</a>";

const char* callSign[KThreeLetterCallSignCount] =
   {"KBW", "KCY", "KDB", "KDF", "KEX", "KFH","KFI","KGA","KGB",
    "KGO", "KGU", "KGW", "KGY", "KHQ", "KID", "KIT", "KJR", "KLO",
    "KLZ", "KMA", "KMJ", "KNX", "KOA", "KOB", "KOY", "KPQ", "KQV",
    "KSD", "KSL", "KUJ", "KUT", "KVI", "KWG", "KXL", "KXO", "KYW",
    "WBT", "WBZ", "WDZ", "WEW", "WGH", "WGL", "WGN", "WGR", "WGY",
    "WHA", "WHB", "WHK", "WHO", "WHP", "WIL", "WIP", "WIS", "WJR",
    "WJW", "WJZ", "WKY", "WLS", "WLW", "WMC", "WMT", "WOC", "WOI",
    "WOL", "WOR", "WOW", "WRC", "WRR", "WSB", "WSM", "WWJ", "WWL"};

const uint piCode[KThreeLetterCallSignCount] =
   {0x99A5, 0x99A6, 0x9990, 0x99A7, 0x9950, 0x9951, 0x9952, 0x9953,
    0x9991, 0x9954, 0x9955, 0x9956, 0x9957, 0x99AA, 0x9958, 0x9959,
    0x995A, 0x995B, 0x995C, 0x995D, 0x995E, 0x995F, 0x9960, 0x99AB,
    0x9992, 0x9993, 0x9964, 0x9994, 0x9965, 0x9966, 0x9995, 0x9967,
    0x9968, 0x9996, 0x9997, 0x996B, 0x9999, 0x996D, 0x996E, 0x996F,
    0x999A, 0x9971, 0x9972, 0x9973, 0x999B, 0x9975, 0x9976, 0x9977,
    0x9978, 0x999C, 0x999D, 0x997A, 0x99B3, 0x997B, 0x99B4, 0x99B5,
    0x997C, 0x997D, 0x997E, 0x999E, 0x999F, 0x9981, 0x99A0, 0x9983,
    0x9984, 0x99A1, 0x99B9, 0x99A2, 0x99A3, 0x99A4, 0x9988, 0x9989};

const uint KDisableLocalAreaCoverageMask = 0x0800;

const int KPsNameChangeThresholdSeconds = 10;

/**
 * Static shared data instance that is used by all default-constructed RadioStation instances
 */
Q_GLOBAL_STATIC_WITH_ARGS( RadioStationPrivate, shared_null, ( RadioStation::SharedNull ) )

/*!
 *
 */
QString RadioStation::parseFrequency( uint frequency )
{
    QString freqString;
    freqString.sprintf( "%.2f", qreal( frequency ) / KFrequencyMultiplier );
    return freqString;
}

/*!
 *
 */
RadioStation::RadioStation() :
    QObject( 0 )
{
    mData = shared_null();
    mData->ref.ref();
}

/*!
 *
 */
RadioStation::RadioStation( const RadioStation& other ) :
    QObject( 0 )
{
    mData = other.mData;
    mData->ref.ref();
}

/*!
 *
 */
RadioStation::RadioStation( int presetIndex, uint frequency ) :
    QObject( 0 )
{
    mData = new RadioStationPrivate( presetIndex, frequency );
}

/*!
 *
 */
RadioStation::~RadioStation()
{
    decrementReferenceCount();
}

/*!
 *
 */
RadioStation& RadioStation::operator=( const RadioStation& other )
{
    qAtomicAssign( mData, other.mData );
    return *this;
}

/*!
 *
 */
void RadioStation::reset()
{
    decrementReferenceCount();
    mData = shared_null();
    mData->ref.ref();
}

/*!
 *
 */
void RadioStation::setChangeFlags( RadioStation::Change flags )
{
    if ( mData->mChangeFlags != flags ) {
        detach();
        mData->mChangeFlags = flags;
    }
}

/*!
 *
 */
void RadioStation::setPresetIndex( int presetIndex )
{
    if ( mData->mPresetIndex != presetIndex ) {
        detach();
        mData->mPresetIndex = presetIndex;
        mData->mChangeFlags |= RadioStation::PersistentDataChanged;
    }
}

/*!
 *
 */
void RadioStation::setFrequency( uint frequency )
{
    if ( mData->mFrequency != frequency ) {
        detach();
        mData->mFrequency = frequency;
        mData->mChangeFlags |= RadioStation::PersistentDataChanged;
    }
}

/*!
 * Sets the preset name
 */
void RadioStation::setName( const QString& name )
{
    // Name emptiness is checked because this name setter is used by incoming RDS PS name
    // and empty names should be ignored
    if ( !name.isEmpty() && mData->mName.compare( name ) != 0 ) {
        detach();
        mData->mName = name.trimmed();
        mData->mChangeFlags |= RadioStation::PersistentDataChanged | RadioStation::NameChanged;

        // Save the time when PS name changed and if the last change was too close to the current time
        // change the PS type to dynamic if it has already been incorrectly determined to be static.
        QTime previousChange = mData->mLastPsNameChangeTime;
        mData->mLastPsNameChangeTime = QTime::currentTime();
        if ( previousChange.isValid() && mData->mPsType == RadioStation::Static &&
             previousChange.secsTo( mData->mLastPsNameChangeTime ) < KPsNameChangeThresholdSeconds ) {
            LOG( "Station changed PS name too often. PS type changed to Dynamic" );
            mData->mPsType = RadioStation::Dynamic;
            mData->mDynamicPsText = mData->mName;
            mData->mName = "";
            mData->mChangeFlags |= RadioStation::PsTypeChanged | RadioStation::DynamicPsChanged;
            mData->mCallSignCheckDone = false;
        }

        //TODO: This is a temporary thing to see some URL. Remove this
        if ( !mData->mName.isEmpty() ) {
            mData->mUrl = "<a href=\"buu\">www." + mData->mName.toLower() + ".fi</a>";
        } else {
            mData->mUrl = "";
        }
        mData->mChangeFlags |= RadioStation::UrlChanged;
    }
}

/*!
 *
 */
void RadioStation::setGenre( const int genre )
{
    if ( mData->mGenre != genre ) {
        detach();
        mData->mGenre = genre;
        mData->mChangeFlags |= RadioStation::PersistentDataChanged | RadioStation::GenreChanged;
    }
}

/*!
 *
 */
void RadioStation::setUrl( const QString& url )
{
    if ( mData->mUrl.compare( url ) != 0 ) {
        detach();
        mData->mUrl = url;
        mData->mChangeFlags |= RadioStation::PersistentDataChanged | RadioStation::UrlChanged;
    }
}

/*!
 * Sets the PI code
 * @return true if code was changed, false if not
 */
bool RadioStation::setPiCode( int piCode, RadioRegion::Region region )
{
    LOG_FORMAT( "RadioStation::setPiCode new PI: %d", piCode );
    // toggling local area coverage bit code must not be interpreted as new PI code
    if( region != RadioRegion::America )
    {
        piCode &= ~KDisableLocalAreaCoverageMask;
    }

    LOG_FORMAT( "stored PI: %d", mData->mPiCode );
    LOG_FORMAT( "call sign check done: %d", mData->mCallSignCheckDone );
    //prevent executing the below code when unnessesary
    if ( mData->mPiCode != piCode || !mData->mCallSignCheckDone )
	{
        detach();
        mData->mPiCode = piCode;
        mData->mChangeFlags |= RadioStation::PersistentDataChanged | RadioStation::PiCodeChanged;
        // call sign not calculated for clear channel stations
		//TODO: Remove magic numbers
        if( ( (mData->mPiCode & 0xF000 ) >> 12 ) == 0x1 )
        {
            LOG( "Clear channel station" );
            mData->mCallSignCheckDone = true;
        }
		// if America region, not PS name received and not user renamed station
        else if ( region == RadioRegion::America && mData->mName.isEmpty() && !isRenamed() )
        {
            LOG( "Calculate call sign" );
            mData->mName = piCodeToCallSign( mData->mPiCode );
            mData->mChangeFlags |= RadioStation::NameChanged;
        }

        if ( mData->mChangeFlags.testFlag( RadioStation::PsTypeChanged ) )
		{
            LOG( "Call sign check done" );
            mData->mCallSignCheckDone = true;
        }

        return true;
    }
    return false;
}

/*!
 *
 */
void RadioStation::setPsType( PsType psType )
{
    if ( mData->mPsType != psType ) {
        detach();
        mData->mPsType = psType;
        mData->mChangeFlags |= RadioStation::PsTypeChanged;
    }
}

/*!
 *
 */
void RadioStation::setRadioText( const QString& radioText )
{
    if ( mData->mRadioText.compare( radioText ) != 0 ) {
        detach();
        mData->mRadioText = radioText.isEmpty() ? "" : radioText.trimmed();
        mData->mChangeFlags |= RadioStation::RadioTextChanged;
    }
}

/*!
 *
 */
void RadioStation::setRadioTextPlus( const int rtPlusClass, const QString& rtPlusItem )
{
    if ( !mData->mRadioText.isEmpty() &&
         !rtPlusItem.isEmpty() &&
         ( rtPlusClass == RtPlus::Artist || rtPlusClass == RtPlus::Title || rtPlusClass == RtPlus::Homepage) )
    {
        // Url is saved to its own variable and it is not highlighted from the radiotext
        if ( rtPlusClass == RtPlus::Homepage ) {
            setUrl( rtPlusItem );
            return;
        }

        detach();
        QString replacement = "";
        if ( rtPlusClass == RtPlus::Artist ) {
            replacement = KLinkArtist;
        } else if ( rtPlusClass == RtPlus::Title ) {
            replacement = KLinkTitle;
        }
        replacement += rtPlusItem + KLinkClose;

        mData->mRadioText.replace( rtPlusItem, replacement );
        mData->mChangeFlags |= RadioStation::RadioTextChanged;
    }
}

/*!
 *
 */
void RadioStation::setDynamicPsText( const QString& dynamicPsText )
{
    if ( mData->mDynamicPsText.compare( dynamicPsText ) != 0 ) {
        detach();
        mData->mDynamicPsText = dynamicPsText;
        mData->mChangeFlags |= RadioStation::DynamicPsChanged;
    }
}

/*!
 *
 */
bool RadioStation::isValid() const
{
    return mData->mPresetIndex >= 0 && mData->mFrequency > 0;
}

/*!
 *
 */
QString RadioStation::name() const
{
    return mData->mName;
}

/*!
 *
 */
void RadioStation::setUserDefinedName( const QString& name )
{
    // We don't check for name emptiness because this setter is used also to remove the renaming
    // of a station by setting an empty name
    if ( mData->mName.compare( name ) != 0 ) {
        detach();
        mData->mName = name;
        mData->mRenamedByUser = !name.isEmpty();
        mData->mChangeFlags |= RadioStation::PersistentDataChanged | RadioStation::NameChanged;
    }
}

/*!
 *
 */
bool RadioStation::isRenamed() const
{
    return mData->mRenamedByUser;
}

/*!
 *
 */
int RadioStation::genre() const
{
    return mData->mGenre;
}

/*!
 *
 */
QString RadioStation::frequencyMhz() const
{
    return parseFrequency( mData->mFrequency );
}

/*!
 *
 */
uint RadioStation::frequency() const
{
    return mData->mFrequency;
}

/*!
 *
 */
int RadioStation::presetIndex() const
{
    return mData->mPresetIndex;
}

/*!
 *
 */
void RadioStation::setFavorite( bool favorite )
{
    if ( isFavorite() != favorite ) {
        detach();
        favorite ? setType( RadioStation::Favorite ) : unsetType( RadioStation::Favorite );
        mData->mChangeFlags |= RadioStation::PersistentDataChanged | RadioStation::FavoriteChanged;
    }
}

/*!
 *
 */
bool RadioStation::isFavorite() const
{
    return mData->mType.testFlag( RadioStation::Favorite );
}

/*!
 *
 */
QString RadioStation::url() const
{
    return mData->mUrl;
}

/*!
 *
 */
bool RadioStation::hasPiCode() const
{
    return mData->mPiCode != -1;
}

/*!
 *
 */
bool RadioStation::hasRds() const
{
    return hasPiCode() ||
            mData->mGenre != -1 ||
            !mData->mDynamicPsText.isEmpty() ||
            !mData->mRadioText.isEmpty() ||
            ( !mData->mName.isEmpty() && !isRenamed() );
}

/*!
 *
 */
void RadioStation::setType( RadioStation::Type type )
{
    if ( !isType( type ) ) {
        detach();

        // Check if favorite-status changed
        if ( mData->mType.testFlag( RadioStation::Favorite ) != type.testFlag( RadioStation::Favorite ) ) {
            mData->mChangeFlags |= RadioStation::FavoriteChanged;
        }

        mData->mType |= type;
        mData->mChangeFlags |= RadioStation::PersistentDataChanged | RadioStation::TypeChanged;
    }
}

/*!
 *
 */
void RadioStation::unsetType( RadioStation::Type type )
{
    if ( isType( type ) ) {
        detach();

        // Check if favorite-status changed
        if ( mData->mType.testFlag( RadioStation::Favorite ) != type.testFlag( RadioStation::Favorite ) ) {
            mData->mChangeFlags |= RadioStation::FavoriteChanged;
        }

        mData->mType &= ~type;
        mData->mChangeFlags |= RadioStation::PersistentDataChanged | RadioStation::TypeChanged;
    }
}

/*!
 *
 */
bool RadioStation::isType( RadioStation::Type type ) const
{
    return ( mData->mType & type ) == type;
}

/*!
 *
 */
RadioStation::PsType RadioStation::psType() const
{
    return mData->mPsType;
}

/*!
 *
 */
QString RadioStation::radioText() const
{
    return mData->mRadioText;
}

/*!
 *
 */
QString RadioStation::dynamicPsText() const
{
    return mData->mDynamicPsText;
}

/*!
 *
 */
RadioStation::Change RadioStation::changeFlags() const
{
    return mData->mChangeFlags;
}

/*!
 *
 */
bool RadioStation::hasDataChanged( RadioStation::Change flags ) const
{
    return ( mData->mChangeFlags & flags ) == flags;
}

/*!
 *
 */
bool RadioStation::hasChanged() const
{
    return mData->mChangeFlags != RadioStation::NoChange;
}

/*!
 *
 */
void RadioStation::resetChangeFlags()
{
    if ( mData->mChangeFlags != RadioStation::NoChange ) {
        detach();
        mData->mChangeFlags = RadioStation::NoChange;
    }
}

/**
 * Decrements the reference count of the implicitly shared data.
 */
void RadioStation::decrementReferenceCount()
{
    if ( !mData->ref.deref() ) {
        delete mData;
        mData = 0;
    }
}

/*!
 *
 */
 QString RadioStation::piCodeToCallSign( uint programmeIdentification )
 {
    QString callSign;

    LOG_FORMAT( "RadioStation::piCodeToCallSign PI: %d", programmeIdentification );
    // call signs beginning with 'K'
    if( ( programmeIdentification>=KKxxxCallSignPiFirst ) && ( programmeIdentification < KWxxxCallSignPiFirst ) ) {
        callSign += "K";
        callSign += iterateCallSign( KKxxxCallSignPiFirst, programmeIdentification );
    }
    // call signs beginning with 'W'
    else if (( programmeIdentification >= KWxxxCallSignPiFirst ) && ( programmeIdentification <= KWxxxCallSignPiLast )) {
        callSign += "W";
        callSign += iterateCallSign( KWxxxCallSignPiFirst, programmeIdentification );
    }
    // 3 letter only call signs
    else if(( programmeIdentification >= KxxxCallSignPiFirst ) && ( programmeIdentification <= KxxxCallSignPiLast)) {
        callSign += callSignString( programmeIdentification );
    }
    else
    {
        LOG( "RadioStation::piCodeToCallSign - Unhandled else" );
    }

    LOG_FORMAT( "RadioStation::piCodeToCallSign, call sign: %s", GETSTRING(callSign) );

    return callSign;
}

/*!
 *
 */
QString RadioStation::iterateCallSign( int piBase, int programmeIdentification )
{
    QString callSign;
    LOG_FORMAT( "RadioStation::iterateCallSign base: %d", piBase );

    int sum(0), i(0);

    while( sum < programmeIdentification ) {
        i++;
        sum = piBase + i * 676 + 0 + 0;
    }
    callSign += callSignChar( i - 1 );

    int tmpSum( sum - 676 );
    sum -= 676;
    i = 0;
    while( sum <= programmeIdentification ) {
        i++;
        sum = tmpSum + 0 + i * 26 + 0;
    }
    callSign += callSignChar( i - 1 );

    tmpSum = sum - 26;
    sum -= 26;
    i = 0;
    while( sum <= programmeIdentification ) {
        i++;
        sum = tmpSum + 0 + 0 + i;
    }
    callSign += callSignChar( i - 1 );

    return callSign;
    }

/*!
 *
 */
QString RadioStation::callSignString( uint programmeIdentification )
{
    for ( uint i = 0; i < KThreeLetterCallSignCount; ++i ) {
        if( piCode[i] == programmeIdentification ) {
            return callSign[i];
        }
    }

    LOG_FORMAT( "RadioStation::callSignString, Not found PI: %d", programmeIdentification );

    return QString("????");
}

/*!
 *
 */
char RadioStation::callSignChar( uint decimalValue )
{
    LOG_FORMAT( "RadioStation::callSignChar A+: %d", decimalValue );
    if ( decimalValue <= KLastCallSignCharCode ) {
        return static_cast<char>( 'A' + decimalValue );
    }
    return '?';
}

/**
 * Detach from the implicitly shared data
 */
void RadioStation::detach()
{
    if ( !isDetached() ) {
        RadioStationPrivate* newData = new RadioStationPrivate( *mData );

        decrementReferenceCount();

        newData->ref = 1;
        mData = newData;

        // The shared null instance of the data has its preset index set to -200 (RadioStation::SharedNull).
        // We change the preset index of the detached data to -100 (RadioStation::Invalid) just to ease
        // debugging. This guarantees that the only instance that has value -200 is the actual shared null.
        #ifdef _DEBUG
        if ( mData->mPresetIndex == RadioStation::SharedNull ) {
            mData->mPresetIndex = RadioStation::Invalid;
        }
        #endif
    }
}

/**
 * Checks if the class is detached from implicitly shared data
 */
bool RadioStation::isDetached() const
{
    return mData->ref == 1;
}
