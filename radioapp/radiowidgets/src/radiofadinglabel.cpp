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
#include "radiofadinglabel.h"
#include "radiologger.h"
#include "radiouiutilities.h"

// Constants
const QString KHideEffect = "hide";
const QString KShowEffect = "show";

#ifdef USE_DEPRECATED_ORBIT_APIS
#   define SET_CONTENT setText
#   define GET_CONTENT text
#else
#   define SET_CONTENT setHtml
#   define GET_CONTENT html
#endif // USE_DEPRECATED_ORBIT_APIS

/*!
 *
 */
RadioFadingLabel::RadioFadingLabel( QGraphicsItem* parent ) :
    HbLabel( parent ),
    mFadingEnabled( false )
{
    QEffectList effectList;
//    effectList.append( EffectInfo( this, ":/effects/fade_in.fxml", KShowEffect ) );
//    effectList.append( EffectInfo( this, ":/effects/fade_out.fxml", KHideEffect ) );
    RadioUiUtilities::addEffects( effectList );
}

/*!
 *
 */
void RadioFadingLabel::setFadingEnabled( bool fading )
{
    mFadingEnabled = fading;
}

/*!
 *
 */
void RadioFadingLabel::setTextWithoutFading( const QString& newText )
{
    HbLabel::SET_CONTENT( trimHtml( newText ) );
    if ( newText.isEmpty() && mFadingEnabled ) {
        setOpacity( 0.0 );
    }
}

/*!
 * Public slot
 *
 */
void RadioFadingLabel::setText( const QString& newText )
{
    if( GET_CONTENT().compare( newText ) != 0 &&                   // Text is different
            parentItem() && parentItem()->isVisible() &&    // Parent is visible
            mFadingEnabled ) {                              // Fading is enabled
        if ( newText.isEmpty() ) {
            if ( isVisible() ) {
                mTextHolder = "";
                startEffect( KHideEffect, "effectFinished" );
            }
        } else {
            if ( GET_CONTENT().isEmpty() ) {
                HbLabel::SET_CONTENT( trimHtml( newText ) );
                startEffect( KShowEffect );
            } else {
                mTextHolder = newText;
                startEffect( KHideEffect, "effectFinished" );
            }
        }
    } else {
        HbLabel::SET_CONTENT( trimHtml( newText ) );
    }
}

/*!
 * Private slot
 *
 */
void RadioFadingLabel::effectFinished( HbEffect::EffectStatus status )
{
    if ( status.reason == Hb::EffectFinished ) {
        HbLabel::SET_CONTENT( trimHtml( mTextHolder ) );
        if ( !mTextHolder.isEmpty() ) {
            HbEffect::start( this, KShowEffect );
        }
        mTextHolder = "";
    }
}

/*!
 *
 */
void RadioFadingLabel::startEffect( const QString& effectName, const char* slot )
{
    if ( HbEffect::effectRunning( this ) ) {
        HbEffect::cancel( this );
    }
    if ( slot ) {
        HbEffect::start( this, effectName, this, slot );
    } else {
        HbEffect::start( this, effectName );
    }
}

/*!
 *
 */
QString RadioFadingLabel::trimHtml( const QString& text )
{
#ifdef USE_DEPRECATED_ORBIT_APIS
    return text;
#else
    return "<font color=\"white\">" + text + "</font>";
#endif // USE_DEPRECATED_ORBIT_APIS
}
