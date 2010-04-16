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

// System includes
#include <QGraphicsLinearLayout>
#include <HbAnchorLayout>
#include <QPixmap>
#include <QGraphicsSceneMouseEvent>
#include <HbEffect>
#include <QTimer>

// User includes
#include "radiostationcarousel.h"
#include "radiostationitem.h"
#include "radiouiengine.h"
#include "radiostationmodel.h"
#include "radiofadinglabel.h"
#include "radiologger.h"
#include "radiolocalization.h"
#include "radiostationfiltermodel.h"
#include "radio_global.h"

#ifdef USE_LAYOUT_FROM_E_DRIVE
    const QString KFavoriteIconPath = "e:/radiotest/images/favoriteiconactive.png";
    const QString KNonFavoriteIconPath = "e:/radiotest/images/favoriteiconinactive.png";
#else
    const QString KFavoriteIconPath = ":/images/favoriteiconactive.png";
    const QString KNonFavoriteIconPath = ":/images/favoriteiconinactive.png";
#endif

const int KRadioTextPlusCheckTimeout = 700; // 700 ms

/*!
 *
 */
RadioStationCarousel::RadioStationCarousel( RadioUiEngine& uiEngine, QGraphicsItem* parent ) :
    HbGridView( parent ),
    mUiEngine( uiEngine ),
    mAntennaAttached( false ),
    mAutoScrollTime( 1000 ),
    mPreviousButtonPos( 0.0 ),
    mMovingLeft( false ),
    mCurrentItem( 0 ),
    mRadioTextTimer( new QTimer( this ) )
#ifdef USE_DEBUGGING_CONTROLS
    ,mRdsLabel( new RadioFadingLabel( this ) )
#endif // USE_DEBUGGING_CONTROLS
{
    mAntennaAttached = mUiEngine.isAntennaAttached();

    setScrollDirections( Qt::Horizontal );
    setFrictionEnabled( true );
    setRowCount( 1 );
    setColumnCount( 1 );
    setClampingStyle( HbScrollArea::BounceBackClamping );
    setScrollingStyle( HbScrollArea::PanOrFlick );
    setLongPressEnabled( false );
    setItemRecycling( false ); // TODO: Enable recycling
    setUniformItemSizes( true );
    setItemPrototype( new RadioStationItem( *this ) );
    setSelectionMode( SingleSelection );

    RadioStationFilterModel* filterModel = mUiEngine.createNewFilterModel( this );
    filterModel->setCyclic( false );

    setModel( filterModel );
    mCurrentItem = static_cast<RadioStationItem*>( itemByIndex( model()->index( 0, 0 ) ) );

    updateFrequencies();

    connectAndTest( model(),        SIGNAL(rowsInserted(QModelIndex,int,int)),
                    this,           SLOT(insertFrequency(QModelIndex,int,int)) );
    connectAndTest( model(),        SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                    this,           SLOT(removeFrequency(QModelIndex,int,int)) );
//    connectAndTest( model(),        SIGNAL(layoutChanged()),
//                    this,           SLOT(updateFrequencies()) );
    connectAndTest( this,           SIGNAL(scrollingEnded()),
                    this,           SLOT(updateLoopedPos()) );

    RadioStationModel* stationModel = &mUiEngine.model();
    connectAndTest( stationModel,   SIGNAL(favoriteChanged(RadioStation)),
                    this,           SLOT(update(RadioStation)) );
    connectAndTest( stationModel,   SIGNAL(stationDataChanged(RadioStation)),
                    this,           SLOT(update(RadioStation)));
    connectAndTest( stationModel,   SIGNAL(radioTextReceived(RadioStation)),
                    this,           SLOT(updateRadioText(RadioStation)));
    connectAndTest( stationModel,   SIGNAL(dynamicPsChanged(RadioStation)),
                    this,           SLOT(update(RadioStation)));

    mRadioTextTimer->setSingleShot( true );
    mRadioTextTimer->setInterval( KRadioTextPlusCheckTimeout );
    connectAndTest( mRadioTextTimer,    SIGNAL(timeout()),
                    this,               SLOT(radioTextPlusCheckEnded()));

#ifdef USE_DEBUGGING_CONTROLS
    mRdsLabel->setPos( QPoint( 300, 10 ) );
    mRdsLabel->setText( "RDS" );
    mRdsLabel->setElideMode( Qt::ElideNone );
    HbFontSpec spec = mRdsLabel->fontSpec();
    spec.setTextPaneHeight( 10 );
    spec.setRole( HbFontSpec::Secondary );
    mRdsLabel->setFontSpec( spec );
    mRdsLabel->setTextColor( Qt::gray );
    connectAndTest( &mUiEngine,     SIGNAL(rdsAvailabilityChanged(bool)),
                    this,           SLOT(setRdsAvailable(bool)) );
#endif // USE_DEBUGGING_CONTROLS
}

/*!
 * Property
 *
 */
void RadioStationCarousel::setBackground( const HbIcon& background )
{
    mBackground = background;
}

/*!
 * Property
 *
 */
HbIcon RadioStationCarousel::background() const
{
    return mBackground;
}

/*!
 * Property
 *
 */
void RadioStationCarousel::setFavoriteIcon( const HbIcon& favoriteIcon )
{
    mFavoriteIcon = favoriteIcon;
}

/*!
 * Property
 *
 */
HbIcon RadioStationCarousel::favoriteIcon() const
{
    return mFavoriteIcon;
}

/*!
 * Property
 *
 */
void RadioStationCarousel::setNonFavoriteIcon( const HbIcon& nonFavoriteIcon )
{
    mNonFavoriteIcon = nonFavoriteIcon;
}

/*!
 * Property
 *
 */
HbIcon RadioStationCarousel::nonFavoriteIcon() const
{
    return mNonFavoriteIcon;
}

/*!
 *
 */
RadioUiEngine& RadioStationCarousel::uiEngine()
{
    return mUiEngine;
}

/*!
 *
 */
bool RadioStationCarousel::isAntennaAttached() const
{
    return mAntennaAttached;
}

/*!
 * Private slot
 */
void RadioStationCarousel::update( const RadioStation& station )
{
    RadioStationItem* item = currentStationItem();
    if ( item && item->frequency() == station.frequency() ) {
        item->update( &station );
    }
}

/*!
 * Private slot
 */
void RadioStationCarousel::updateRadioText( const RadioStation& station )
{
    if ( isAntennaAttached() ) {
        if ( station.radioText().isEmpty() ) {
            RadioStationItem* item = currentStationItem();
            if ( item ) {
                item->mRadiotextLabel->setText( "" );
            }
        } else {
            mRadioTextHolder = station.radioText();
            mRadioTextTimer->stop();
            mRadioTextTimer->start();
        }
    }
}

/*!
 * Private slot
 */
void RadioStationCarousel::leftGesture( int speedPixelsPerSecond )
{
    Q_UNUSED( speedPixelsPerSecond );
    QModelIndex index = currentIndex();

    if ( index == model()->index( model()->rowCount() - 1, 0 ) ) {
        index = model()->index( 0, 0 );
    } else {
        index = nextIndex( index );
    }

    scrollToIndex( index, mAutoScrollTime );
}

/*!
 * Private slot
 */
void RadioStationCarousel::rightGesture( int speedPixelsPerSecond )
{
    Q_UNUSED( speedPixelsPerSecond );
    QModelIndex index = currentIndex();

    if ( index == model()->index( 0, 0 ) ) {
        index = model()->index( model()->rowCount() - 1, 0 );
    } else {
        index = previousIndex( index );
    }

    scrollToIndex( index, mAutoScrollTime );
}

/*!
 * Private slot
 */
void RadioStationCarousel::insertFrequency( const QModelIndex& parent, int first, int last )
{
    Q_UNUSED( parent );
    QAbstractItemModel* freqModel = model();
    for ( int i = first; i <= last; ++i ) {
        QModelIndex index = freqModel->index( i, 0 );
        RadioStation station = freqModel->data( index, RadioStationModel::RadioStationRole ).value<RadioStation>();
        mModelIndexes.insert( station.frequency(), index );
        LOG_FORMAT( "Added frequency %u", station.frequency() );
        scrollToIndex( index, 0 );
    }
}

/*!
 * Private slot
 */
void RadioStationCarousel::removeFrequency( const QModelIndex& parent, int first, int last )
{
    Q_UNUSED( parent );
    QAbstractItemModel* freqModel = model();
    for ( int i = first; i <= last; ++i ) {
        QModelIndex index = freqModel->index( i, 0 );
        RadioStation station = freqModel->data( index, RadioStationModel::RadioStationRole ).value<RadioStation>();
        mModelIndexes.remove( station.frequency() );
    }
}

/*!
 * Private slot
 */
void RadioStationCarousel::updateFrequencies()
{
    mModelIndexes.clear();
    QAbstractItemModel* itemModel = model();
    const int count = itemModel->rowCount();
    for ( int i = 0; i < count; ++i ) {
        QModelIndex index = itemModel->index( i, 0 );
        uint frequency = itemModel->data( index, RadioStationModel::RadioStationRole ).value<RadioStation>().frequency();
        mModelIndexes.insert( frequency, index );
    }
}

/*!
 * Private slot
 */
void RadioStationCarousel::updateLoopedPos()
{
    const int row = currentIndex().row();
    if ( filterModel()->hasLooped( currentIndex() ) ) {
        QModelIndex realIndex = filterModel()->realIndex( currentIndex() );
        scrollTo( realIndex );
        setCurrentIndex( realIndex, QItemSelectionModel::SelectCurrent );
//        scrollToIndex( realIndex , 0 );
        LOG_FORMAT( "Index %d has looped. real index is %d", row, realIndex.row() );
    }
}

/*!
 * Private slot
 */
void RadioStationCarousel::radioTextPlusCheckEnded()
{
    RadioStationItem* item = currentStationItem();
    if ( item ) {
        item->mRadiotextLabel->setText( mRadioTextHolder );
    }
    mRadioTextHolder = "";
    mRadioTextTimer->stop();
}

#ifdef USE_DEBUGGING_CONTROLS
/*!
 * Public slot
 */
void RadioStationCarousel::setRdsAvailable( bool available )
{
    QColor color = Qt::green;
    if ( !available ) {
        LOG_FORMAT( "No RDS signal: Station has sent RDS earlier: %d", mUiEngine.model().currentStation().hasRds() );
        color = mUiEngine.model().currentStation().hasRds() ? Qt::yellow : Qt::gray;
        mRdsLabel->setText( "RDS" );
    } else {
        mRdsLabel->setText( "-RDS-" );
    }
    mRdsLabel->setTextColor( color );
}
#endif // USE_DEBUGGING_CONTROLS

/*!
 * Public slot
 *
 */
void RadioStationCarousel::setFrequency( uint frequency )
{
    RadioStationItem* item = currentStationItem();
    if ( item && item->mFrequency == frequency ) {
        return;
    }
/*
    QModelIndex index = static_cast<RadioStationFilterModel*>( model() )->modelIndexFromFrequency( frequency );
    if ( index.isValid() ) {
        scrollToIndex( index, 0 );
    } else {
        if ( item ) {
            item->setFrequency( frequency );
        }
    }
    */
/*


    QAbstractItemModel* itemModel = model();
    const int count = itemModel->rowCount();
    for ( int i = 0; i < count; ++i ) {
        QModelIndex index = itemModel->index( i, 0 );
        uint stationFrequency = itemModel->data( index, RadioStationModel::RadioStationRole ).value<RadioStation>().frequency();
        if ( frequency == stationFrequency ) {
            scrollToIndex( index, mAutoScrollTime );
            return;
        }
    }
 */

    if ( mModelIndexes.contains( frequency ) ) {
        QModelIndex index = mModelIndexes.value( frequency );
        scrollToIndex( index, 0 );
    } else {
        if ( item ) {
            item->setFrequency( frequency );
        }
    }
}

/*!
 * Public slot
 *
 */
void RadioStationCarousel::setSeekingText()
{
    mRadioTextTimer->stop();
    RadioStationItem* item = currentStationItem();
    if ( item ) {
        item->setSeekingText();
    }

#ifdef USE_DEBUGGING_CONTROLS
    mRdsLabel->setTextColor( Qt::gray );
#endif //USE_DEBUGGING_CONTROLS
}

/*!
 * Public slot
 */
void RadioStationCarousel::updateHeadsetStatus( bool connected )
{
    mRadioTextTimer->stop();
    mAntennaAttached = connected;
    RadioStationItem* item = currentStationItem();
    if ( item  ) {
        item->update();
    }
}

/*!
 * \reimp
 *
 */
void RadioStationCarousel::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    mBackground.paint( painter, QRectF( QPoint( 0, 0 ), size() ), Qt::IgnoreAspectRatio );
    HbGridView::paint( painter, option, widget );
}

/*!
 * \reimp
 */
void RadioStationCarousel::mouseMoveEvent( QGraphicsSceneMouseEvent* event )
{
    HbGridView::mouseMoveEvent( event );
}

/*!
 * \reimp
 */
void RadioStationCarousel::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
    QPointF pos = QPointF( size().width() / 2, size().height() / 2 );
    HbAbstractViewItem* item = itemAtPosition( pos );
    if ( item ) {
        scrollToIndex( item->modelIndex(), mAutoScrollTime );
    }

    HbGridView::mouseReleaseEvent( event );
}

/*!
 * \reimp
 */
void RadioStationCarousel::resizeEvent( QGraphicsSceneResizeEvent* event )
{
    HbGridView::resizeEvent( event );
    QModelIndex index = filterModel()->modelIndexFromFrequency( mUiEngine.currentFrequency() );
    setCurrentIndex( index, QItemSelectionModel::SelectCurrent );
    scrollTo( index );
}

/*!
 *
 */
RadioStationItem* RadioStationCarousel::currentStationItem()
{
    return static_cast<RadioStationItem*>( currentViewItem() );
//    return mCurrentItem;
}

/*!
 *
 */
RadioStationFilterModel* RadioStationCarousel::filterModel() const
{
    return static_cast<RadioStationFilterModel*>( model() );
}

/*!
 *
 */
void RadioStationCarousel::scrollToIndex( const QModelIndex& index, int time )
{
    RadioStationItem* item = static_cast<RadioStationItem*>( itemByIndex( index ) );
    if ( index.isValid() && item ) {
        int posX = item->pos().x();
        const int currentRow = currentIndex().row();
        const int nextRow = index.row();
        if ( currentRow != nextRow ) {
            LOG_FORMAT( "Current row is %d, scrolling to row %d", currentRow, nextRow);
        }
        if ( !filterModel()->isEqual( currentIndex(), index ) ) {
            setCurrentIndex( index, QItemSelectionModel::SelectCurrent );
            mCurrentItem = static_cast<RadioStationItem*>( item );
            uint frequency = model()->data( index, RadioStationModel::RadioStationRole ).value<RadioStation>().frequency();
            if ( item->frequency() != frequency ) {
                item->setFrequency( frequency );
            }
            emit frequencyChanged( frequency, CommandSender::StationCarousel );
        }
        scrollContentsTo( QPointF( posX, 0 ) , time );
    }
}
