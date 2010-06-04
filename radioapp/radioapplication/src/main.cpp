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
#include <HbTranslator>

// User includes
#include "radioapplication.h"
#include "radiologger.h"

// Constants
#ifdef BUILD_WIN32
    const char* TRANSLATIONS_FOLDER = "";
#else
    const char* TRANSLATIONS_FOLDER = "/resource/qt/translations/";
#endif // BUILD_WIN32

/*!
 * Runs the application
 */
int main( int argc, char* argv[] )
{
    INSTALL_MESSAGE_HANDLER // Installs the file tracer if enabled

    LOG_TIMESTAMP( "Tesla started" );

    RadioApplication app( argc, argv );

    HbTranslator translator( TRANSLATIONS_FOLDER, "fmradio" ); // TODO: Rename exe to fmradio.exe to get rid of this

    const int returnValue = app.exec();

    UNINSTALL_MESSAGE_HANDLER // Uninstalls the file tracer

    return returnValue;
}
