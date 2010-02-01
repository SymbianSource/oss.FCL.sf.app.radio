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
* Description:  Main application class implementation.
*
*/


// INCLUDE FILES
#include <fmradiouids.h>

#include "fmradioapp.h"
#include "fmradiodocument.h"
#include "debug.h"
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CFMRadioApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CFMRadioApp::AppDllUid() const
    {
    return TUid::Uid( KUidFMRadioApplication );
    }

   
// ---------------------------------------------------------
// CFMRadioApp::CreateDocumentL()
// Creates CFMRadioDocument object
// ---------------------------------------------------------
//
CApaDocument* CFMRadioApp::CreateDocumentL()
    {
    return CFMRadioDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============


#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
    {
    return new CFMRadioApp;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }


// End of File  

