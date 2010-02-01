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
* Description:  Handles music store integration.
*
*/


#ifndef FMRADIOMUSICSTOREHANDLER_H
#define FMRADIOMUSICSTOREHANDLER_H

#include <e32base.h>

class CEikMenuPane;
class CRepository;

/**
 * Handler for Music Store.
 *
 * Takes care of Nokia music store and operator specific music store.
 *
 * Provides also a interface to open a web page.
 */
class CFMRadioMusicStoreHandler : public CBase
    {
private:
    /**
     * Used for tracking the visibility state.
     */
    enum TFMRadioMusicStoreState
        {
        EFMRadioMusicStoreUninitialized,
        EFMRadioMusicStoreEnabled,
        EFMRadioMusicStoreDisabled,
        EFMRadioMusicStoreForceEnabled
        };

    /**
     * Distinguishes between different implementation types of music store.
     */
    enum TFMRadioOperatorAppType
        {
        EFMRadioAppNotAvailable,
        EFMRadioNativeApp,
        EFMRadioJavaApp,
        EFMRadioWebsite
        };

public:
    /**
     * Two-phase constructor of CMusicStoreHandler.
     * 
     * @param aResourceId   Menu pane where the music store item/items
     *                      will be added.
     */
    static CFMRadioMusicStoreHandler* NewL( TInt aResourceId );

    /**
     * Destructor
     */
    ~CFMRadioMusicStoreHandler();

    /**
     * Checks wether given command id is for music store functionality.
     * 
     * @param aCommandId command id to check
     * @returns ETrue for music store command ids
     */
    TBool IsMusicStoreCommandId( TInt aCommandId );

    /**
     * This is called when dynamically initializing menu (ie, from DynInitMenuPaneL).
     * 
     * @param aResourceId   Resource id for currently processed menu part
     * @param aMenuPane     Menu pane which is being handled
     * @returns ETrue, if menu pane was handled by this method
     */
    TBool InitializeMenuL( TInt aResourceId, CEikMenuPane* aMenuPane );

    /**
     * The value given will be used as a guideline to wether to show or hide the item.
     * If music shop feature is not supported and this is called with ETrue it will
     * not show the item unless forced state is used.
     * 
     * @param aEnable       Whether enable or disable the item
     * @param aForce        Whether the state is forced, by default it is not
     */
    void EnableMusicStore( TBool aEnable, TBool aForce = EFalse );

    /**
     * Launches Music store based on command id.
     * 
     * @param aCommandId    Command id for menu item
     * @param aArtist       Artist of the single
     * @param aAlbum        Album of the single
     * @param aTitle        Name of the single
     */
    void LaunchMusicStoreL( TInt aCommandId,
            const TDesC& aTitle,
            const TDesC& aArtist,
            const TDesC& aAlbum );

    /**
     * Launches web page based on given string.
     * 
     * @param aWebpage      Web site to be launched
     */
    void LaunchWebPageL( const TDesC& aWebpage );
    
    /**
     * Is Nokia Music shop available
     * @returns ETrue if available
     */
    TBool NokiaMusicStoreAvailable();

    /**
     * Is Operator music store available
     * @returns ETrue if available
     */
    TBool OperatorMusicStoreAvailable();
    
    /**
     * Check if the music store functionality has been enabled
	 * @return state
     */
    TBool IsMusicStoreEnabled();
    
    /** 
     * returns name of the operator music store
     * @return name
     */
    const TDesC& OperatorMusicStoreName() const;

private:
    /**
     * Default C++ constuctor
     * 
     * @param aResourceId   Menu pane where the music store item/items
     *                      will be added.
     */
    CFMRadioMusicStoreHandler( TInt aResourceId );

    /**
     * Second-phase constructor
     */
    void ConstructL();

    /**
     * Finds out the type of operator specific music store.
     * 
     * @param aRepository   Cenrep from which information is sought.
     * @returns Application type if available, otherwise EFMRadioAppNotAvailable
     */
    TFMRadioOperatorAppType CheckOperatorMusicStoreTypeL( CRepository& aRepository );

    /**
     * Called for application type specific initialization.
     * 
     * @param aRepository   Cenrep from which information is sought.
     * @param aAppType      Type of implementation to be initialized.
     */
    void InitializeParametersL( CRepository& aRepository, TFMRadioOperatorAppType aAppType );

    /**
     * Called when native music store needs initializing.
     * 
     * @param aMusicstoreUid    P&S UID for initializing music shop
     */
    void InitializeMusicStore( TUid aMusicstoreUid );

    /**
     * Returns the count of active music stores.
     * 
     * @returns Active music store count
     */
    TInt MusicStoreCount();

    /**
     * Creates the search string for Nokia Music Shop application
     * 
     * @param aArtistName   Artist of the single
     * @param aAlbumName    Album of the single
     * @param aSongName     Name of the single
     * @returns search string and pushes it to cleanup stack.
     */
    HBufC* NokiaMusicShopSearchLC( const TDesC& aSongName,
            const TDesC& aArtistName,
            const TDesC& aAlbumName );

    /**
     * Handles calling appropriate operator music store implementation
     * 
     * @param aSearchString     string used for searching
     */
    void LaunchOperatorMusicStoreL( const TDesC& aSearchString );

    /**
     * Launches Nokia Music shop application
     * 
     * @param aMusicshopUid     UID of the music shop
     * @param aSearchString     string used for searching
     */
    void LaunchMusicShopL( TUid aMusicshopUid, const TDesC& aSearchString );
    
    /**
     * Converts binary data from cenrep to Uint32 presentation
     * @param aBuf binary data from cenrep
     * @return resolved Uint32 number
     */
    TUint32 Uint32Presentation( const TDesC8& aBuf );
    
    /**
     * Checks if the application with the given uid is installed to the system
     * @param aAppUid Application uid
     * @return true if app is found
     */
    TBool IsAppInstalledL( const TUid& aAppUid );

private: // members
    /**
     * Resource id to which music store option will be bound.
     */
    TInt iResourceId;

    /**
     * This is used for with isMenuInitialized so that it is not needed
     * to go through all items in case we don't find the music store item.
     */
    TInt iFirstKnownResourceId;

    /**
     * Saves the state of Music store item
     */
    TFMRadioMusicStoreState iState;

    /**
     * Nokia Music store UID
     */
    TUint32 iMusicStoreUID;

    /**
     * Display name for operator music store
     */
    RBuf iOperatorMusicStoreName;

    /**
     * UID for Operator music store (native app.)
     */
    TUint32 iOperatorStoreNativeUID;

    /**
     * Website where Operator music store can be found
     */
    RBuf iOperatorStoreWebsite;

    /**
     * Control environment.
     */
    CCoeEnv* iCoeEnv;
    
    /**
     * flag for music store application availability 
     */
    TBool iMusicStoreAppInstalled;
    };

#endif /*FMRADIOMUSICSTOREHANDLER_H*/
