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

#include "cradioenginelogger.h"
#include "radiointernalcrkeys.h"

#include "cradioapplicationsettings.h"
#include "cradiorepositorymanager.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioApplicationSettings* CRadioApplicationSettings::NewL( CRadioRepositoryManager& aRepositoryManager,
                                                            CCoeEnv& aCoeEnv )
    {
    CRadioApplicationSettings* self = new ( ELeave ) CRadioApplicationSettings( aRepositoryManager, aCoeEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioApplicationSettings::ConstructL()
    {
    iRepositoryManager.AddEntityL( KRadioCRUid, KRadioCRActiveFocusLocation, CRadioRepositoryManager::ERadioEntityInt );
    iRepositoryManager.AddEntityL( KRadioCRUid, KRadioCRAudioPlayHistory, CRadioRepositoryManager::ERadioEntityInt );
    iRepositoryManager.AddEntityL( KRadioCRUid, KRadioCRUiFlags, CRadioRepositoryManager::ERadioEntityInt );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioApplicationSettings::CRadioApplicationSettings( CRadioRepositoryManager& aRepositoryManager,
                                                      CCoeEnv& aCoeEnv )
    : CRadioSettingsBase( aRepositoryManager, aCoeEnv )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioApplicationSettings::~CRadioApplicationSettings()
    {
    }

// ---------------------------------------------------------------------------
// From class MRadioApplicationSettings.
//
// ---------------------------------------------------------------------------
//
TInt CRadioApplicationSettings::SetActiveFocusLocation( TInt aIndex )
    {
    return iRepositoryManager.SetEntityValue( KRadioCRUid, KRadioCRActiveFocusLocation, aIndex );
    }

// ---------------------------------------------------------------------------
// From class MRadioApplicationSettings.
//
// ---------------------------------------------------------------------------
//
TInt CRadioApplicationSettings::ActiveFocusLocation() const
    {
    return iRepositoryManager.EntityValueInt( KRadioCRUid, KRadioCRActiveFocusLocation );
    }

// ---------------------------------------------------------------------------
// From class MRadioApplicationSettings.
//
// ---------------------------------------------------------------------------
//
void CRadioApplicationSettings::SetAudioPlayHistoryL( TRadioCRAudioPlayHistory aHistory )
    {
    if ( AudioPlayHistory() != ERadioCRAudioPlayed )
        {
        User::LeaveIfError( iRepositoryManager.SetEntityValue( KRadioCRUid, KRadioCRAudioPlayHistory, aHistory ) );
        }
    }

// ---------------------------------------------------------------------------
// From class MRadioApplicationSettings.
//
// ---------------------------------------------------------------------------
//
MRadioApplicationSettings::TRadioCRAudioPlayHistory CRadioApplicationSettings::AudioPlayHistory() const
    {
    return static_cast<TRadioCRAudioPlayHistory>(
                    iRepositoryManager.EntityValueInt( KRadioCRUid, KRadioCRAudioPlayHistory ) );
    }

// ---------------------------------------------------------------------------
// From class MRadioApplicationSettings.
//
// ---------------------------------------------------------------------------
//
TInt CRadioApplicationSettings::UpdateStartCount()
    {
    TInt startCount = 0;
    TRAP_IGNORE
        (
        CRadioRepositoryManager::GetRepositoryValueL( KRadioCRUid, KRadioCRLaunchCount, startCount );
        CRadioRepositoryManager::SetRepositoryValueL( KRadioCRUid, KRadioCRLaunchCount, startCount + 1 );
        );
    return startCount;
    }

// ---------------------------------------------------------------------------
// From class MRadioApplicationSettings.
//
// ---------------------------------------------------------------------------
//
TInt CRadioApplicationSettings::SetUiFlags( TUint aUiFlags )
    {
    return iRepositoryManager.SetEntityValue( KRadioCRUid, KRadioCRUiFlags, static_cast<TInt>( aUiFlags ) );
    }

// ---------------------------------------------------------------------------
// From class MRadioApplicationSettings.
//
// ---------------------------------------------------------------------------
//
TUint CRadioApplicationSettings::UiFlags() const
    {
    return static_cast<TUint>( iRepositoryManager.EntityValueInt( KRadioCRUid, KRadioCRUiFlags ) );
    }
