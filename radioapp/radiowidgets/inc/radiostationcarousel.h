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

#ifndef RADIOSTATIONCAROUSEL_H
#define RADIOSTATIONCAROUSEL_H

// System includes
#include <hbgridview.h>
#include <hbabstractviewitem.h>
#include <qmap>
#include <qabstractitemmodel>
#include <hbicon.h>

// Forward declarations
class RadioUiEngine;
class RadioFadingLabel;
class HbPushButton;
class HbAnchorLayout;
class RadioStation;
class RadioStationCarousel;
class RadioStationFilterModel;

// Class declaration
class RadioStationItem : public HbAbstractViewItem
{
    Q_OBJECT
    friend class RadioStationCarousel;

public:

    RadioStationItem( QGraphicsItem* parent = 0 );

// From base class HbAbstractViewItem

    HbAbstractViewItem* createItem();
    void updateChildItems();

// New functions

    uint frequency() const;

    void update( const RadioStation* station = 0 );

    void setFrequency( uint frequency );
    void setSeekingText();

private slots:

    void toggleFavorite();

private:

    void updateFavoriteIcon( bool isFavorite );

    RadioStationCarousel* carousel();

private: // data

    HbAnchorLayout*         mLayout;
    RadioFadingLabel*       mNameLabel;
    HbPushButton*           mIconButton;
    RadioFadingLabel*       mGenreLabel;
    RadioFadingLabel*       mRadiotextLabel;
    uint                    mFrequency;
};

// Class declaration
class RadioStationCarousel : public HbGridView
{
    Q_OBJECT
    Q_PROPERTY(HbIcon background READ background WRITE setBackground)

public:

    RadioStationCarousel( RadioUiEngine& uiEngine, QGraphicsItem* parent = 0 );

    void setBackground( const HbIcon& background );
    HbIcon background() const;

    RadioUiEngine& uiEngine();

    bool isAntennaAttached() const;

signals:

    void frequencyChanged( uint frequency, int commandSender );

public slots:

    void setFrequency( uint frequency );
    void setSeekingText();
    void updateHeadsetStatus( bool connected );

private slots:

    void update( const RadioStation& station );
    void leftGesture( int speedPixelsPerSecond );
    void rightGesture( int speedPixelsPerSecond );
    void insertFrequency( const QModelIndex& parent, int first, int last );
    void removeFrequency( const QModelIndex& parent, int first, int last );
    void updateFrequencies();
    void updateLoopedPos();

private:

// from base class QGraphicsItem

    void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );

    void mouseMoveEvent( QGraphicsSceneMouseEvent* event );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );
    void resizeEvent( QGraphicsSceneResizeEvent* event );

    RadioStationItem* currentStationItem();

    RadioStationFilterModel* filterModel() const;

    void scrollToIndex( const QModelIndex& index, int time );

private: // data

    RadioUiEngine&          mUiEngine;

    bool                    mAntennaAttached;

    int                     mAutoScrollTime;

    qreal                   mPreviousButtonPos;

    bool                    mMovingLeft;

    RadioStationItem*       mCurrentItem;

    QMap<uint,QModelIndex>  mModelIndexes;

    HbIcon                  mBackground;

};

#endif // RADIOSTATIONCAROUSEL_H
