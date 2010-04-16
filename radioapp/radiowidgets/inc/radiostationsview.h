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

#ifndef RADIOSTATIONSVIEW_H
#define RADIOSTATIONSVIEW_H

// System includes
#include <HbIcon>

// User includes
#include "radioviewbase.h"
#include "radiowidgetsexport.h"

// Forward declarations
class RadioMainWindow;
class RadioStationModel;
class HbListView;
class HbPushButton;
class RadioBannerLabel;
class HbAbstractViewItem;
class RadioXmlUiLoader;
class RadioStation;
class RadioStationFilterModel;

// Class declaration
class WIDGETS_DLL_EXPORT RadioStationsView : public RadioViewBase
{
    Q_OBJECT
    Q_PROPERTY(HbIcon nowPlayingIcon READ nowPlayingIcon WRITE setNowPlayingIcon)
    Q_PROPERTY(HbIcon favoriteIcon READ favoriteIcon WRITE setFavoriteIcon)

public:

    explicit RadioStationsView( RadioXmlUiLoader* uiLoader );

    void setNowPlayingIcon( const HbIcon& nowPlayingIcon );
    HbIcon nowPlayingIcon() const;

    void setFavoriteIcon( const HbIcon& favoriteIcon );
    HbIcon favoriteIcon() const;

private slots:

    void listItemClicked( const QModelIndex& index );
    void listItemLongPressed( HbAbstractViewItem* item, const QPointF& coords );
    void updateHeadsetStatus( bool connected );
    void updateCurrentStation();
    void deckButtonPressed();
    void startScanAndSavePresets();
    void updateControlVisibilities();

private:

// from base class RadioViewBase

    void init( RadioMainWindow* aMainWindow, RadioStationModel* aModel );

// from base class QGraphicsWidget

    void showEvent( QShowEvent* event );

// New functions

    void initListView();

    void updateHeading();

private: //data

    RadioStationFilterModel*    mFilterModel;

    HbAction*                   mScanStationsAction;
    HbAction*                   mRemoveAllPresetsAction;

    HbListView*                 mStationsList;

    RadioBannerLabel*           mHeadingBanner;

    HbAction*                   mFavoritesButton;
    HbAction*                   mLocalStationsButton;

    HbIcon                      mFavoriteIcon;

    HbIcon                      mNowPlayingIcon;

};

#endif // RADIOSTATIONSVIEW_H
