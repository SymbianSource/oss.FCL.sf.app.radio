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
* Description:  fmradio preset station information
*
*/

#include "fmradiopreset.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CFMRadioPreset::CFMRadioPreset
// Default constructor
// ----------------------------------------------------------------------------
//
CFMRadioPreset::CFMRadioPreset() : iNameIsValid( ETrue )
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioPreset::ConstructL
// ---------------------------------------------------------------------------
//
void CFMRadioPreset::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// CFMRadioPreset::NewL
// Two-phase constructor of CFMRadioPreset
// ---------------------------------------------------------------------------
//
CFMRadioPreset* CFMRadioPreset::NewL() 
    {
    CFMRadioPreset* self = new ( ELeave ) CFMRadioPreset;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CFMRadioPreset::~CFMRadioPreset
// Destructor
// ---------------------------------------------------------------------------
//
CFMRadioPreset::~CFMRadioPreset()
    {
    iStationName.Close();
    }

// ---------------------------------------------------------------------------
// CFMRadioPreset::SetPresetNameL
// ---------------------------------------------------------------------------
//
void CFMRadioPreset::SetPresetNameL( const TDesC& aName )
    {
    iStationName.Close();
    iStationName.CreateL( aName );
    }

// ---------------------------------------------------------------------------
// CFMRadioPreset::PresetName
// ---------------------------------------------------------------------------
//
TDesC& CFMRadioPreset::PresetName()
    {
    return iStationName;
    }

// ---------------------------------------------------------------------------
// CFMRadioPreset::SetPresetNameValid
// ---------------------------------------------------------------------------
//
void CFMRadioPreset::SetPresetNameValid( TBool aValid )
    {
    iNameIsValid = aValid;
    }

// ---------------------------------------------------------------------------
// CFMRadioPreset::PresetNameValid
// ---------------------------------------------------------------------------
//
TBool CFMRadioPreset::PresetNameValid()
    {
    return iNameIsValid;
    }
 
// ---------------------------------------------------------------------------
// CFMRadioPreset::SetPresetFrequency
// ---------------------------------------------------------------------------
//
void CFMRadioPreset::SetPresetFrequency( TInt aFrequency  )
    {
    iStationFrequency = aFrequency;
    }

// ---------------------------------------------------------------------------
// CFMRadioPreset::PresetFrequency
// ---------------------------------------------------------------------------
//
TInt CFMRadioPreset::PresetFrequency()
    {
    return iStationFrequency;
    }
