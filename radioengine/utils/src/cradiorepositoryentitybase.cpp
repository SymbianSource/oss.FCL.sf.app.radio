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

#include "cradiorepositoryentitybase.h"
#include "cradioenginelogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CRadioRepositoryEntityBase::CRadioRepositoryEntityBase( const TUid& aUid,
                                                        TUint32 aKey,
                                                        MRadioRepositoryEntityObserver& aObserver,
                                                        CActive::TPriority aPriority )
    : CActive( aPriority ), iObserver( aObserver ), iUid( aUid ), iKey( aKey )
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CRadioRepositoryEntityBase::~CRadioRepositoryEntityBase()
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
// Returns the UID of this entity.
// ---------------------------------------------------------------------------
//
const TUid& CRadioRepositoryEntityBase::Uid() const
    {
    return iUid;
    }

// ---------------------------------------------------------------------------
// Returns the key of this entity.
// ---------------------------------------------------------------------------
//
TUint32 CRadioRepositoryEntityBase::Key() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iKey;
    }
