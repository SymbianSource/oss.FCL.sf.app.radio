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

#include "cradiorepositorymanager.h"
#include "cradiorepositorymanagerimp.h"
#include "radioengineutils.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CRadioRepositoryManager* CRadioRepositoryManager::NewL( TInt aGranularity )
    {
    return CRadioRepositoryManagerImp::NewL( aGranularity );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CRadioRepositoryManager::~CRadioRepositoryManager()
    {
    }

// ---------------------------------------------------------------------------
// Sets the value of a repository key.
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioRepositoryManager::SetRepositoryValueL( const TUid& aUid, TUint32 aKey, TInt aValue )
    {
    CRadioRepositoryManagerImp::SetRepositoryValueL( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Sets the value of a repository key.
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioRepositoryManager::SetRepositoryValueL( const TUid& aUid, TUint32 aKey, const TReal& aValue )
    {
    CRadioRepositoryManagerImp::SetRepositoryValueL( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Sets the value of a repository key.
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioRepositoryManager::SetRepositoryValueL( const TUid& aUid, TUint32 aKey, const TDesC8& aValue )
    {
    CRadioRepositoryManagerImp::SetRepositoryValueL( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Sets the value of a repository key.
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioRepositoryManager::SetRepositoryValueL( const TUid& aUid, TUint32 aKey, const TDesC16& aValue )
    {
    CRadioRepositoryManagerImp::SetRepositoryValueL( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Gets the value of a repository key.
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioRepositoryManager::GetRepositoryValueL( const TUid& aUid, TUint32 aKey, TInt& aValue )
    {
    CRadioRepositoryManagerImp::GetRepositoryValueL( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Gets the value of a repository key.
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioRepositoryManager::GetRepositoryValueL( const TUid& aUid, TUint32 aKey, TReal& aValue )
    {
    CRadioRepositoryManagerImp::GetRepositoryValueL( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Gets the value of a repository key.
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioRepositoryManager::GetRepositoryValueL( const TUid& aUid, TUint32 aKey, TDes8& aValue )
    {
    CRadioRepositoryManagerImp::GetRepositoryValueL( aUid, aKey, aValue );
    }

// ---------------------------------------------------------------------------
// Gets the value of a repository key.
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioRepositoryManager::GetRepositoryValueL( const TUid& aUid, TUint32 aKey, TDes16& aValue )
    {
    CRadioRepositoryManagerImp::GetRepositoryValueL( aUid, aKey, aValue );
    }
