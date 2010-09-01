/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: provides information about FM region
*
*/


// INCLUDE FILES
#include <barsread.h>
#include "fmradioregion.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CFMRadioRegion::CFMRadioRegion()
    {
    }

// EPOC default constructor can leave.
void CFMRadioRegion::ConstructL(TResourceReader& aRr)
    {
	iId = static_cast<TFMRadioRegionSetting>(aRr.ReadUint16());
	iStepSize = static_cast<TUint16>( aRr.ReadUint16() );
	iMinFreq = aRr.ReadUint32();
	iMaxFreq = aRr.ReadUint32();
	iDecimalCount = aRr.ReadInt16();

	TInt countryCodeCount = aRr.ReadInt16();
	for(TInt i = 0 ; i < countryCodeCount ; i++)
		{
		TPtrC code;
		code.Set(aRr.ReadTPtrC());
		User::LeaveIfError(iCountryCodes.Append(code.AllocL()));
		}

	iName = aRr.ReadHBufC16L();
	iSettingName = aRr.ReadHBufC16L();
    }

// Two-phased constructor.
CFMRadioRegion* CFMRadioRegion::NewL(TResourceReader& aRr)
    {
    CFMRadioRegion* self = new (ELeave) CFMRadioRegion;
    
    CleanupStack::PushL(self);
    self->ConstructL(aRr);
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CFMRadioRegion::~CFMRadioRegion()
    {
    iCountryCodes.ResetAndDestroy();
	iCountryCodes.Close();
	delete iName;
	delete iSettingName;
    }

// ---------------------------------------------------------
// CFMRadioRegion::Id
// Get the id of region
// ---------------------------------------------------------
//
TFMRadioRegionSetting CFMRadioRegion::Id() const
	{
	return iId;
	}

// ---------------------------------------------------------
// CFMRadioRegion::StepSize
// Get the step interval of region
// ---------------------------------------------------------
//
TUint32 CFMRadioRegion::StepSize() const
	{
	return iStepSize;
	}

// ---------------------------------------------------------
// CFMRadioRegion::MinFreq
// ---------------------------------------------------------
//
TUint32 CFMRadioRegion::MinFrequency() const
	{
	return iMinFreq;
	}

// ---------------------------------------------------------
// CFMRadioRegion::MaxFreq
// ---------------------------------------------------------
//
TUint32 CFMRadioRegion::MaxFrequency() const
	{
	return iMaxFreq;
	}
	
// ---------------------------------------------------------
// CFMRadioRegion::DecimalCount
// ---------------------------------------------------------
//
TInt CFMRadioRegion::DecimalCount() const
	{
	return iDecimalCount;
	}

// ---------------------------------------------------------
// CFMRadioRegion::CountryCode
// Get the country code of region
// ---------------------------------------------------------
//
const RFMRadioCountryCodeArray& CFMRadioRegion::CountryCodes()
	{
	return iCountryCodes;
	}

// ---------------------------------------------------------
// CFMRadioRegion::Name
// Get the name of region
// ---------------------------------------------------------
//
const TDesC16& CFMRadioRegion::Name() const
	{
	return *iName;
	}

// ---------------------------------------------------------
// CFMRadioRegion::SettingItemName
// Get the name of region
// ---------------------------------------------------------
//
const TDesC16& CFMRadioRegion::SettingItemName() const
	{
	return *iSettingName;
	}

//  End of File  
