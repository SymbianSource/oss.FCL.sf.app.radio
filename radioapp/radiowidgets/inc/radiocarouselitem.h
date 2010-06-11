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

#ifndef RADIOCAROUSELITEM_H
#define RADIOCAROUSELITEM_H

// System includes
#include <HbWidget>
#include <QScopedPointer>

// User includes

// Forward declarations
class RadioStation;
class RadioFadingLabel;
class HbPushButton;
class RadioStation;
class HbAnchorLayout;
class RadioStationCarousel;

// Class declaration
class RadioCarouselItem : public HbWidget
{
    friend class RadioStationCarousel;

    class Data;

public:

    RadioCarouselItem( RadioStationCarousel& carousel );
    ~RadioCarouselItem();

// New functions

    void setStation( const RadioStation& station );

    void swapData( RadioCarouselItem& other );

    uint frequency() const;
    void update( const RadioStation* station = NULL );
    void setFrequency( uint frequency );
    void cleanRdsData();
    void handleLongPress( const QPointF& coords );
    void setRadioText( const QString& text );

    void setSeekLayout( bool seekLayout );

    enum ItemVisibility{ AllVisible, AllHidden, IconVisible };
    void setItemVisibility( ItemVisibility visibility );

    void setIconOpacity( qreal opacity );

private:

    void updateFavoriteIcon( bool isFavorite );

    QString parseFrequency( const uint frequency );
    QString nameOrFrequency( const RadioStation& station, uint frequency = 0 );

private: // data

    class Data
    {
    public:
        Data();
        ~Data();

        QScopedPointer<RadioStation>        mStation;
        HbAnchorLayout*                     mLayout;
        QScopedPointer<RadioFadingLabel>    mNameLabel;
        QScopedPointer<HbPushButton>        mIconButton;
        QScopedPointer<RadioFadingLabel>    mGenreLabel;
        QScopedPointer<RadioFadingLabel>    mRadiotextLabel;
        QScopedPointer<RadioFadingLabel>    mUrlLabel;
        bool                                mSeekLayout;

    };

    RadioStationCarousel&   mCarousel;

    QScopedPointer<Data>    mData;

};

#endif // RADIOCAROUSELITEM_H
