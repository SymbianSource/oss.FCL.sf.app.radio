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
#include <HbGridView>
#include <QMap>
#include <QAbstractItemModel>
#include <HbIcon>

// Forward declarations
class RadioUiEngine;
class RadioStation;
class RadioStationItem;
class RadioStationCarousel;
class RadioStationFilterModel;
class RadioFadingLabel;

// Class declaration
class RadioStationCarousel : public HbGridView
{
    Q_OBJECT
    Q_PROPERTY(HbIcon background READ background WRITE setBackground)
    Q_PROPERTY(HbIcon favoriteIcon READ favoriteIcon WRITE setFavoriteIcon)
    Q_PROPERTY(HbIcon nonFavoriteIcon READ nonFavoriteIcon WRITE setNonFavoriteIcon)

public:

    RadioStationCarousel( RadioUiEngine& uiEngine, QGraphicsItem* parent = 0 );

    void setBackground( const HbIcon& background );
    HbIcon background() const;

    void setFavoriteIcon( const HbIcon& favoriteIcon );
    HbIcon favoriteIcon() const;

    void setNonFavoriteIcon( const HbIcon& nonFavoriteIcon );
    HbIcon nonFavoriteIcon() const;

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
    void updateRadioText( const RadioStation& station );
    void leftGesture( int speedPixelsPerSecond );
    void rightGesture( int speedPixelsPerSecond );
    void insertFrequency( const QModelIndex& parent, int first, int last );
    void removeFrequency( const QModelIndex& parent, int first, int last );
    void updateFrequencies();
    void updateLoopedPos();
    void radioTextPlusCheckEnded();

#ifdef USE_DEBUGGING_CONTROLS
    void setRdsAvailable( bool available );
#endif // USE_DEBUGGING_CONTROLS

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
    HbIcon                  mFavoriteIcon;
    HbIcon                  mNonFavoriteIcon;

    QTimer*                 mRadioTextTimer;

    QString                 mRadioTextHolder;
	
#ifdef USE_DEBUGGING_CONTROLS
    RadioFadingLabel*       mRdsLabel;
#endif

};

#endif // RADIOSTATIONCAROUSEL_H
