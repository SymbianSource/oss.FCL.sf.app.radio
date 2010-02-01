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
* Description:  fmradio media idle controller
*
*/

#include "fmradioidlecontroller.h"
#include "fmradioidlecontrolinterface.h"
#include "debug.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CFMRadioIdleController::CFMRadioIdleController
// Default constructor
// ----------------------------------------------------------------------------
//
CFMRadioIdleController::CFMRadioIdleController()
    {
    }

// ---------------------------------------------------------------------------
// CFMRadioIdleController::ConstructL
// ---------------------------------------------------------------------------
//
void CFMRadioIdleController::ConstructL()
    {
    FTRACE( FPrint( _L("CFMRadioIdleController::ConstructL()") ) );
    }


// ---------------------------------------------------------------------------
// CFMRadioIdleController::NewL
// Two-phase constructor of CFMRadioPreset
// ---------------------------------------------------------------------------
//
CFMRadioIdleController* CFMRadioIdleController::NewL() 
    {
    FTRACE( FPrint( _L("CFMRadioIdleController::NewL()") ) );
    CFMRadioIdleController* self = new ( ELeave ) CFMRadioIdleController();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CFMRadioIdleController::~CFMRadioIdleController
// Destructor
// ---------------------------------------------------------------------------
//
CFMRadioIdleController::~CFMRadioIdleController()
    {
    FTRACE( FPrint( _L("CFMRadioIdleController::~CFMRadioIdleController()") ) );
    iControlArray.Close();
    }

// ---------------------------------------------------------------------------
// CFMRadioIdleController::AddControlItem
// ---------------------------------------------------------------------------
//
void CFMRadioIdleController::AddControlItem( const MFMRadioIdleControlInterface& aControl )
    {    
    TInt index = iControlArray.FindInAddressOrder( &aControl );
    if ( index == KErrNotFound )
        {
        iControlArray.InsertInAddressOrder( &aControl );
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioIdleController::ShowControl
// ---------------------------------------------------------------------------
//
void CFMRadioIdleController::ShowControl( const MFMRadioIdleControlInterface& aControl )
    {
    for ( TInt i = 0; i < iControlArray.Count(); i++ )
        {
        MFMRadioIdleControlInterface* controlItem = iControlArray[i];
        // show requested item
        if ( &aControl == controlItem )
            {
            controlItem->Show();
            }
        else // hide rest of the items
            {
            controlItem->Hide();
            }
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioIdleController::HideAllControls
// ---------------------------------------------------------------------------
//
void CFMRadioIdleController::HideAllControls()
    {
    FTRACE( FPrint( _L("CFMRadioIdleController::HideAllControls()") ) );
    for ( TInt i = 0; i < iControlArray.Count(); i++ )
        {
        iControlArray[i]->Hide();
        }
    }

// ---------------------------------------------------------------------------
// CFMRadioIdleController::DeactivateControls
// ---------------------------------------------------------------------------
//
void CFMRadioIdleController::DeactivateControls()
    {
    FTRACE( FPrint( _L("CFMRadioIdleController::DeactivateControls()") ) );
    for ( TInt i = 0; i < iControlArray.Count(); i++ )
        {
        iControlArray[i]->Deactivate();
        }
    }

// end of file
