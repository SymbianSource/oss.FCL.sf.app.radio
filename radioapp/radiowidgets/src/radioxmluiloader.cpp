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
#include "radioxmluiloader.h"
#include "radiomainwindow.h"
#include "radiostationsview.h"
#include "radiotuningview.h"
#include "radiowizardview.h"
#include "radioplaylogview.h"
#include "radiostationcarousel.h"
#include "radiostationcontrolwidget.h"
#include "radiofrequencystrip.h"
#include "radiobannerlabel.h"
#include "radiofadinglabel.h"
#include "radiocontextmenu.h"
#include "radiostationmodel.h"
#include "radiouiengine.h"

/*!
 *
 */
RadioXmlUiLoader::RadioXmlUiLoader( RadioMainWindow& mainWindow ) :
    HbDocumentLoader(),
    mMainWindow( mainWindow )
{
}

/*!
 *
 */
QObject* RadioXmlUiLoader::createObject( const QString& type, const QString &name )
{
    QObject* object = 0;

    if ( type == RadioTuningView::staticMetaObject.className() )
    {
        object = new RadioTuningView( this );
    }
    else if ( type == RadioStationCarousel::staticMetaObject.className() )
    {
        object = new RadioStationCarousel( mMainWindow.uiEngine() );
    }
    else if ( type == RadioStationControlWidget::staticMetaObject.className() )
    {
        object = new RadioStationControlWidget( mMainWindow.uiEngine() );
    }
    else if ( type == RadioFrequencyStrip::staticMetaObject.className() )
    {
        RadioUiEngine& engine = mMainWindow.uiEngine();
        object = new RadioFrequencyStrip( engine.minFrequency(),
                                          engine.maxFrequency(),
                                          engine.frequencyStepSize(),
                                          engine.currentFrequency(),
                                          &engine );
    }
    else if ( type == RadioStationsView::staticMetaObject.className() )
    {
        object = new RadioStationsView( this );
    }
    else if ( type == RadioBannerLabel::staticMetaObject.className() )
    {
        object = new RadioBannerLabel();
    }
    else if ( type == RadioFadingLabel::staticMetaObject.className() )
    {
        object = new RadioFadingLabel();
    }
    else if ( type == RadioWizardView::staticMetaObject.className() )
    {
        object = new RadioWizardView( this );
    }
    else if ( type == RadioContextMenu::staticMetaObject.className() )
    {
        object = new RadioContextMenu( mMainWindow.uiEngine() );
    }
    else if ( type == RadioPlayLogView::staticMetaObject.className() )
    {
        object = new RadioPlayLogView( this );
    }

    if ( object )
    {
        object->setObjectName( name );
        return object;
    }

    return HbDocumentLoader::createObject( type, name );
}
