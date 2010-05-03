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

#ifndef RADIOHISTORYVIEW_H
#define RADIOHISTORYVIEW_H

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
class WIDGETS_DLL_EXPORT RadioHistoryView : public RadioViewBase
{
    Q_OBJECT

public:

    explicit RadioHistoryView();

private slots:

    void deckButtonPressed();
    void clearList();
    void updateVisibilities();
    void listItemClicked( const QModelIndex& index );
    void listItemLongPressed( HbAbstractViewItem* item, const QPointF& coords );

private:

// from base class RadioViewBase

    void init( RadioXmlUiLoader* uiLoader, RadioMainWindow* mainWindow );
    void setOrientation();

// New functions

    void showContextMenu( const QModelIndex& index );

private: //data

    HbListView*                 mHistoryList;
    HbAction*                   mAllSongsButton;
    HbAction*                   mTaggedSongsButton;
    RadioStationFilterModel*    mFilterModel;

};

#endif // RADIOHISTORYVIEW_H
