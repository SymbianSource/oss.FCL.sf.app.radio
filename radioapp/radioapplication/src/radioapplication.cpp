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

// User includes
#include "radioapplication.h"
#include "radiomainwindow.h"
#include "radiouiengine.h"
#include "radiolocalization.h"
#include "radio_global.h"
#include "radiologger.h"

#ifdef BUILD_WIN32
#   include "testwindow_win32.h"
#   define CREATE_WIN32_TEST_WINDOW \
        mWin32Window.reset( new Win32Window() ); \
        mMainWindow->setParent( mWin32Window.data() ); \
        mWin32Window->addHbWindow( mMainWindow.data() ); \
        mWin32Window->show();
#   define INIT_WIN32_TEST_WINDOW \
        mWin32Window->init();
#else
#   define CREATE_WIN32_TEST_WINDOW
#   define INIT_WIN32_TEST_WINDOW
#endif // BUILD_WIN32

/*!
 * Constructor
 */
RadioApplication::RadioApplication( int argc, char* argv[] ) :
    HbApplication( argc, argv ),
    mUiEngine( 0 )
{
    // Initializes the radio engine utils if UI logs are entered into the engine log
    INIT_COMBINED_LOGGER

    LOG_TIMESTAMP( "Start radio" );
    setApplicationName( TRANSLATE( KApplicationName ) );

    // MainWindow needs to be alive to be able to show the offline query dialog.
    // The window is only constructed half-way at this point because we may need to shut down if
    // offline usage is not allowed
    mMainWindow = RadioMainWindowPtr( new RadioMainWindow() );

    CREATE_WIN32_TEST_WINDOW

    if ( RadioUiEngine::isOfflineProfile() && !mMainWindow->isOfflineUsageAllowed() ) {
        quit();
        return;
    }

    // Start the engine
    mUiEngine = new RadioUiEngine( this );
    if ( !mUiEngine->startRadio() ) {
        mMainWindow->showErrorMessage( TRANSLATE( KErrorEngineStartFailed ) );
        quit();
        return;
    }

    INIT_WIN32_TEST_WINDOW

    // Construct the real views
    mMainWindow->init( mUiEngine );

    mMainWindow->show();
}

/*!
 *
 */
RadioApplication::~RadioApplication()
{
    // Destructor needs to be defined. See explanation from RadioEngineWrapperPrivate destructor.

    // Releases the radio engine utils if it was initialized in the beginning
    RELEASE_COMBINED_LOGGER
}
