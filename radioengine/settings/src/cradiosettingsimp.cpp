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
#include <bautils.h>
#include <coemain.h>
#include <ConeResLoader.h>
#include <f32file.h>
#include <data_caging_path_literals.hrh>

// User includes
#include "cradiosettingsimp.h"
#include "cradioapplicationsettings.h"
#include "cradioenginesettings.h"
#include "cradiorepositorymanager.h"
#include "radioengineutils.h"
#include "radioengineutils.h"
#include "cradioenginelogger.h"

// The name of the radio settings resource file.
_LIT( KRadioSettingsResourceFile, "radioenginesettings.rsc" );

// The granularity of the repository manager array.
const TInt KRadioSettingsRepositoryManagerGranularity = 8;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSettingsImp* CRadioSettingsImp::NewL( CCoeEnv* aCoeEnv )
    {
    CRadioSettingsImp* self = new (ELeave) CRadioSettingsImp;
    CleanupStack::PushL( self );
    self->ConstructL( aCoeEnv );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioSettingsImp::ConstructL( CCoeEnv* aCoeEnv )
    {
    RadioEngineUtils::InitializeL( aCoeEnv );
    LoadResourcesL();

    iRepositoryManager = CRadioRepositoryManager::NewL( KRadioSettingsRepositoryManagerGranularity );

    // Constructs the implementors of the interfaces.
    iApplicationSettings = CRadioApplicationSettings::NewL( *iRepositoryManager, *RadioEngineUtils::Env() );
    iEngineSettings = CRadioEngineSettings::NewL( *iRepositoryManager, *RadioEngineUtils::Env() );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSettingsImp::CRadioSettingsImp()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSettingsImp::~CRadioSettingsImp()
    {
    delete iApplicationSettings;
    delete iEngineSettings;
    delete iRepositoryManager;

    if ( iResourceLoader )
        {
        iResourceLoader->Close();
        }
    delete iResourceLoader;
    RadioEngineUtils::Release();
    }

// ---------------------------------------------------------------------------
// Determines if region is allowed.
// ---------------------------------------------------------------------------
//
TBool CRadioSettingsImp::IsRegionAllowed( TRadioRegion aRegionId ) const
    {
    return iEngineSettings->IsRegionAllowed( aRegionId );
    }

// ---------------------------------------------------------------------------
// Returns the application settings interface.
// ---------------------------------------------------------------------------
//
MRadioApplicationSettings& CRadioSettingsImp::ApplicationSettings() const
    {
    return *iApplicationSettings;
    }

// ---------------------------------------------------------------------------
// Returns the radio settings interface.
// ---------------------------------------------------------------------------
//
MRadioEngineSettings& CRadioSettingsImp::EngineSettings() const
    {
    return *iEngineSettings;
    }

// ---------------------------------------------------------------------------
// Returns the radio settings setter interface.
// ---------------------------------------------------------------------------
//
MRadioSettingsSetter& CRadioSettingsImp::RadioSetter() const
    {
    return *iEngineSettings;
    }

// ---------------------------------------------------------------------------
// Returns the underlying repository.
// ---------------------------------------------------------------------------
//
CRadioRepositoryManager& CRadioSettingsImp::Repository() const
    {
    return *iRepositoryManager;
    }

// ---------------------------------------------------------------------------
// Static version of ResolveDriveL.
// ---------------------------------------------------------------------------
//
void CRadioSettingsImp::ResolveDriveL( TFileName& aFileName, const TDesC& aPath )
    {
    LOG_FORMAT( "CRadioSettingsImp::ResolveDriveL( aFileName = %S, aPath = %S )", &aFileName, &aPath );

    RFs& fsSession = RadioEngineUtils::FsSession();
    TFileName fileName;
    TFileName baseResource;
    TFindFile finder( fsSession );
    TLanguage language( ELangNone );

    _LIT( resourceFileExt, ".rsc" );
    _LIT( resourceFileWildExt, ".r*" );

    TParsePtrC parse( aFileName );
    TBool isResourceFile = ( parse.Ext() == resourceFileExt() );

    TInt err = KErrUnknown;
    if  ( isResourceFile )
        {
        CDir* entries = NULL;
        fileName.Copy( parse.Name() );
        fileName.Append( resourceFileWildExt() );
        err = finder.FindWildByDir( fileName, aPath, entries );
        delete entries;
        }
    else
        {
        // TFindFile applies search order that is from
        // drive Y to A, then Z
        err = finder.FindByDir( aFileName, aPath );
        }

    LOG_FORMAT( "CRadioSettingsImp::ResolveDriveL - err = %d", err );
    TBool found = EFalse;
    if ( !isResourceFile && err == KErrNone )
        {
        found = ETrue;
        aFileName.Zero();
        aFileName.Append( finder.File() );
        }

    while ( !found && err == KErrNone && isResourceFile )
        {
        // Found file
        fileName.Zero();
        TParsePtrC foundPath( finder.File() );
        fileName.Copy( foundPath.DriveAndPath() );
        fileName.Append( aFileName );
        BaflUtils::NearestLanguageFile( fsSession, fileName, language );
        if ( language != ELangNone && BaflUtils::FileExists( fsSession, fileName ) )
            {
            found = ETrue;
            aFileName.Zero();
            aFileName.Copy( fileName );
            }
        else
            {
            if ( language == ELangNone &&
                 !baseResource.Compare( KNullDesC ) &&
                 BaflUtils::FileExists( fsSession, fileName ) )
                {
                baseResource.Copy( fileName );
                }
            CDir* entries = NULL;
            err = finder.FindWild( entries );
            delete entries;
            }
        }

    if ( !found && baseResource.Compare( KNullDesC ) )
        {
        // If we found *.rsc then better to use that than nothing
        if ( BaflUtils::FileExists( fsSession, baseResource ) )
            {
            found = ETrue;
            aFileName.Zero();
            aFileName.Append( baseResource );
            }
        }

    if ( !found )
        {
        LOG_FORMAT( "CRadioSettingsImp::ResolveDriveL - File %S not found ( err = %d )!", &aFileName, err );
        User::Leave( KErrNotFound );
        }

    LOG_FORMAT( "CRadioSettingsImp::ResolveDriveL( aFileName = %S )", &aFileName );
    }

// ---------------------------------------------------------------------------
// Loads the required resources.
// ---------------------------------------------------------------------------
//
void CRadioSettingsImp::LoadResourcesL()
    {
    // Allocated in heap only so that the resource loader header doesn't need to be
    // included in the header of this class. This is because this will be included
    // by a QT component that should not depend on CONE
    iResourceLoader = new (ELeave) RConeResourceLoader( *RadioEngineUtils::Env() );

    TFileName resourceFileName;
    resourceFileName.Append( KRadioSettingsResourceFile );

    ResolveDriveL( resourceFileName, KDC_RESOURCE_FILES_DIR );

    iResourceLoader->OpenL( resourceFileName );
    }

