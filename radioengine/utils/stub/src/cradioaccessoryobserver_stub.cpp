/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of the License "Eclipse Public License v1.0"
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

#include "cradioenginelogger.h"

#include "cradioaccessoryobserver_stub.h"
#include "mradioheadseteventobserver.h"

#define STUB  iRadioStubManager->iAccessoryObserver

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioAccessoryObserver::CRadioAccessoryObserver()
    {
    LOG_METHOD_AUTO;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::ConstructL()
    {
    LOG_METHOD_AUTO;
    // Open chunk for test configuration/control data
    TInt err = iRadioStubManagerChunk.OpenGlobal(
            KRadioStubManagerLocalChunkName, EFalse, // == Read | Write
            EOwnerThread);
    User::LeaveIfError(err);
    if (sizeof(SRadioStubManager) > iRadioStubManagerChunk.MaxSize())
        {
        User::Leave(KErrTooBig);
        }
    TUint8* basePtr = iRadioStubManagerChunk.Base();
    User::LeaveIfNull(basePtr);
    iRadioStubManager = (SRadioStubManager*) basePtr;
    if (STUB.iLeaveNewL.iError)
        {
        User::Leave(STUB.iLeaveNewL.iError);
        }
    if (STUB.iLeaveConstructL.iError)
        {
        User::Leave(STUB.iLeaveConstructL.iError);
        }
    iHeadsetObserver = CRadioPropertyObserver::NewL(*this, KStub_KRadioPSUid,
            SRadioStubManager::EStub_KRadioTestPSKeyHeadsetConnected,
            CRadioPropertyObserver::ERadioPropertyInt);
    iHeadsetObserver->ActivateL();
    STUB.iHeadsetConnected = iHeadsetObserver->ValueInt();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioAccessoryObserver* CRadioAccessoryObserver::NewL()
    {
    LOG_METHOD_AUTO;
    CRadioAccessoryObserver* self = new (ELeave) CRadioAccessoryObserver;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioAccessoryObserver::~CRadioAccessoryObserver()
    {
    LOG_METHOD_AUTO;
    delete iHeadsetObserver;
    iRadioStubManagerChunk.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioAccessoryObserver::IsHeadsetConnectedL() const
    {
    LOG_METHOD_AUTO;
    if (STUB.iLeaveIsHeadsetConnectedL.iError)
        {
        User::Leave(STUB.iLeaveIsHeadsetConnectedL.iError);
        }
    return STUB.iHeadsetConnected;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::SetObserver(
        MRadioHeadsetEventObserver* aObserver)
    {
    LOG_METHOD_AUTO;
    STUB.iObserver = aObserver;
    }

// ---------------------------------------------------------------------------
// Handling of the int property changes is done here.
// Observer components are getting notifications in correspondence with what
// has changed
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::HandlePropertyChangeL(const TUid& aCategory,
        const TUint aKey, const TInt aValue)
    {
    LEVEL2( LOG_METHOD_AUTO );
    if ((KStub_KRadioPSUid == aCategory)
            && (SRadioStubManager::EStub_KRadioTestPSKeyHeadsetConnected
                    == aKey))
        {
        STUB.iHeadsetConnected = aValue;
        if (STUB.iHeadsetConnected)
            {
            if ( STUB.iObserver )
                {
                STUB.iObserver->HeadsetConnectedCallbackL();
                }
            }
        else
            {
            if ( STUB.iObserver )
                 {
                 STUB.iObserver->HeadsetDisconnectedCallbackL();
                 }
            }
        }
    }

// ---------------------------------------------------------------------------
// Dummy version for WINS in order to avoid compiler warnings.
// The real implementation of function is above.
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::HandlePropertyChangeL(
        const TUid& /*aCategory*/, const TUint /*aKey*/, const TDesC8& /*aValue*/)
    {
    LOG_METHOD_AUTO;
    }

// ---------------------------------------------------------------------------
// Handling of the text property changes is done here.
// Observer components are getting notifications in correspondence with what
// has changed
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::HandlePropertyChangeL(
        const TUid& /*aCategory*/, const TUint /*aKey*/, const TDesC& /*aValue*/)
    {
    LOG_METHOD_AUTO;
    }

// ---------------------------------------------------------------------------
// This is a callback function which is called when a P&S components returns
// an error
// ---------------------------------------------------------------------------
//
void CRadioAccessoryObserver::HandlePropertyChangeErrorL(
        const TUid& DEBUGVAR(aCategory), const TUint DEBUGVAR(aKey), TInt DEBUGVAR(aError) )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "aCategory = %i, aKey = %i, aError = %i", aCategory, aKey, aError );
    }
