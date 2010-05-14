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

// Class declaration
class WIDGETS_DLL_EXPORT RadioFrequencyStrip : public RadioStripBase
{
    Q_OBJECT
    Q_PROPERTY( HbIcon leftButtonIcon READ leftButtonIcon WRITE setLeftButtonIcon )
    Q_PROPERTY( HbIcon rightButtonIcon READ rightButtonIcon WRITE setRightButtonIcon )

    friend class RadioFrequencyItem;

public:

    RadioFrequencyStrip();

    void setLeftButtonIcon( const HbIcon& leftButtonIcon );
    HbIcon leftButtonIcon() const;

    void setRightButtonIcon( const HbIcon& rightButtonIcon );
    HbIcon rightButtonIcon() const;

    void init( RadioUiEngine* engine );

    void setFrequency( const uint frequency, int reason = 0 );
    uint frequency() const;

public slots:

    void updateFavorite( const RadioStation& station );
    void setScanningMode( bool isScanning );

signals:

    void frequencyChanged( uint frequency, int reason ); // reason is always CommandSender::RadioFrequencyStrip
    void skipRequested( int skipMode );
    void seekRequested( int seekDirection );

private slots:

    void updateStation( const QModelIndex& parent, int first, int last );
    void initEmptyItems();
    void handleLeftButton();
    void handleLongLeftButton();
    void handleRightButton();
    void handleLongRightButton();
    void toggleButtons();
    void checkIllegalPos();

private:

// from base class RadioStripBase

    void updateItemPrimitive( QGraphicsItem* itemToUpdate, int itemIndex );
    QGraphicsItem* createItemPrimitive( QGraphicsItem *parent );
    void scrollPosChanged( QPointF newPosition );

// from base class QGraphicsWidget

    void resizeEvent ( QGraphicsSceneResizeEvent* event );
    void showEvent( QShowEvent* event );
    void changeEvent( QEvent* event );

// from base class HbScrollArea

    void mousePressEvent( QGraphicsSceneMouseEvent* event );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );
    void gestureEvent( QGestureEvent* event );

// New functions

    void initModel();

    void initSelector();

    void initPositions();

    void initButtons();

    void addFrequencyPos( int pos, uint frequency, RadioFrequencyItem* item );

    void updateFavorites( RadioFrequencyItem* item );

    void updateItems();

    QPixmap drawPixmap( uint frequency, QList<RadioStation> stations, RadioFrequencyItem* item );

    void emitFrequencyChanged( uint frequency );

    int selectorPos() const;

    void scrollToFrequency( uint frequency, int time = 0 );

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

    HbIcon                      mLeftButtonIcon;

    HbIcon                      mRightButtonIcon;

    HbPushButton*               mLeftButton;

    HbPushButton*               mRightButton;

    QTimer*                     mButtonTimer;
    bool                        mButtonsVisible;

    bool                        mUserIsScrolling;

    QColor                      mForegroundColor;

};

#endif // FREQUENCYSTRIP_H
