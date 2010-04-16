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

#ifndef RADIOCONTEXTMENU_H_
#define RADIOCONTEXTMENU_H_

// System includes
#include <HbMenu>

// User includes
#include "radiostation.h"

// Forward declarations
class RadioUiEngine;
class RadioStation;
class RadioXmlUiLoader;

// Class declaration
class RadioContextMenu : public HbMenu
{
    Q_OBJECT

public:

    RadioContextMenu( RadioUiEngine& uiEngine, QGraphicsItem* parent = 0 );

    /*!
     * Initializes the menu
     * @param station Reference to the station that the menu handles
     * @param uiLoader Reference to UI loader
     */
    void init( const RadioStation& station, RadioXmlUiLoader& uiLoader );

private slots:

    void rename();
    void toggleFavorite();
    void deleteStation();

private:

// from base class QGraphicsWidget

    void resizeEvent( QGraphicsSceneResizeEvent* event );

private: // data

    /*!
     * Reference to the UI engine
     */
    RadioUiEngine&  mUiEngine;

    RadioStation    mStation;

    QPointF         mPos;

};

#endif // RADIOCONTEXTMENU_H_
