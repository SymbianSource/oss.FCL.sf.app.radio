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
#include <coemain.h>
#include <e32svr.h>

// User includes
#include "cradioenginelogger.h"
#include "cradioenginetls.h"

const int KUidRadioEngineTls = 0x10281CB8;

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Returns the CRadioEngineTls instance from TLS
// ---------------------------------------------------------------------------
//
CRadioEngineTls& CRadioEngineTls::Instance()
    {
    return *static_cast<CRadioEngineTls*>( UserSvr::DllTls( KUidRadioEngineTls ) );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineTls::InitializeL( CCoeEnv* aEnv, RFs* aFs )
    {
    if ( !UserSvr::DllTls( KUidRadioEngineTls ) )
        {
        CRadioEngineTls* self = new ( ELeave ) CRadioEngineTls( aEnv, aFs );
        CleanupStack::PushL( self );
        self->ConstructL();
        User::LeaveIfError( UserSvr::DllSetTls( KUidRadioEngineTls, self ) );
        CleanupStack::Pop( self );
        }
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineTls::CRadioEngineTls( CCoeEnv* aEnv, RFs* aFs )
    : iEnv( aEnv )
    , iFs( aFs )
    {
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineTls::ConstructL()
    {
    if ( !iEnv )
        {
        iEnv = CCoeEnv::Static();
        }

    if ( !iFs )
        {
        iFsOwned = ETrue;
        iFs = new ( ELeave ) RFs;
        User::LeaveIfError( iFs->Connect() );
        }

#ifdef LOGGING_ENABLED
    iLogger = CRadioEngineLogger::NewL( *iFs );
#endif // LOGGING_ENABLED

    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineTls::~CRadioEngineTls()
    {
    UserSvr::DllFreeTls( KUidRadioEngineTls );

#ifdef LOGGING_ENABLED
    delete iLogger;
#endif

    if ( iFs && iFsOwned )
        {
        iFs->Close();
        delete iFs;
        }
    }


// ---------------------------------------------------------------------------
// Return the logger storage if logging is enabled
// ---------------------------------------------------------------------------
//
MRadioEngineLogger* CRadioEngineTls::Logger()
    {
#ifdef LOGGING_ENABLED
    return CRadioEngineTls::Instance().iLogger;
#else
    return NULL;
#endif
    }


// ---------------------------------------------------------------------------
// Return the eikon environment
// ---------------------------------------------------------------------------
//
CCoeEnv* CRadioEngineTls::Env()
    {
    return CRadioEngineTls::Instance().iEnv;
    }


// ---------------------------------------------------------------------------
// Returns the file server session
// ---------------------------------------------------------------------------
//
RFs& CRadioEngineTls::FsSession()
    {
    return *CRadioEngineTls::Instance().iFs;
    }


// ---------------------------------------------------------------------------
// CRadioEngineTls::AddRef
// ---------------------------------------------------------------------------
//
void CRadioEngineTls::AddRef()
    {
    ++iRefs;
    }

// ---------------------------------------------------------------------------
// CRadioEngineTls::Release
// ---------------------------------------------------------------------------
//
void CRadioEngineTls::Release()
    {
    __ASSERT_DEBUG( iRefs > 0 , User::Panic( _L( "CRadioEngineTls" ), KErrCorrupt ) );
    if ( !--iRefs )
        {
        delete this;
        }
    }
