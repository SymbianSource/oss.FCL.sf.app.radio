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

// System includes
#include <HbIcon>
#include <QScopedPointer>

// User includes
#include "radioviewbase.h"
#include "radiowidgetsexport.h"

// Forward declarations
class RadioXmlUiLoader;
class HbListView;
class HbAction;
class HbAbstractViewItem;
class RadioHistoryModel;
class RadioHistoryItem;

// Class declaration
class WIDGETS_DLL_EXPORT RadioHistoryView : public RadioViewBase
{
    Q_OBJECT
    Q_PROPERTY(HbIcon nonTaggedIcon READ nonTaggedIcon WRITE setNonTaggedIcon)
    Q_PROPERTY(HbIcon taggedIcon READ taggedIcon WRITE setTaggedIcon)

public:

    RadioHistoryView();
    ~RadioHistoryView();

    void setNonTaggedIcon( const HbIcon& nonTaggedIcon );
    HbIcon nonTaggedIcon() const;

    void setTaggedIcon( const HbIcon& taggedIcon );
    HbIcon taggedIcon() const;

private slots:

    void deckButtonPressed();
    void clearList();
    void updateVisibilities();
    void showContextMenu( const QModelIndex& index );
    void toggleTagging();
    void openOviStore();
    void openOtherStore();
    void addSongs();            // Temporary test code
    void addOneSong();          // Temporary test code

private:

// from base class RadioViewBase

    void init();
    void setOrientation();
    void userAccepted();

// New functions

    RadioHistoryModel& historyModel() const;

private: //data

    HbListView*                         mHistoryList;
    HbAction*                           mAllSongsButton;
    HbAction*                           mTaggedSongsButton;
    QScopedPointer<RadioHistoryItem>    mSelectedItem;
    int                                 mCurrentRow;
    HbIcon                              mNonTaggedIcon;
    HbIcon                              mTaggedIcon;

    int                                 mSongIndex; // Temporary test variable

};

#endif // RADIOHISTORYVIEW_H
