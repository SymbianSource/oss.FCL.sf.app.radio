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

#ifndef RADIOSTATIONITEM_H
#define RADIOSTATIONITEM_H

// System includes
#include <HbAbstractViewItem>

// Forward declarations
class RadioFadingLabel;
class HbPushButton;
class RadioStation;
class HbAnchorLayout;
class RadioStationCarousel;

// Class declaration
class RadioStationItem : public HbAbstractViewItem
{
    Q_OBJECT
    friend class RadioStationCarousel;

public:

    RadioStationItem( RadioStationCarousel& carousel );

// From base class HbAbstractViewItem

    HbAbstractViewItem* createItem();
    void updateChildItems();

// New functions

    uint frequency() const;
    void update( const RadioStation* station = 0 );
    void setFrequency( uint frequency );
    void cleanRdsData();
    void handleLongPress( const QPointF& coords );

private slots:

    void toggleFavorite();

private:

    void updateFavoriteIcon( bool isFavorite );

    RadioStationCarousel* carousel();

    QString parseFrequency( const uint frequency );
    QString nameOrFrequency( const RadioStation& station, uint frequency = 0 );

private: // data

    RadioStationCarousel&   mCarousel;
    HbAnchorLayout*         mLayout;
    RadioFadingLabel*       mNameLabel;
    HbPushButton*           mIconButton;
    RadioFadingLabel*       mGenreLabel;
    RadioFadingLabel*       mRadiotextLabel;
    RadioFadingLabel*       mUrlLabel;
    uint                    mFrequency;
};


#endif // RADIOSTATIONITEM_H
