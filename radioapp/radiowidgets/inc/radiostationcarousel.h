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
#include <HbScrollArea>
#include <HbIcon>
#include <QPointer>

// User includes
#include "radiowidgetsexport.h"
#include "radio_global.h"

// Forward declarations
class RadioUiEngine;
class RadioUiLoader;
class RadioCarouselItem;
class RadioStation;
class RadioStationModel;
class RadioStationCarousel;
class RadioCarouselAnimator;
class HbLabel;

namespace CarouselInfoText
{
    enum Type
    {
        None,
        ConnectAntenna,
        NoFavorites,
        FavoriteIconHint,
        Seeking,
        Scanning,
        ManualSeek
    };
}

// Class declaration
class WIDGETS_DLL_EXPORT RadioStationCarousel : public HbScrollArea
{
    Q_OBJECT
    Q_PROPERTY(HbIcon favoriteIcon READ favoriteIcon WRITE setFavoriteIcon)
    Q_PROPERTY(HbIcon nonFavoriteIcon READ nonFavoriteIcon WRITE setNonFavoriteIcon)
    Q_PROPERTY( int autoScrollTime READ autoScrollTime WRITE setAutoScrollTime )

    friend class RadioCarouselAnimator;
    friend class RadioCarouselItem;

public:

    enum ScrollFlag
    {
        Default             = 0,
        NoAnim              = 1 << 0,
        NoSignal            = 1 << 1,
        IgnoreCenter        = 1 << 2,
        UpdateItem          = 1 << 3,
        FromPanGesture      = 1 << 4,
        FromSwipeGesture    = 1 << 5
    };
    Q_DECLARE_FLAGS( ScrollMode, ScrollFlag )

    RadioStationCarousel( QGraphicsItem* parent = 0 );

    void setFavoriteIcon( const HbIcon& favoriteIcon );
    HbIcon favoriteIcon() const;

    void setNonFavoriteIcon( const HbIcon& nonFavoriteIcon );
    HbIcon nonFavoriteIcon() const;

    void setAutoScrollTime( const int time );
    int autoScrollTime() const;

    void init( RadioUiLoader& uiLoader, RadioUiEngine* uiEngine );

    void setFrequency( uint frequency, int reason, Scroll::Direction direction = Scroll::Shortest );

    RadioUiEngine* uiEngine();

    bool isAntennaAttached() const;

    void setScanningMode( bool scanning );
    bool isInScanningMode() const;

    void cleanRdsData();

    void animateNewStation( const RadioStation& station );
    void cancelAnimation();

    void setInfoText( CarouselInfoText::Type type );
    void clearInfoText();

    void setManualSeekMode( bool manualSeekActive );

    void setAlternateSkippingMode( bool alternateSkipping ); //TODO: Remove this! This is test code

signals:

    void frequencyChanged( uint frequency, int reason, int direction );
    void skipRequested( int skipMode );
    void scanAnimationFinished();

public slots:

    void updateAntennaStatus( bool connected );

private slots:

    void scrollPosChanged( const QPointF& newPosition );
    void adjustAfterScroll();
    void update( const RadioStation& station );
    void updateRadioText( const RadioStation& station );
    void updateStations();
    void timerFired();
    void toggleFavorite();
//    void openContextMenu( HbAbstractViewItem* item, const QPointF& coords );

#ifdef USE_DEBUGGING_CONTROLS
    void setRdsAvailable( bool available );
#endif // USE_DEBUGGING_CONTROLS

private:

// from base class QGraphicsItem

    void mousePressEvent( QGraphicsSceneMouseEvent* event );
    void resizeEvent( QGraphicsSceneResizeEvent* event );
    void showEvent( QShowEvent* event );
    void gestureEvent( QGestureEvent* event );

// New functions

    bool isInitialized() const;

    void setCenterIndex( int index, ScrollMode mode = Default );

    void scrollToIndex( int index, Scroll::Direction direction = Scroll::Shortest,
                        ScrollMode mode = Default );

    int calculateDifference( int targetIndex, Scroll::Direction& direction );

    bool isScrollingAllowed() const;

    void adjustPos( int offset );

    int trimIndex( int index );

    int prevIndex( int index );
    int nextIndex( int index );

    void skip( StationSkip::Mode mode );

private: // data

    enum TimerMode { NoTimer, SetFrequency, RtPlusCheck, InfoText, FavoriteHintShow, FavoriteHintHide };

    RadioUiEngine*                      mUiEngine;

    int                                 mAutoScrollTime;

    HbIcon                              mFavoriteIcon;
    HbIcon                              mNonFavoriteIcon;

    QTimer*                             mGenericTimer;
    TimerMode                           mTimerMode;

    QString                             mRadioTextHolder;

    QPointer<RadioCarouselAnimator>     mAnimator;

    HbLabel*                            mInfoText;

    HbWidget*                           mContainer;

    enum CarouselItem                   { LeftItem, CenterItem, RightItem };
    RadioCarouselItem*                  mItems[3];

    int                                 mMidScrollPos;
    int                                 mMaxScrollPos;

    int                                 mCurrentIndex;
    int                                 mTargetIndex;
    bool                                mIsCustomFreq;

    CarouselInfoText::Type              mInfoTextType;

    RadioStationModel*                  mModel;

    bool                                mPosAdjustmentDisabled;

    Scroll::Direction                   mScrollDirection;

    bool                                mManualSeekMode;

    bool                                mAlternateSkipping;
	
#ifdef USE_DEBUGGING_CONTROLS
    RadioFadingLabel*                   mRdsLabel;
#endif

};

Q_DECLARE_OPERATORS_FOR_FLAGS( RadioStationCarousel::ScrollMode )

#endif // RADIOSTATIONCAROUSEL_H
