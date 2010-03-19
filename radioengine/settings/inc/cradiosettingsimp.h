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

#ifndef C_RADIOSETTINGSIMP_H
#define C_RADIOSETTINGSIMP_H

// User includes
#include "cradiosettings.h"

// Forward declarations
class CCoeEnv;
class CRadioApplicationSettings;
class CRadioEngineSettings;
class CRadioRepositoryManager;
class RConeResourceLoader;

/**
 * Manages persistent application settings.
 *
 * Exposes access to the underlying settings implementations.
 */
NONSHARABLE_CLASS( CRadioSettingsImp ) : public CRadioSettings
    {

public:

    static CRadioSettingsImp* NewL( CCoeEnv* aCoeEnv = NULL );

    ~CRadioSettingsImp();

// from base class CRadioSettings

    TBool IsRegionAllowed( TRadioRegion aRegionId ) const;
    MRadioApplicationSettings& ApplicationSettings() const;
    MRadioEngineSettings& EngineSettings() const;
    MRadioSettingsSetter& RadioSetter() const;
    CRadioRepositoryManager& Repository() const;
    void ResolveDriveL( TFileName& aFileName, const TDesC& aPath );

private:

    CRadioSettingsImp();

    void ConstructL( CCoeEnv* aCoeEnv );

    /**
     * Loads the resources required by the settings.
     */
    void LoadResourcesL();

private: // data

    /**
     * The central repository manager.
     * Own.
     */
    CRadioRepositoryManager*    iRepositoryManager;

    /**
     * Resource loader for the settings resources.
     * Own.
     */
    RConeResourceLoader*        iResourceLoader;

    /**
     * Implementation of the application settings.
     * Own
     */
    CRadioApplicationSettings*  iApplicationSettings;

    /**
     * Implementation of the radio settings.
     * Own
     */
    CRadioEngineSettings*       iEngineSettings;

    };

#endif // C_RADIOSETTINGSIMP_H

