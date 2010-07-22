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

#ifndef FREQUENCYSTRIP_H
#define FREQUENCYSTRIP_H

// System includes
#include <QMap>
#include <QHash>
#include <HbEffect>
#include <QColor>

// User includes
#include "radiostripbase.h"
#include "radio_global.h"
#include "radiowidgetsexport.h"

// Forward declarations
class RadioUiEngine;
class RadioFrequencyItem;
class RadioStation;
class HbPushButton;
class QTimer;
class QModelIndex;
class RadioUiLoader;

typedef QList<RadioStation> StationList;

// Class declaration
class WIDGETS_DLL_EXPORT RadioFrequencyStrip : public RadioStripBase
{
    Q_OBJECT
    Q_PROPERTY(int itemHeight READ itemHeight WRITE setItemHeight)
    Q_PROPERTY(uint frequency READ frequency)

    friend class RadioFrequencyItem;

public:

    RadioFrequencyStrip();

    void setItemHeight( int itemHeight );
    int itemHeight() const;

    void init( RadioUiEngine* engine, RadioUiLoader& uiLoader );

    void setFrequency( const uint frequency, int reason, Scroll::Direction direction = Scroll::Shortest );
    uint frequency() const;

    bool isInManualSeekMode() const;
    void cancelManualSeek();

    void addScannedStation( const RadioStation& station );

    void updateFavorite( const RadioStation& station );

public slots:

    void setScanningMode( bool isScanning );

signals:

    void frequencyChanged( uint frequency, int reason, int direction );
    void skipRequested( int skipMode );
    void seekRequested( int seekDirection );
    void manualSeekChanged( bool manualSeek );

private slots:

    void removeStation( const QModelIndex& parent, int first, int last );
    void updateStation( const QModelIndex& parent, int first, int last );
    void initEmptyItems();
    void handleLeftButton();
    void handleLongLeftButton();
    void handleRightButton();
    void handleLongRightButton();
    void toggleManualSeek();
    void handleScrollingEnd();

private:

// from base class RadioStripBase

    void updateItemPrimitive( QGraphicsItem* itemToUpdate, int itemIndex );
    QGraphicsItem* createItemPrimitive( QGraphicsItem *parent );
    void scrollPosChanged();

// from base class QGraphicsWidget

    void resizeEvent ( QGraphicsSceneResizeEvent* event );
//    void showEvent( QShowEvent* event );
    void changeEvent( QEvent* event );

// from base class HbScrollArea

    void mousePressEvent( QGraphicsSceneMouseEvent* event );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );
    void gestureEvent( QGestureEvent* event );

// from base class QObject

    void timerEvent( QTimerEvent* event );

// New functions

    void initModel();

    void initSelector();

    void initPositions();

    void initButtons();

    void addFrequencyPos( int pos, uint frequency, RadioFrequencyItem* item );

    void updateStationsInRange( int first, int last, bool stationRemoved = false );

    void updateItem( RadioFrequencyItem* item, uint upperRange = 0, uint ignoredFrequency = 0 );

    void updateAllItems();

    QPixmap drawPixmap( uint frequency, QList<RadioStation> stations, RadioFrequencyItem* item );

    QLineF makeTab( qreal pos, int height );

    void emitFrequencyChanged( uint frequency );

    int selectorPos() const;

    void scrollToFrequency( uint frequency,
                            Scroll::Direction direction = Scroll::Shortest,
                            int time = 0,
                            TuneReason::Reason reason = TuneReason::Unspecified );

    void hideButtons();
    void showButtons();

private: // data

    class FrequencyPos
    {
    public:
        explicit FrequencyPos( int pos, RadioFrequencyItem* item ) :
            mPosition( pos ),
            mItem( item ) {}

        FrequencyPos() :
            mPosition( 0 ),
            mItem( 0 ) {}

        int                     mPosition;
        RadioFrequencyItem*     mItem;

    };

    RadioUiEngine*              mUiEngine;

    int                         mItemHeight;

    uint                        mMinFrequency;

    uint                        mMaxFrequency;

    uint                        mFrequencyStepSize;

    uint                        mFrequency;

    QGraphicsPixmapItem*        mSelectorImage;

    qreal                       mSeparatorPos;

    short                       mMaxWidth;

    qreal                       mSelectorPos;

    QList<RadioFrequencyItem*>  mFrequencyItems;

    /**
     * Container to help map a frequency to strip position and additional information about the frequency.
     * In the default region contains an item for every valid frequency from 87.50 Mhz to 108.00 Mhz with
     * stepsize 50 Khz which amounts to 410 items.
     */
    QMap<uint,FrequencyPos>     mFrequencies;

    /**
     * Container to help map strip position to frequency.
     * It is queried every time the strip moves so the lookup time needs to be fast. QHash is used because
     * it offers the fastest lookup time of any other QT container. QHash stores the items in arbitrary
     * order so we must never try to loop through them and assume they are in any particular order.
     */
    QHash<int,uint>             mPositions;

    HbPushButton*               mLeftButton;

    HbPushButton*               mRightButton;

    QTimer*                     mManualSeekTimer;

    bool                        mManualSeekMode;

    uint                        mLastReportedFrequency;

    int                         mManualSeekTimerId;

    QColor                      mForegroundColor;

};

#endif // FREQUENCYSTRIP_H
