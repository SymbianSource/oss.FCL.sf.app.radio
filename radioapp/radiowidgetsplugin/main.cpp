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
#include <QtPlugin>
#include <hbdocumentloaderplugin>

// User includes
#include "radiostationsview.h"
#include "radiotuningview.h"
#include "radiohistoryview.h"
#include "radiostationcarousel.h"
#include "radiofrequencystrip.h"
#include "radiobannerlabel.h"
#include "radiofadinglabel.h"

// Class declaration
class RadioWidgetsPlugin : public HbDocumentLoaderPlugin
{
public:

    QObject* createObject(const QString& type, const QString& name);
    QList<const QMetaObject*> supportedObjects();

};

Q_EXPORT_PLUGIN(RadioWidgetsPlugin)

/*!
 *
 */
QObject* RadioWidgetsPlugin::createObject( const QString& type, const QString& name )
{
    QObject* object = 0;

    if ( type == RadioTuningView::staticMetaObject.className() ) {
        object = new RadioTuningView();
    } else if ( type == RadioStationsView::staticMetaObject.className() ) {
        object = new RadioStationsView();
    } else if ( type == RadioHistoryView::staticMetaObject.className() ) {
        object = new RadioHistoryView();
    } else if ( type == RadioStationCarousel::staticMetaObject.className() ) {
        object = new RadioStationCarousel();
    } else if ( type == RadioFrequencyStrip::staticMetaObject.className() ) {
        object = new RadioFrequencyStrip();
    } else if ( type == RadioBannerLabel::staticMetaObject.className() ) {
        object = new RadioBannerLabel();
    } else if ( type == RadioFadingLabel::staticMetaObject.className() ) {
        object = new RadioFadingLabel();
    }

    if ( object ) {
        object->setObjectName( name );
    }

    return object;
}

/*!
 *
 */
QList<const QMetaObject *> RadioWidgetsPlugin::supportedObjects()
{
    QList<const QMetaObject *> result;
    result.append( &RadioTuningView::staticMetaObject );
    result.append( &RadioStationsView::staticMetaObject );
    result.append( &RadioHistoryView::staticMetaObject );
    result.append( &RadioBannerLabel::staticMetaObject );
    result.append( &RadioFadingLabel::staticMetaObject );
    result.append( &RadioFrequencyStrip::staticMetaObject );
    result.append( &RadioStationCarousel::staticMetaObject );
    return result;
}
