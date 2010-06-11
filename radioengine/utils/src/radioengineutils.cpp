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
#include <StringLoader.h>
#include <utf.h>

// User includes
#include "radioengineutils.h"
#include "cradioenginetls.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void RadioEngineUtils::InitializeL()
    {
    CRadioEngineTls::InitializeL();
    CRadioEngineTls::Instance().AddRef();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void RadioEngineUtils::Release()
    {
    CRadioEngineTls::Instance().Release();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C MRadioEngineLogger* RadioEngineUtils::Logger()
    {
    return CRadioEngineTls::Instance().Logger();
    }

// ---------------------------------------------------------------------------
// Returns the file server session
// ---------------------------------------------------------------------------
//
EXPORT_C RFs& RadioEngineUtils::FsSession()
    {
    return CRadioEngineTls::Instance().FsSession();
    }

// ---------------------------------------------------------------------------
// Utility method for frequency formatting.
// Frequency is assumed to be in kilohertz format.
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* RadioEngineUtils::ReadFrequencyStringLC( TUint32 aFreq,
                                                         TInt aDecimalCount,
                                                         TInt aResourceId )
    {
    TBuf<KDefaultRealWidth> freqText;
    freqText.AppendNum( static_cast<TReal>( aFreq ) / 1000.0f, TRealFormat( KDefaultRealWidth, aDecimalCount ) ); // Converts kilohertz to megahertz.

    // Converts the numbers to the proper display mode.

    HBufC* channelFreq = NULL;

    if ( aResourceId == KErrNotFound ) // No resource string.
        {
        channelFreq = freqText.AllocLC();
        }
    else
        {
        channelFreq = StringLoader::LoadLC( aResourceId, freqText );
        }

    return channelFreq;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void RadioEngineUtils::FormatFrequencyString( TDes& aDest,
                                                       TUint32 aFreq,
                                                       TInt aDecimalCount,
                                                       TDesC& aFormat )
    {
    TBuf<KDefaultRealWidth> freqText;
    freqText.AppendNum( static_cast<TReal>( aFreq ) / 1000.0f, TRealFormat( KDefaultRealWidth, aDecimalCount ) ); // Converts kilohertz to megahertz.

    // Converts the numbers to the proper display mode.

    if ( aFormat.Length() <= 0 || freqText.Length() > aDest.MaxLength() ) // No format.
        {
        aDest.Copy( freqText.Left( aDest.Length() ) );
        }
    else
        {
        StringLoader::Format( aDest, aFormat, KErrNotFound, freqText );
        }
    }

