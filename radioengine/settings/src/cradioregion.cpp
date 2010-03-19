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
#include <barsread.h>

// User includes
#include "cradioregion.h"

// Used to convert kilohertz values to hertz values
const TInt KRadioThousand = 1000;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRegion::CRadioRegion()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRegion::ConstructL( TResourceReader& aRr )
    {
    iId = static_cast<TRadioRegion>( aRr.ReadUint16());
    iStepSize = static_cast<TUint16>( aRr.ReadUint16() ) * KRadioThousand;
    iMinFreq = aRr.ReadUint32() * KRadioThousand;
    iMaxFreq = aRr.ReadUint32() * KRadioThousand;
    iDecimalCount = aRr.ReadInt16();

    TInt countryCodeCount = aRr.ReadInt16();
    for ( TInt i = 0 ; i < countryCodeCount ; i++)
        {
        TPtrC code;
        code.Set( aRr.ReadTPtrC());
        User::LeaveIfError( iCountryCodes.Append( code.AllocL()));
        }

    iName = aRr.ReadHBufC16L();
    iSettingName = aRr.ReadHBufC16L();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRegion* CRadioRegion::NewL( TResourceReader& aRr )
    {
    CRadioRegion* self = new ( ELeave ) CRadioRegion;

    CleanupStack::PushL( self );
    self->ConstructL( aRr );
    CleanupStack::Pop();

    return self;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRegion::~CRadioRegion()
    {
    iCountryCodes.ResetAndDestroy();
    iCountryCodes.Close();
    delete iName;
    delete iSettingName;
    }

// ---------------------------------------------------------
// Get the id of region
// ---------------------------------------------------------
//
EXPORT_C TRadioRegion CRadioRegion::Id() const
    {
    return iId;
    }

// ---------------------------------------------------------
// Get the step interval of region
// ---------------------------------------------------------
//
EXPORT_C TUint32 CRadioRegion::StepSize() const
    {
    return iStepSize;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C TUint32 CRadioRegion::MinFrequency() const
    {
    return iMinFreq;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C TUint32 CRadioRegion::MaxFrequency() const
    {
    return iMaxFreq;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C TInt CRadioRegion::DecimalCount() const
    {
    return iDecimalCount;
    }

// ---------------------------------------------------------
// Get the country code of region
// ---------------------------------------------------------
//
EXPORT_C const RRadioCountryCodeArray& CRadioRegion::CountryCodes()
    {
    return iCountryCodes;
    }

// ---------------------------------------------------------
// Get the name of region
// ---------------------------------------------------------
//
EXPORT_C const TDesC16& CRadioRegion::Name() const
    {
    return *iName;
    }

// ---------------------------------------------------------
// Get the name of region
// ---------------------------------------------------------
//
EXPORT_C const TDesC16& CRadioRegion::SettingItemName() const
    {
    return *iSettingName;
    }
