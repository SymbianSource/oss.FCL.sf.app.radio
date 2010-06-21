/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for music store integration.
*
*/

#include <apgcli.h>
#include <apgtask.h>
#include <centralrepository.h>
#include <e32base.h>
#include <e32property.h>
#include <eikenv.h>
#include <eikmenup.h>
#include <eikmenub.h>
#include <fmradio.rsg>
#include <mpxfindinmusicshop.h>
#include <mpxfindinmusicshopcommon.h>   //for p&s keys

#include "fmradio.hrh"
#include "fmradiomusicstorehandler.h"
#include "debug.h"

// Some debug flags

// If defined, forces Operator specific music shop to be enabled,
// regardless if it is supported or not. For debugging purposes
#undef FORCE_OPERATOR_MUSIC_SHOP

// Following constants are from mpxmusicplayer_101FFCDC.crml
const TUid KCRUidMPXMPSettings = {0x101FFCDC}; // UID for Music Store Settings CenRep
const TUint32 KMPXMusicStoreUID = 0x3;
const TUint32 KOperatorMusicStore = 0x4;
const TUint32 KOperatorMusicStoreType = 0x5;
const TUint32 KOperatorMusicStoreDisplayName = 0x6;
const TUint32 KOperatorMusicStoreNativeUid = 0x7;
const TUint32 KOperatorMusicStoreWebPage = 0x9;
const TUint32 KOperatorMusicStoreURI = 0xA;
const TInt TUInt32HexLength = 8;

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::CMusicStoreHandler
// C++ class constructor.
// ----------------------------------------------------
//
CFMRadioMusicStoreHandler::CFMRadioMusicStoreHandler( TInt aResourceId )
: iResourceId( aResourceId )
    {
    // Nothing to do
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::~CMusicStoreHandler
// Default destructor.
// ----------------------------------------------------
//
CFMRadioMusicStoreHandler::~CFMRadioMusicStoreHandler()
    {
    iOperatorMusicStoreName.Close();
    iOperatorStoreWebsite.Close();
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::NewL
// Two-phased class constructor.
// ----------------------------------------------------
//
CFMRadioMusicStoreHandler* CFMRadioMusicStoreHandler::NewL( TInt aResourceId )
    {
    CFMRadioMusicStoreHandler* self = new (ELeave) CFMRadioMusicStoreHandler( aResourceId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::NewL
// Second phase class constructor.
// ----------------------------------------------------
//
void CFMRadioMusicStoreHandler::ConstructL()
    {
    iCoeEnv = CEikonEnv::Static();
    CRepository* repository = CRepository::NewL( KCRUidMPXMPSettings );
    CleanupStack::PushL( repository );
    
    RBuf8 uidDes;
    uidDes.CleanupClosePushL();
    uidDes.CreateL( TUInt32HexLength );
    TInt error = KErrNone;
    error = repository->Get( KMPXMusicStoreUID, uidDes );
    
    // Check for availability of Nokia Music store
    if ( !error )
        {
        iMusicStoreUID = Uint32Presentation( uidDes );
        iMusicStoreAppInstalled = IsAppInstalledL( TUid::Uid( iMusicStoreUID ) );
        }

    if ( NokiaMusicStoreAvailable() )
        {
        InitializeMusicStore( TUid::Uid( iMusicStoreUID ) );
        }

    // Check if operator provided a music store
    TFMRadioOperatorAppType appType = CheckOperatorMusicStoreTypeL( *repository );
    if ( appType != EFMRadioAppNotAvailable )
        {
        // Get the name of the music store
        const TInt initialNameLength = 11;
        iOperatorMusicStoreName.CreateL( initialNameLength );
        TInt realLength = KErrNotFound;
        error = repository->Get( KOperatorMusicStoreDisplayName, iOperatorMusicStoreName, realLength );
        if ( error == KErrOverflow )
            {
            iOperatorMusicStoreName.ReAllocL( realLength );
            error = repository->Get( KOperatorMusicStoreDisplayName, iOperatorMusicStoreName );
            }
        User::LeaveIfError( error );
#ifdef FORCE_OPERATOR_MUSIC_SHOP
        if ( !iOperatorMusicStoreName.Length() )
            {
            _LIT( KHardCodedName, "O'MusicShop" ); // length <= initialNameLength
            iOperatorMusicStoreName.Copy( KHardCodedName );
            }
#endif

        // Get application type specific parameters
        InitializeParametersL( *repository, appType );
        }
    CleanupStack::PopAndDestroy( 2 ); // uidDes, repository
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::CheckOperatorMusicStoreTypeL
// ----------------------------------------------------
//
CFMRadioMusicStoreHandler::TFMRadioOperatorAppType CFMRadioMusicStoreHandler::CheckOperatorMusicStoreTypeL(
        CRepository& aRepository )
    {
    TFMRadioOperatorAppType appType = EFMRadioAppNotAvailable;
    // Check if operator provided a music store
    TBool operatorStorePresent = EFalse;
    TInt error = aRepository.Get( KOperatorMusicStore, operatorStorePresent );
    if ( error != KErrNotFound )
        {
        User::LeaveIfError( error );
        }
    if ( operatorStorePresent )
        {
        TInt appTypeCR;
        error = aRepository.Get( KOperatorMusicStoreType, appTypeCR );
        User::LeaveIfError( error );
        if ( appTypeCR == 0 ) // 0 = Native app
            {
            appType = EFMRadioNativeApp;
            }
        else    // 1 = Java app
            {
            appType = EFMRadioJavaApp;
            }
         }

    // The code assumes that if both application and website is available then
    // website is preferred type
    TInt website;
    error = aRepository.Get( KOperatorMusicStoreWebPage, website );
    User::LeaveIfError( error );
#ifdef FORCE_OPERATOR_MUSIC_SHOP
    website = ETrue;
#endif
    if ( website )
        {
        appType = EFMRadioWebsite;
        }

    return appType;
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::InitializeParametersL
// ----------------------------------------------------
//
void CFMRadioMusicStoreHandler::InitializeParametersL(
        CRepository& aRepository,
        TFMRadioOperatorAppType aAppType )
    {
    TInt error = KErrNone;
    switch ( aAppType )
        {
        case EFMRadioNativeApp:
            {
            RBuf8 operatorUidDes;
            operatorUidDes.CleanupClosePushL();
            operatorUidDes.CreateL( TUInt32HexLength );
            
            TInt error = KErrNone;
            error = aRepository.Get( KOperatorMusicStoreNativeUid, operatorUidDes );
            
            if ( !error )
                {
                iOperatorStoreNativeUID = Uint32Presentation( operatorUidDes );
                // It is assumed that both Nokia Music Shop and operator specific music store use
                // same kind of interface to communicate
                InitializeMusicStore( TUid::Uid( iOperatorStoreNativeUID ) );
                }
            CleanupStack::PopAndDestroy( &operatorUidDes );
            break;
            }
        case EFMRadioJavaApp:
            // Java application not supported currently
            break;
        case EFMRadioWebsite:
            {
            const TInt initialSize = 5;
            TInt realLength = KErrNotFound;
            iOperatorStoreWebsite.Create( initialSize );
            error = aRepository.Get( KOperatorMusicStoreURI, iOperatorStoreWebsite, realLength );
            if ( error == KErrOverflow )
                {
                iOperatorStoreWebsite.ReAllocL( realLength );
                error = aRepository.Get( KOperatorMusicStoreURI, iOperatorStoreWebsite );
                }
            User::LeaveIfError( error );
#ifdef FORCE_OPERATOR_MUSIC_SHOP
            if ( !iOperatorStoreWebsite.Length() )
                {
                _LIT( KHardCodedName, "http://www.google.com/" );
                iOperatorStoreWebsite.ReAllocL( KHardCodedName().Length() );
                iOperatorStoreWebsite.Copy( KHardCodedName );
                }
#endif
            break;
            }
        default:
            break;
        }
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::InitializeMusicShop
// ----------------------------------------------------
//
void CFMRadioMusicStoreHandler::InitializeMusicStore( TUid aMusicstoreUid )
    {
    //MusicShop CenRep initialization values
    _LIT_SECURITY_POLICY_C1(KMPlayerRemoteReadPolicy, ECapabilityReadUserData);
    _LIT_SECURITY_POLICY_C1(KMPlayerRemoteWritePolicy, ECapabilityWriteUserData);
    TInt retval(KErrNone);
    // P/S key for music shop
    retval = RProperty::Define( aMusicstoreUid,
            KMShopCategoryId,
            RProperty::EInt,
            KMPlayerRemoteReadPolicy,
            KMPlayerRemoteWritePolicy );

    if( retval != KErrAlreadyExists)
        {
        RProperty::Set( aMusicstoreUid,
                KMShopCategoryId,
                KFindInMShopKeyInValid );  // initialize Find In Musicshop was not called

        RProperty::Define( aMusicstoreUid,
                KMShopCategoryName,
                RProperty::ELargeText,
                KMPlayerRemoteReadPolicy,
                KMPlayerRemoteWritePolicy );
        }
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::GetUint32Presentation
// ----------------------------------------------------
//
TUint32 CFMRadioMusicStoreHandler::Uint32Presentation( const TDesC8& aBuf )
    {
    TUint32 result = 0;

    const TInt KProEngByteLength( 8 );
    const TInt length( aBuf.Length() );
    const TInt maxBitShift( KProEngByteLength * ( length - 1 ) );

    for ( TInt i( 0 ); i < length; ++i )
        {
        result |= ( aBuf[i] <<
                   ( maxBitShift - ( KProEngByteLength * i ) ) );
        }
    return result;
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::IsMusicStoreCommandId
// Goes through known command ids and compares to them
// ----------------------------------------------------
//
TBool CFMRadioMusicStoreHandler::IsMusicStoreCommandId( TInt aCommandId )
    {
    TBool isHandled = EFalse;
    switch( aCommandId )
        {
        case EFMRadioMusicStore:
        case EFMRadioMusicStoreGoogle:
        case EFMRadioMusicStoreWikipedia:
        case EFMRadioMusicStoreAmg:
        case EFMRadioMusicStoreNokiaMusicShop:
        case EFMRadioMusicStoreMusicPortl:
        case EFMRadioMusicStoreOperator:
            isHandled = ETrue;
            break;
        default:
            break;
        }
    return isHandled;
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::MusicStoreCount
// ----------------------------------------------------
//
TInt CFMRadioMusicStoreHandler::MusicStoreCount()
    {
    TInt count = 0;
    if ( NokiaMusicStoreAvailable() )
        {
        count++;
        }
    if ( OperatorMusicStoreAvailable() )
        {
        count++;
        }
    return count;
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::OperatorMusicStoreAvailable
// ----------------------------------------------------
//
TBool CFMRadioMusicStoreHandler::OperatorMusicStoreAvailable()
    {
    return ( iOperatorMusicStoreName.Length() != 0 );
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::NokiaMusicStoreAvailable
// ----------------------------------------------------
//
TBool CFMRadioMusicStoreHandler::NokiaMusicStoreAvailable()
    {
    TBool response = EFalse;
    
    if ( iMusicStoreUID != 0 && iMusicStoreAppInstalled )
        {
        response = ETrue;
        }
    return response;
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::InitializeMenuL
// Takes care of dynamically initializing menu items related to Music shop.
// Takes care of adding either menu item or cascaded menuitem
// based on how many Music shops there is available. Fills the
// cascaded menuitem automatically.
// ----------------------------------------------------
//
TBool CFMRadioMusicStoreHandler::InitializeMenuL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    TBool resourceHandled = EFalse;

    if ( aResourceId == R_FMRADIO_OPTIONS_MUSIC_STORE_SUB_MENU )
        {
        if ( NokiaMusicStoreAvailable() )
            {
            aMenuPane->SetItemDimmed( EFMRadioMusicStoreNokiaMusicShop, EFalse );
            }
        else
            {
            aMenuPane->SetItemDimmed( EFMRadioMusicStoreNokiaMusicShop, ETrue );
            }
        if ( OperatorMusicStoreAvailable() )
            {
            aMenuPane->SetItemTextL( EFMRadioMusicStoreOperator, iOperatorMusicStoreName );
            aMenuPane->SetItemDimmed( EFMRadioMusicStoreOperator, EFalse );
            }
        else
            {
            aMenuPane->SetItemDimmed( EFMRadioMusicStoreOperator, ETrue );
            }
        resourceHandled = ETrue;
        }
    else if ( iResourceId == aResourceId )
        {
        const TInt firstIndex = 0;
        TInt firstCommand = aMenuPane->MenuItemCommandId( firstIndex );
        if ( MusicStoreCount() > 1 || OperatorMusicStoreAvailable() )
            {
            aMenuPane->AddMenuItemsL( R_FMRADIO_OPTIONS_MUSIC_STORE_WITH_SUB_MENU, firstCommand );
            }
        else
            {
            aMenuPane->AddMenuItemsL( R_FMRADIO_OPTIONS_MUSIC_STORE, firstCommand );
            }

        // Show or hide menu item based on Music store state
        if ( iState == EFMRadioMusicStoreDisabled || iState == EFMRadioMusicStoreUninitialized )
            {
            aMenuPane->SetItemDimmed( EFMRadioMusicStore, ETrue );
            }
        else if ( iState == EFMRadioMusicStoreEnabled && !MusicStoreCount() )
            {
            aMenuPane->SetItemDimmed( EFMRadioMusicStore, ETrue );
            }
        else
            {
            aMenuPane->SetItemDimmed( EFMRadioMusicStore, EFalse );
            }
        }
    else
        {
        //NOP
        }
    return resourceHandled;
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::EnableMusicStore
// Shows or hides Music store related functionality
// ----------------------------------------------------
//
void CFMRadioMusicStoreHandler::EnableMusicStore( TBool aEnable, TBool aForce )
    {
    if ( aForce && aEnable )
        {
        iState = EFMRadioMusicStoreForceEnabled;
        }
    else if ( aEnable )
        {
        iState = EFMRadioMusicStoreEnabled;
        }
    else
        {
        iState = EFMRadioMusicStoreDisabled;
        }
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::LaunchMusicStoreL
// Launches appropriate Music Store functionality based on parameters.
// ----------------------------------------------------
//
void CFMRadioMusicStoreHandler::LaunchMusicStoreL(
        TInt aCommandId,
        const TDesC& aTitle,
        const TDesC& aArtist,
        const TDesC& aAlbum )
    {
    if ( aCommandId == EFMRadioMusicStore )
        {
        aCommandId = EFMRadioMusicStoreNokiaMusicShop;
        }

    RBuf searchString;
    searchString.CleanupClosePushL();

    TBool isHandled = ETrue;
    switch( aCommandId )
        {
        case EFMRadioMusicStoreGoogle:
        case EFMRadioMusicStoreWikipedia:
        case EFMRadioMusicStoreAmg:
        case EFMRadioMusicStoreMusicPortl:
            isHandled = EFalse;
            break;
        case EFMRadioMusicStoreNokiaMusicShop:
            searchString.Assign( NokiaMusicShopSearchL( aTitle, aArtist, aAlbum ) );
            break;
        case EFMRadioMusicStoreOperator:
            if ( iOperatorStoreWebsite.Length() != 0 )
                {
                searchString.CreateL( iOperatorStoreWebsite );
                }
            else if ( iOperatorStoreNativeUID )
                {
                // It is assumed that both Nokia Music Shop and operator specific music store use
                // same kind of interface to communicate
                searchString.Assign( NokiaMusicShopSearchL( aTitle, aArtist, aAlbum ) );
                }
            else
                {
                isHandled = EFalse;
                }
            break;
        default:
            FTRACE( FPrint( _L("CFMRadioMusicStoreHandler::LaunchMusicStoreL - Unknown id = %d"), aCommandId ) );
            isHandled = EFalse;
            break;
        }

    if ( isHandled )
        {
        switch( aCommandId )
            {
            case EFMRadioMusicStoreNokiaMusicShop:
                {
                TUid musicshopUid( TUid::Uid( iMusicStoreUID ) );
                LaunchMusicShopL( musicshopUid, searchString );
                break;
                }
            case EFMRadioMusicStoreOperator:
                LaunchOperatorMusicStoreL( searchString );
                break;
            default:
                LaunchWebPageL( searchString );
                break;
            }
        CleanupStack::PopAndDestroy( &searchString );
        }
    }

// -----------------------------------------------------------------------------
// CFMRadioMusicStoreHandler::LaunchOperatorMusicStoreL
// -----------------------------------------------------------------------------
//
void CFMRadioMusicStoreHandler::LaunchOperatorMusicStoreL( const TDesC& aSearchString )
    {
    if ( iOperatorStoreWebsite.Length() != 0 )
        {
        LaunchWebPageL( aSearchString );
        }
    else if ( iOperatorStoreNativeUID )
        {
        // It is assumed that both Nokia Music Shop and operator specific music store use
        // same kind of interface to communicate
        TUid musicshopUid( TUid::Uid( iOperatorStoreNativeUID ) );
        LaunchMusicShopL( musicshopUid, aSearchString );
        }
    else
        {
        //NOP
        }
    }

// -----------------------------------------------------------------------------
// CFMRadioMusicStoreHandler::NokiaMusicShopSearchL
// -----------------------------------------------------------------------------
//
HBufC* CFMRadioMusicStoreHandler::NokiaMusicShopSearchL(
        const TDesC& aSongName,
        const TDesC& aArtistName,
        const TDesC& aAlbumName )
    {
    HBufC* searchUrl = NULL;    // Launching the music store client
    _LIT(KReferrerAppParam, "&ReferrerApp=3");
    
    if ( aSongName.Length() || aArtistName.Length() || aAlbumName.Length() )
        {
        CMPXFindInMShop* finder = CMPXFindInMShop::NewL();  //ECom Plugin
        CleanupStack::PushL( finder );
        searchUrl = finder->CreateSearchURLL(  aSongName,
                                         aArtistName,
                                         aAlbumName,
                                         KNullDesC,     // Composer - Not used
                                         KNullDesC );   // Genre - Not used
        
        HBufC* finalUrl = HBufC::NewL( searchUrl->Length() + KReferrerAppParam().Length() );
        
        TPtr urlPtr = finalUrl->Des();
        urlPtr.Copy( *searchUrl );
        urlPtr.Append( KReferrerAppParam() );
        
        delete searchUrl;
        searchUrl = NULL;
        
        CleanupStack::PopAndDestroy( finder ); // finder
        REComSession::FinalClose();
        return finalUrl;
        }
    return searchUrl;
    }

// -----------------------------------------------------------------------------
// CFMRadioMusicStoreHandler::LaunchMusicShopL
// Launch Nokia music shop application
// -----------------------------------------------------------------------------
//
void CFMRadioMusicStoreHandler::LaunchMusicShopL( TUid aMusicshopUid, const TDesC& aSearchString )
    {
    if ( aSearchString.Length() )
        {
        RProperty::Set( aMusicshopUid,
                KMShopCategoryName,
                aSearchString );
    
        RProperty::Set( aMusicshopUid,
                KMShopCategoryId,
                KFindInMShopKeyValid );  // Set Key to Valid
        }

    TApaTaskList taskList( iCoeEnv->WsSession() );
    TApaTask task = taskList.FindApp( aMusicshopUid );

    if ( task.Exists() )
        {
        task.BringToForeground();
        }
    else
        {
        RApaLsSession session;
        User::LeaveIfError( session.Connect() );
        TThreadId threadId;
        session.CreateDocument( KNullDesC, aMusicshopUid, threadId );
        session.Close();
        }
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::LaunchWebPageL
// Opens web site based on the string
// ----------------------------------------------------
//
void CFMRadioMusicStoreHandler::LaunchWebPageL( const TDesC& aWebpage )
    {
    _LIT( KCommand, "4 ");
    RBuf param;
    param.CreateL( KCommand().Length() + aWebpage.Length() );
    param.CleanupClosePushL();
    param.Copy( KCommand() );   //Web Browser requires this in order to fetch url.
    param.Append( aWebpage );

    const TInt KWebBrowserUid = 0x10008D39;
    TUid browserId( TUid::Uid( KWebBrowserUid ) );
    TApaTaskList taskList( iCoeEnv->WsSession() );
    TApaTask task = taskList.FindApp( browserId );
    if ( task.Exists() )
        {
        HBufC8* param8 = HBufC8::NewLC( param.Length() );
        param8->Des().Append( param );
        task.SendMessage( TUid::Uid( 0 ), *param8 ); // Uid is not used
        CleanupStack::PopAndDestroy( param8 );
        }
    else
        {
        RApaLsSession session;
        User::LeaveIfError( session.Connect() );
        TThreadId threadId;
        session.StartDocument( param, browserId, threadId );
        session.Close();
        }
    CleanupStack::PopAndDestroy( &param );
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::IsMusicStoreLaunchable
// ----------------------------------------------------
//
TBool CFMRadioMusicStoreHandler::IsMusicStoreEnabled()
    {
    TBool musicStoreLaunchable = EFalse;
    
    if ( iState != CFMRadioMusicStoreHandler::EFMRadioMusicStoreDisabled &&
         iState != CFMRadioMusicStoreHandler::EFMRadioMusicStoreUninitialized )
        {
        musicStoreLaunchable = ETrue;
        }
    return musicStoreLaunchable;
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::OperatorMusicStoreName
// ----------------------------------------------------
//
const TDesC& CFMRadioMusicStoreHandler::OperatorMusicStoreName() const
    {
    return iOperatorMusicStoreName;
    }

// ----------------------------------------------------
// CFMRadioMusicStoreHandler::IsAppInstalled
// ----------------------------------------------------
//
TBool CFMRadioMusicStoreHandler::IsAppInstalledL( const TUid& aAppUid )
    {
    TBool response = EFalse;
    
    RApaLsSession apaSession;
    CleanupClosePushL( apaSession );
    User::LeaveIfError( apaSession.Connect() );
    apaSession.GetAllApps();
    
    TApaAppInfo appInfo;
    TInt err = apaSession.GetAppInfo( appInfo, aAppUid );
    CleanupStack::PopAndDestroy( &apaSession );
    
    if ( !err )
        {
        // app was found
        response = ETrue;
        }
    return response;
    }
