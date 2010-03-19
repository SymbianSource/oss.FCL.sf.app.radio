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

#ifndef C_RADIOAPPLICATIONSETTINGS_H
#define C_RADIOAPPLICATIONSETTINGS_H

#include "mradioapplicationsettings.h"
#include "cradiosettingsbase.h"

/**
 * Concrete implementation of application settings.
 */
NONSHARABLE_CLASS( CRadioApplicationSettings ) : public CRadioSettingsBase
                                               , public MRadioApplicationSettings
    {

public:

    static CRadioApplicationSettings* NewL( CRadioRepositoryManager& aRepositoryManager, CCoeEnv& aCoeEnv );

    ~CRadioApplicationSettings();

// from base class MRadioApplicationSettings

    TInt SetActiveFocusLocation( TInt aIndex );
    TInt ActiveFocusLocation() const;
    void SetAudioPlayHistoryL( TRadioCRAudioPlayHistory aHistory );
    TRadioCRAudioPlayHistory AudioPlayHistory() const;
    TInt UpdateStartCount();
    TInt SetUiFlags( TUint aUiFlags );
    TUint UiFlags() const;

private:

    CRadioApplicationSettings( CRadioRepositoryManager& aRepositoryManager, CCoeEnv& aCoeEnv );

    void ConstructL();

    };

#endif // C_RADIOAPPLICATIONSETTINGS_H
