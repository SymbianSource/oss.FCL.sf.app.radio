/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the CFMRadioDocument class, which
*                      handles application data serialization. Owns the Ui 
*                      (controller) and the engine (model)
*
*/


// INCLUDE FILES
#include "fmradiodocument.h"
#include "fmradioappui.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFMRadioDocument::CFMRadioDocument
// Constructor
// ---------------------------------------------------------
//
CFMRadioDocument::CFMRadioDocument( CEikApplication& aApp )
: CAknDocument( aApp )    
    {
    }

// ---------------------------------------------------------
// CFMRadioDocument::~CFMRadioDocument
// Class destructor
// ---------------------------------------------------------
//
CFMRadioDocument::~CFMRadioDocument()
    {
    }

// ---------------------------------------------------------
// CFMRadioDocument::NewL
// Two-phase constructor of CFMRadioDelayTimer
// ---------------------------------------------------------
//
CFMRadioDocument* CFMRadioDocument::NewL( CEikApplication& aApp )
    {
    CFMRadioDocument* self = new ( ELeave ) CFMRadioDocument( aApp );
    return self;
    }
    
// ----------------------------------------------------
// CFMRadioDocument::CreateAppUiL
// constructs CFMRadioAppUi
// ----------------------------------------------------
//
CEikAppUi* CFMRadioDocument::CreateAppUiL()
    {
    return new ( ELeave ) CFMRadioAppUi;
    }

// End of File  
