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

#ifndef RADIOPLAYLOGVIEW_H
#define RADIOPLAYLOGVIEW_H

// User includes
#include "radioviewbase.h"
#include "radiowidgetsexport.h"

// Forward declarations
class RadioXmlUiLoader;
class HbListView;
class HbAction;
class RadioStationFilterModel;
class HbAbstractViewItem;

// Class declaration
class WIDGETS_DLL_EXPORT RadioPlayLogView : public RadioViewBase
{
    Q_OBJECT

public:

    explicit RadioPlayLogView( RadioXmlUiLoader* uiLoader );

private slots:

    void deckButtonPressed();
    void clearList();
    void updateVisibilities();
    void listItemClicked( const QModelIndex& index );
    void listItemLongPressed( HbAbstractViewItem* item, const QPointF& coords );

private:

// from base class RadioViewBase

    void init( RadioMainWindow* aMainWindow, RadioStationModel* aModel );
    void setOrientation();

// New functions

    void showContextMenu( const QModelIndex& index );

private: //data

    HbListView*         mPlayLogList;
    HbAction*           mAllSongsButton;
    HbAction*           mFavoritesButton;
    RadioStationFilterModel*   mFilterModel;

};

#endif // RADIOPLAYLOGVIEW_H
