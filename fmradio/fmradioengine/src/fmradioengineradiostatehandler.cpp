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
* Description:  The class implements the powerup/down, and scan and store
*                       state machines.
*
*/


// INCLUDES
#include <f32file.h>

#include "fmradioengineradiostatehandler.h"
#include "fmradioengine.h"
#include "debug.h"


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------
// CRadioStateHandler::CRadioStateHandler
// c++ default constructor
// ----------------------------------------------------
//
CRadioStateHandler::CRadioStateHandler(CRadioEngine* aEngine)
    :    CActive(EPriorityStandard), iEngine(aEngine)
    {
    CActiveScheduler::Add(this);
    }

// ----------------------------------------------------
// CRadioStateHandler::ConstructL
// 2nd phase constructor
// ----------------------------------------------------
//
void CRadioStateHandler::ConstructL()
    {
    }

// ----------------------------------------------------
// CRadioStateHandler::NewL
// Method for creating new instances of this class.
// ----------------------------------------------------
//
CRadioStateHandler* CRadioStateHandler::NewL(
    CRadioEngine* aEngine) //a pointer to CRadioEngine object
    {
    CRadioStateHandler *self = new(ELeave) CRadioStateHandler(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CRadioStateHandler::~CRadioStateHandler()
    {
    Cancel();
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

// ----------------------------------------------------
// CRadioStateHandler::Callback
// Trigger the active object to call the client callback function
// Returns: None
// ----------------------------------------------------
//
void CRadioStateHandler::Callback(
    MRadioEngineStateChangeCallback::TFMRadioNotifyEvent aEventCode,   // a event code
    TInt aErrorCode )  // a error code
    {
    iCallbackEventCode = aEventCode;
    iCallbackErrorCode = aErrorCode;
    Cancel();
    SetActive();
    TRequestStatus* status=&iStatus;
    User::RequestComplete(status, KRadioStateEventCallback);
    }


// ----------------------------------------------------
// CRadioStateHandler::DoCancel
// Cancel the timeout timer. This generally means that a 
// response was received, so the timeout is no longer needed.
// Returns: None
// ----------------------------------------------------
//
void CRadioStateHandler::DoCancel()
    {
    }

// ----------------------------------------------------
// CRadioStateHandler::RunL
// State Change Request/timeout handler
// Returns: None
// ----------------------------------------------------
//
void CRadioStateHandler::RunL()
    {
    FTRACE(FPrint(_L("inside runL()")));  

    if (iStatus.Int() == KRadioStateEventCallback)
        {
        iEngine->HandleCallback(iCallbackEventCode, iCallbackErrorCode);
        }
    }

// ---------------------------------------------------------
// CRadioStateHandler::RunError
// handle any errors that occur in the RunL
// Returns: aError: the error code
// ---------------------------------------------------------
//
TInt CRadioStateHandler::RunError( 
    TInt aError )  // error code
    {
    FTRACE( FPrint( _L("CRadioStateHandler::RunError()") ) );
    return aError;
    }
    
   // end of file
