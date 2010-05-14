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
#include "radio_global.h"

// Forward declarations
class RadioUiEngine;
class RadioUiLoader;
class RadioStation;
class RadioStationItem;
class RadioStationCarousel;
class RadioCarouselModel;
class RadioFadingLabel;
class QTimeLine;
class HbLabel;

namespace CarouselInfoText
{
    enum Type
    {
        None,
        ConnectAntenna,
        NoFavorites,
        Seeking,
        Scanning
    };
}

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

    enum ScrollFlag
    {
        Default         = 0,
        NoAnim          = 1 << 0,
        NoSignal        = 1 << 1,
        UpdateItem      = 1 << 2
    };
    Q_DECLARE_FLAGS( ScrollMode, ScrollFlag )

    RadioStationCarousel( RadioUiEngine* uiEngine = 0 );

    void setFavoriteIcon( const HbIcon& favoriteIcon );
    HbIcon favoriteIcon() const;

    void setNonFavoriteIcon( const HbIcon& nonFavoriteIcon );
    HbIcon nonFavoriteIcon() const;

    void setAutoScrollTime( const int time );
    int autoScrollTime() const;

    void init( RadioUiLoader& uiLoader, RadioUiEngine* uiEngine );

    void setCarouselModel( RadioCarouselModel* carouselModel );

    void setFrequency( uint frequency, int reason );

    RadioUiEngine* uiEngine();

    bool isAntennaAttached() const;

    void setScanningMode( bool scanning );
    bool isInScanningMode() const;

    void cleanRdsData();

    void updateCurrentItem();

    void animateNewStation( const RadioStation& station );

    void setItemVisible( bool visible );

    void setInfoText( CarouselInfoText::Type type );
    void clearInfoText();

signals:

    void frequencyChanged( uint frequency, int reason );
    void scanAnimationFinished();

public slots:

    void updateAntennaStatus( bool connected );

private slots:

    void update( const RadioStation& station );
    void updateRadioText( const RadioStation& station );
    void insertFrequency( const QModelIndex& parent, int first, int last );
    void prepareToRemoveFrequency( const QModelIndex& parent, int first, int last );
    void removeFrequency( const QModelIndex& parent, int first, int last );
    void updateFrequencies();
    void timerFired();
    void openContextMenu( HbAbstractViewItem* item, const QPointF& coords );

#ifdef USE_DEBUGGING_CONTROLS
    void setRdsAvailable( bool available );
#endif // USE_DEBUGGING_CONTROLS

private:

// from base class QGraphicsItem

    void mousePressEvent( QGraphicsSceneMouseEvent* event );

    void gestureEvent( QGestureEvent* event );

// New functions

    void initToLastTunedFrequency();

    void updateClampingStyle();

    void initCurrentStationItem();

    RadioStationItem* currentStationItem();

    RadioCarouselModel* carouselModel() const;

    void scrollToIndex( const QModelIndex& index, ScrollMode mode = Default );

    void updatePos( int offset );

    void skip( StationSkip::Mode mode );

private: // data

    enum TimerMode { NoTimer, RtPlusCheck, InfoText };

    RadioUiEngine*          mUiEngine;

    bool                    mAntennaAttached;

    int                     mAutoScrollTime;

    QMap<uint,QModelIndex>  mModelIndexes;

    HbIcon                  mFavoriteIcon;
    HbIcon                  mNonFavoriteIcon;

    QTimer*                 mGenericTimer;
    TimerMode               mTimerMode;

    QString                 mRadioTextHolder;

    ScanningHelper*         mScanningHelper;

    HbLabel*                mInfoText;

    RadioStationItem*       mCurrentItem;

    CarouselInfoText::Type  mInfoTextType;

    int                     mPanStartPos;
	
#ifdef USE_DEBUGGING_CONTROLS
    RadioFadingLabel*       mRdsLabel;
#endif

};

Q_DECLARE_OPERATORS_FOR_FLAGS( RadioStationCarousel::ScrollMode )

#endif // RADIOSTATIONCAROUSEL_H
