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
#include <HbEffect>

// User includes
#include "radiouiutilities.h"
#include "radiologger.h"

// Constants

/*!
 *
 */
bool RadioUiUtilities::addEffects( QEffectList list )
{
    bool allAvailable = true;
    QEffectList added;
    foreach ( EffectInfo info, list ) {
#ifdef USE_LAYOUT_FROM_E_DRIVE
        info.mPath.replace( QString( ":/" ), QString( "e:/radiotest/" ) );
#endif
        if ( HbEffect::add( info.mItem, info.mPath, info.mEvent ) ) {
            added.append( info );
        } else {
            allAvailable = false;
            break;
        }
    }

    if ( !allAvailable ) {
        foreach ( const EffectInfo& info, added ) {
            HbEffect::remove( info.mItem, info.mPath, info.mEvent );
        }
    }

    return allAvailable;
}
