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
#include <HbIcon>
#include <HbEffect>

// User includes
#include "radiostripbase.h"
#include "radio_global.h"

// Forward declarations
class RadioUiEngine;
class RadioFrequencyItem;
class RadioStation;
class HbPushButton;
class QTimer;

const int KOneHertz = KFrequencyMultiplier;
const int KHalfHertz = KOneHertz / 2;
const int KOneTabDistance = 15;
const uint KOneTabInHz = 0.2 * KOneHertz;
const qreal KPixelInHz = KOneTabInHz / KOneTabDistance;
//const int KCharWidth = 8;                  // TODO: Remove hardcoding
const int KWidth = KOneTabDistance * 5;
const int KHeight = 50;                 //TODO: Remove hardcoding

// Class declaration
class RadioFrequencyStrip : public RadioStripBase
{
    Q_OBJECT
    Q_PROPERTY( HbIcon leftButtonIcon READ leftButtonIcon WRITE setLeftButtonIcon )
    Q_PROPERTY( HbIcon rightButtonIcon READ rightButtonIcon WRITE setRightButtonIcon )

    friend class RadioFrequencyItem;

public:

    RadioFrequencyStrip( uint minFrequency,
                         uint maxFrequency,
                         uint stepSize,
                         uint currentFrequency,
                         RadioUiEngine* engine = 0 );

    void setLeftButtonIcon( const HbIcon& leftButtonIcon );
    HbIcon leftButtonIcon() const;

    void setRightButtonIcon( const HbIcon& rightButtonIcon );
    HbIcon rightButtonIcon() const;

    uint frequency( bool* favorite = 0 ) const;

    void connectLeftButton( const char* signal, QObject* receiver, const char* slot );
    void connectRightButton( const char* signal, QObject* receiver, const char* slot );



public slots:

    void favoriteChanged( const RadioStation& station );
    void stationAdded( const RadioStation& station );
    void stationRemoved( const RadioStation& station );
    void setFrequency( const uint frequency, int commandSender = 0 );

signals:

    void frequencyChanged( uint frequency, int sender ); // sender is always CommandSender::RadioFrequencyStrip
    void frequencyIsFavorite( bool favorite );
    void swipedLeft();
    void swipedRight();

private slots:

    void leftGesture( int speedPixelsPerSecond );
    void rightGesture( int speedPixelsPerSecond );
    void panGesture( const QPointF& point );
    void toggleButtons();

private:

// from base class RadioStripBase

    void updateItemPrimitive( QGraphicsItem* itemToUpdate, int itemIndex );
    QGraphicsItem* createItemPrimitive( QGraphicsItem *parent );
    void scrollPosChanged( QPointF newPosition );

// from base class QGraphicsWidget

    void resizeEvent ( QGraphicsSceneResizeEvent* event );
    void showEvent( QShowEvent* event );

// from base class HbScrollArea

    void mousePressEvent( QGraphicsSceneMouseEvent* event );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );

//    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);

// New functions

    void initModel();

    void initSelector();

    void initItems();

    void initButtons();

    void addFrequencyPos( int pos, uint frequency, RadioFrequencyItem* item );

    void updateFavorites( RadioFrequencyItem* item );

    QPixmap drawPixmap( uint frequency, QList<RadioStation> stations, RadioFrequencyItem* item );

    void emitFrequencyChanged( uint frequency );

    void emitFavoriteSelected( bool favoriteSelected );

    int selectorPos() const;

    void scrollToFrequency( uint frequency, int time = 0 );

private: // data

    class FrequencyPos
    {
    public:
        explicit FrequencyPos( int pos, bool favorite, bool localStation, RadioFrequencyItem* item ) :
            mPosition( pos ),
            mFavorite( favorite ),
            mLocalStation( localStation ),
            mItem( item ) {}


        FrequencyPos() :
            mPosition( 0 ),
            mFavorite( false ),
            mLocalStation( false ),
            mItem( 0 ) {}

        int                     mPosition;
        bool                    mFavorite;
        bool                    mLocalStation;
        RadioFrequencyItem*     mItem;

    };

    uint                        mMinFrequency;

    uint                        mMaxFrequency;

    uint                        mFrequencyStepSize;

    RadioUiEngine*              mEngine;

    QGraphicsPixmapItem*        mSelectorImage;

    qreal                       mSeparatorPos;

    short                       mMaxWidth;

    qreal                       mSelectorPos;

    uint                        mFrequency;

    QList<RadioFrequencyItem*>  mFrequencyItems;

    bool                        mFavoriteSelected;

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

    HbIcon                      mLeftButtonIcon;

    HbIcon                      mRightButtonIcon;

    HbPushButton*               mLeftButton;

    HbPushButton*               mRightButton;

    QTimer*                     mButtonTimer;

    bool                        mIsPanGesture;

};

#endif // FREQUENCYSTRIP_H
