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
#include <QTranslator>
#include <QLocale>
#include <QFile>

// User includes
#include "radioapplication.h"
#include "radiologger.h"

static const QString TRANSLATIONS_PATH_FORMAT = "%1:/resource/qt/translations/";

/*!
 * Runs the application
 */
int main( int argc, char* argv[] )
{
    INSTALL_MESSAGE_HANDLER // Installs the file tracer if enabled

    LOG_TIMESTAMP( "Tesla started" );

    const QString localizedRadio = "fmradio_" + QLocale::system().name();
    QString path = QString( TRANSLATIONS_PATH_FORMAT ).arg( "C" );  
    if ( !QFile::exists( path + localizedRadio ) ) {
        path = QString( TRANSLATIONS_PATH_FORMAT ).arg( "Z" );
    }
    
    QTranslator translator;
    translator.load( localizedRadio, path );
   
    RadioApplication app( argc, argv );
    app.installTranslator( &translator );
    int returnValue = app.exec();

    UNINSTALL_MESSAGE_HANDLER // Uninstalls the file tracer

    return returnValue;
}
