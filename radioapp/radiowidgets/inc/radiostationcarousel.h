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

// User includes
#include "radiowidgetsexport.h"

// Forward declarations
class RadioUiEngine;
class RadioStation;
class RadioStationItem;
class RadioStationCarousel;
class RadioStationModel;
class RadioFadingLabel;
class QTimeLine;

// Class declaration
class ScanningHelper : public QObject
{
    Q_OBJECT

public:

    ScanningHelper( RadioStationCarousel& carousel );

    void start();

private slots:

    void startSlide();
    void startNumberScroll();
    void numberScrollUpdate( int value );

public:

    RadioStationCarousel&   mCarousel;
    uint                    mCurrentFrequency;
    uint                    mPreviousFrequency;
    RadioStationItem*       mStationItem;
    QTimeLine*              mNumberScrollingTimeLine;
    QModelIndex             mModelIndex;

};

// Class declaration
class WIDGETS_DLL_EXPORT RadioStationCarousel : public HbGridView
{
    Q_OBJECT
    Q_PROPERTY(HbIcon favoriteIcon READ favoriteIcon WRITE setFavoriteIcon)
    Q_PROPERTY(HbIcon nonFavoriteIcon READ nonFavoriteIcon WRITE setNonFavoriteIcon)
    Q_PROPERTY( int autoScrollTime READ autoScrollTime WRITE setAutoScrollTime )

    friend class ScanningHelper;

public:

    RadioStationCarousel( RadioUiEngine* uiEngine = 0 );

    void setFavoriteIcon( const HbIcon& favoriteIcon );
    HbIcon favoriteIcon() const;

    void setNonFavoriteIcon( const HbIcon& nonFavoriteIcon );
    HbIcon nonFavoriteIcon() const;

    void setAutoScrollTime( const int time );
    int autoScrollTime() const;

    void init( RadioUiEngine* uiEngine );

    void setStationModel( RadioStationModel* stationModel );

    RadioUiEngine* uiEngine();

    bool isAntennaAttached() const;

    void setScanningMode( bool scanning );
    bool isInScanningMode() const;

    void cleanRdsData();
    void animateNewStation( const RadioStation& station );
    void setItemVisibility( bool visible );

signals:

    void frequencyChanged( uint frequency, int commandSender );
    void scanAnimationFinished();

public slots:

    void setFrequency( uint frequency );
    void updateAntennaStatus( bool connected );

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
    void delayedScroll();

#ifdef USE_DEBUGGING_CONTROLS
    void setRdsAvailable( bool available );
#endif // USE_DEBUGGING_CONTROLS

private:

// from base class QGraphicsItem

    void mouseMoveEvent( QGraphicsSceneMouseEvent* event );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );
    void resizeEvent( QGraphicsSceneResizeEvent* event );

    RadioStationItem* currentStationItem();

    RadioStationModel* stationModel() const;

    void scrollToIndex( const QModelIndex& index, int time );

private: // data

    RadioUiEngine*          mUiEngine;

    bool                    mAntennaAttached;

    int                     mAutoScrollTime;

    qreal                   mPreviousButtonPos;

    bool                    mMovingLeft;

    RadioStationItem*       mCurrentItem;

    QMap<uint,QModelIndex>  mModelIndexes;

    HbIcon                  mFavoriteIcon;
    HbIcon                  mNonFavoriteIcon;

    QTimer*                 mRadioTextTimer;

    QString                 mRadioTextHolder;
    
    int                     mScrollPos;

    ScanningHelper*         mScanningHelper;
	
#ifdef USE_DEBUGGING_CONTROLS
    RadioFadingLabel*       mRdsLabel;
#endif

};

#endif // RADIOSTATIONCAROUSEL_H
