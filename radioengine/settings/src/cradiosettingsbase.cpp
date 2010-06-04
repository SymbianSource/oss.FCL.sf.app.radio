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
#include <s32mem.h>

// User includes
#include "cradiorepositorymanager.h"
#include "cradiosettingsbase.h"
#include "cradioenginelogger.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSettingsBase::CRadioSettingsBase( CRadioRepositoryManager& aRepositoryManager,
                                        CCoeEnv& aCoeEnv )
    : iCoeEnv( aCoeEnv )
    , iRepositoryManager( aRepositoryManager )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSettingsBase::~CRadioSettingsBase()
    {
    }

// ---------------------------------------------------------------------------
// Writes a desciptor array to the repository.
// ---------------------------------------------------------------------------
//
void CRadioSettingsBase::WriteArrayEntityL( const TUid& aUid, TUint32 aKey, const CDesCArray& aArray )
    {
    LOG_FORMAT( "CRadioSettingsBase::WriteArrayEntityL( aUid = %d, aKey = %d )", aUid.iUid, aKey );

    CBufFlat* buf = CBufFlat::NewL( KRadioEntityBuf8Length );
    CleanupStack::PushL( buf );
    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );

    TInt count = aArray.MdcaCount();
    stream << TCardinality( count );

    LOG_FORMAT( "     count = %d", count );

    for ( TInt i = 0; i < count; i++ )
        {
        DEBUGVAR( TPtrC ptr( aArray.MdcaPoint( i ) ) );
        LOG_FORMAT( "     entry %d: %S", i, &ptr );
        stream << aArray.MdcaPoint( i );
        }

    stream.CommitL();

    CleanupStack::PopAndDestroy( &stream );

    TRadioEntityBuf8 entityBuf( buf->Ptr( 0 ).Left( KRadioEntityBuf8Length ) );
    User::LeaveIfError( iRepositoryManager.SetEntityValue( aUid, aKey, entityBuf ) );

    CleanupStack::PopAndDestroy( buf );
    }

// ---------------------------------------------------------------------------
// Reads a descriptor array from the repository.
// ---------------------------------------------------------------------------
//
CDesCArray* CRadioSettingsBase::ReadArrayEntityL( const TUid& aUid, TUint32 aKey ) const
    {
    LOG_FORMAT( "CRadioSettingsBase::ReadArrayEntityL( aUid = %d, aKey = %d )", aUid.iUid, aKey );

    CBufFlat* buf = CBufFlat::NewL( KRadioEntityBuf8Length );
    CleanupStack::PushL( buf );
    buf->InsertL( 0, iRepositoryManager.EntityValueDes8( aUid, aKey ) );
    RBufReadStream stream( *buf );
    CleanupClosePushL( stream );

    TCardinality count;
    stream >> count;

    LOG_FORMAT( "     count = %d", static_cast<TInt>( count ) );

    CDesCArray* array = new ( ELeave ) CDesCArrayFlat( Max( static_cast<TInt>( count ), 1 ) );
    CleanupStack::PushL( array );

    for ( TInt i = 0; i < count; i++ )
        {
        HBufC* curBuf = HBufC::NewLC( stream, KMaxTInt );
        DEBUGVAR( TPtrC ptr( *curBuf ) );
        LOG_FORMAT( "     entry %d: %S", i, &ptr );
        array->AppendL( *curBuf );
        CleanupStack::PopAndDestroy( curBuf );
        }

    CleanupStack::Pop( array );
    CleanupStack::PopAndDestroy( 2, buf );

    return array;
    }
