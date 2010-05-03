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
#include <QTimeLine>

// User includes
#include "radiostationcarousel.h"
#include "radiostationitem.h"
#include "radiouiengine.h"
#include "radiostationmodel.h"
#include "radiofadinglabel.h"
#include "radiologger.h"
#include "radiostationmodel.h"
#include "radiouiutilities.h"
#include "radio_global.h"

#ifdef USE_LAYOUT_FROM_E_DRIVE
    const QString KFavoriteIconPath = "e:/radiotest/images/favoriteiconactive.png";
    const QString KNonFavoriteIconPath = "e:/radiotest/images/favoriteiconinactive.png";
#else
    const QString KFavoriteIconPath = ":/images/favoriteiconactive.png";
    const QString KNonFavoriteIconPath = ":/images/favoriteiconinactive.png";
#endif

const int KRadioTextPlusCheckTimeout = 700; // 700 ms
const int KFreqScrollDivider = 100000;

// ===============================================================
//  Scanning helper
// ===============================================================

/*!
 *
 */
ScanningHelper::ScanningHelper( RadioStationCarousel& carousel ) :
    mCarousel( carousel ),
    mCurrentFrequency( 0 ),
    mPreviousFrequency( 0 ),
    mStationItem( 0 ),
    mNumberScrollingTimeLine( new QTimeLine( 1000, this ) )
{
    mNumberScrollingTimeLine->setCurveShape( QTimeLine::EaseInCurve );
    connectAndTest( mNumberScrollingTimeLine,  SIGNAL(finished()),
                    &mCarousel,                SIGNAL(scanAnimationFinished()) );
    connectAndTest( mNumberScrollingTimeLine,  SIGNAL(frameChanged(int)),
                    this,                      SLOT(numberScrollUpdate(int)) );
}

/*!
 *
 */
void ScanningHelper::start()
{
    QTimer::singleShot( 0, this, SLOT(startSlide()) );
}

/*!
 * Private slot
 */
void ScanningHelper::startSlide()
{
    mCarousel.scrollToIndex( mModelIndex, 1000 );
    startNumberScroll();
}

/*!
 * Private slot
 */
void ScanningHelper::startNumberScroll()
{
    //TODO: Take italy case into account
    if ( mPreviousFrequency ) {
        mNumberScrollingTimeLine->setFrameRange( mPreviousFrequency / KFreqScrollDivider, mCurrentFrequency / KFreqScrollDivider );
        mNumberScrollingTimeLine->start();
    } else {
        emit mCarousel.scanAnimationFinished();
    }
}

/*!
 * Private slot
 */
void ScanningHelper::numberScrollUpdate( int value )
{
    if ( mStationItem ) {
        mStationItem->setFrequency( value * KFreqScrollDivider );
    }
}

// ===============================================================
//  Carousel
// ===============================================================

/*!
 *
 */
RadioStationCarousel::RadioStationCarousel( RadioUiEngine* uiEngine ) :
    HbGridView( 0 ),
    mUiEngine( uiEngine ),
    mAntennaAttached( false ),
    mAutoScrollTime( 300 ),
    mPreviousButtonPos( 0.0 ),
    mMovingLeft( false ),
    mCurrentItem( 0 ),
    mRadioTextTimer( new QTimer( this ) ),
    mScrollPos( 0 ),
    mScanningHelper( 0 )
#ifdef USE_DEBUGGING_CONTROLS
    ,mRdsLabel( new RadioFadingLabel( this ) )
#endif // USE_DEBUGGING_CONTROLS
{
    RadioUiUtilities::setCarousel( this );

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
    if ( mUiEngine ) {
        connectAndTest( mUiEngine,      SIGNAL(rdsAvailabilityChanged(bool)),
                        this,           SLOT(setRdsAvailable(bool)) );
    }
#endif // USE_DEBUGGING_CONTROLS
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
void RadioStationCarousel::setAutoScrollTime( const int time )
{
    mAutoScrollTime = time;
}

/*!
 *
 */
int RadioStationCarousel::autoScrollTime() const
{
    return mAutoScrollTime;
}

/*!
 *
 */
void RadioStationCarousel::init( RadioUiEngine* uiEngine )
{
    mUiEngine = uiEngine;
    mAntennaAttached = mUiEngine->isAntennaAttached();

    setStationModel( &mUiEngine->model() );

    mCurrentItem = static_cast<RadioStationItem*>( itemByIndex( model()->index( 0, 0 ) ) );
}

/*!
 *
 */
void RadioStationCarousel::setStationModel( RadioStationModel* stationModel )
{
    if ( stationModel ) {
        connectAndTest( stationModel,   SIGNAL(rowsInserted(QModelIndex,int,int)),
                        this,           SLOT(insertFrequency(QModelIndex,int,int)) );
        connectAndTest( stationModel,   SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                        this,           SLOT(removeFrequency(QModelIndex,int,int)) );
        connectAndTest( stationModel,   SIGNAL(favoriteChanged(RadioStation)),
                        this,           SLOT(update(RadioStation)) );
        connectAndTest( stationModel,   SIGNAL(stationDataChanged(RadioStation)),
                        this,           SLOT(update(RadioStation)));
        connectAndTest( stationModel,   SIGNAL(radioTextReceived(RadioStation)),
                        this,           SLOT(updateRadioText(RadioStation)));
        connectAndTest( stationModel,   SIGNAL(dynamicPsChanged(RadioStation)),
                        this,           SLOT(update(RadioStation)));
    } else {
        QAbstractItemModel* currentModel = model();
//        disconnect( currentModel,   SIGNAL(rowsInserted(QModelIndex,int,int)),
//                    this,           SLOT(insertFrequency(QModelIndex,int,int)) );
//        disconnect( currentModel,   SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
//                    this,           SLOT(removeFrequency(QModelIndex,int,int)) );
        disconnect( currentModel,   SIGNAL(favoriteChanged(RadioStation)),
                    this,           SLOT(update(RadioStation)) );
        disconnect( currentModel,   SIGNAL(stationDataChanged(RadioStation)),
                    this,           SLOT(update(RadioStation)));
        disconnect( currentModel,   SIGNAL(radioTextReceived(RadioStation)),
                    this,           SLOT(updateRadioText(RadioStation)));
        disconnect( currentModel,   SIGNAL(dynamicPsChanged(RadioStation)),
                    this,           SLOT(update(RadioStation)));
    }
    setModel( stationModel );
    updateFrequencies();
}

/*!
 *
 */
RadioUiEngine* RadioStationCarousel::uiEngine()
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
 *
 */
void RadioStationCarousel::setScanningMode( bool scanning )
{
    RadioStationItem* item = currentStationItem();

    if ( scanning ) {
        cleanRdsData();
        if ( !mScanningHelper ) {
            mScanningHelper = new ScanningHelper( *this );
        }
    } else {
        delete mScanningHelper;
        mScanningHelper = 0;
        if ( item ) {
            item->update();
        }
    }
    setEnabled( !scanning );
}

/*!
 *
 */
bool RadioStationCarousel::isInScanningMode() const
{
    return mScanningHelper != 0;
}

/*!
 *
 */
void RadioStationCarousel::cleanRdsData()
{
    RadioStationItem* item = currentStationItem();
    if ( item ) {
        item->cleanRdsData();
    }
}

/*!
 *
 */
void RadioStationCarousel::animateNewStation( const RadioStation& station )
{
    if ( mScanningHelper ) {
        RadioStationModel* model = stationModel();
        const QModelIndex index = model->modelIndexFromFrequency( station.frequency() );
        mScanningHelper->mModelIndex = index;
        mScanningHelper->mCurrentFrequency = station.frequency();
        mScanningHelper->mStationItem = static_cast<RadioStationItem*>( itemByIndex( index ) );

        uint prevFrequency = 0;
        if ( model->rowCount() > 1 ) {
            QModelIndex prevIndex = model->index( index.row() - 1, 0 );
            RadioStation prevStation = model->data( prevIndex, RadioStationModel::RadioStationRole ).value<RadioStation>();
            prevFrequency = prevStation.frequency();
        } else if ( mUiEngine ) {
            prevFrequency = mUiEngine->minFrequency();
        }
        mScanningHelper->mPreviousFrequency = prevFrequency;
        mScanningHelper->mStationItem->setFrequency( prevFrequency );
        mScanningHelper->mStationItem->cleanRdsData();

        mScanningHelper->start();
    }
}

/*!
 *
 */
void RadioStationCarousel::setItemVisibility( bool visible )
{
    RadioStationItem* item = currentStationItem();
        if ( item ) {
            item->setVisible( visible );
        }
}

/*!
 * Private slot
 */
void RadioStationCarousel::update( const RadioStation& station )
{
    RadioStationItem* item = currentStationItem();
    if ( item && item->frequency() == station.frequency() && !isInScanningMode() ) {
        item->update( &station );
    }
}

/*!
 * Private slot
 */
void RadioStationCarousel::updateRadioText( const RadioStation& station )
{
    if ( isAntennaAttached() && !isInScanningMode() ) {
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

//    if ( index == model()->index( model()->rowCount() - 1, 0 ) ) {
//        index = model()->index( 0, 0 );
//    } else {
//        index = nextIndex( index );
//    }

    index = nextIndex( index );
    if ( index.isValid() ) {
        scrollToIndex( index, mAutoScrollTime );
    }
}

/*!
 * Private slot
 */
void RadioStationCarousel::rightGesture( int speedPixelsPerSecond )
{
    Q_UNUSED( speedPixelsPerSecond );
    QModelIndex index = currentIndex();

//    if ( index == model()->index( 0, 0 ) ) {
//        index = model()->index( model()->rowCount() - 1, 0 );
//    } else {
//        index = previousIndex( index );
//    }

    index = previousIndex( index );
    if ( index.isValid() ) {
        scrollToIndex( index, mAutoScrollTime );
    }
}

/*!
 * Private slot
 */
void RadioStationCarousel::insertFrequency( const QModelIndex& parent, int first, int last )
{
    Q_UNUSED( parent );
    QAbstractItemModel* freqModel = model();

    for ( int i = first; freqModel && i <= last; ++i ) {
        QModelIndex index = freqModel->index( i, 0 );
        RadioStation station = freqModel->data( index, RadioStationModel::RadioStationRole ).value<RadioStation>();
        mModelIndexes.insert( station.frequency(), index );
        LOG_FORMAT( "Added frequency %u", station.frequency() );
        if ( !isInScanningMode() ) {
            scrollToIndex( index, mAutoScrollTime );
        }
    }
}

/*!
 * Private slot
 */
void RadioStationCarousel::removeFrequency( const QModelIndex& parent, int first, int last )
{
    Q_UNUSED( parent );
    QAbstractItemModel* freqModel = model();
    for ( int i = first; freqModel && i <= last; ++i ) {
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
    if ( itemModel ) {
        const int count = itemModel->rowCount();
        for ( int i = 0; i < count; ++i ) {
            QModelIndex index = itemModel->index( i, 0 );
            uint frequency = itemModel->data( index, RadioStationModel::RadioStationRole ).value<RadioStation>().frequency();
            mModelIndexes.insert( frequency, index );
        }
    }
}

/*!
 * Private slot
 */
void RadioStationCarousel::updateLoopedPos()
{
//    const int row = currentIndex().row();
//    if ( filterModel()->hasLooped( currentIndex() ) ) {
//        QModelIndex realIndex = filterModel()->realIndex( currentIndex() );
//        scrollTo( realIndex );
//        setCurrentIndex( realIndex, QItemSelectionModel::SelectCurrent );
////        scrollToIndex( realIndex , 0 );
//        LOG_FORMAT( "Index %d has looped. real index is %d", row, realIndex.row() );
//    }
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

/*!
 * Private slot
 */
void RadioStationCarousel::delayedScroll()
{
    scrollContentsTo( QPointF( mScrollPos, 0 ) , 1000 );
}

#ifdef USE_DEBUGGING_CONTROLS
/*!
 * Public slot
 */
void RadioStationCarousel::setRdsAvailable( bool available )
{
    QColor color = Qt::green;
    if ( !available && mUiEngine ) {
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
        scrollToIndex( index, mAutoScrollTime );
    } else {
        if ( item ) {
            item->setFrequency( frequency );
        }
    }
}

/*!
 * Public slot
 */
void RadioStationCarousel::updateAntennaStatus( bool connected )
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
//    QPointF pos = QPointF( size().width() / 2, size().height() / 2 );
//    HbAbstractViewItem* item = itemAtPosition( pos );
//    if ( item ) {
//        scrollToIndex( item->modelIndex(), mAutoScrollTime );
//    }

    HbGridView::mouseReleaseEvent( event );
}

/*!
 * \reimp
 */
void RadioStationCarousel::resizeEvent( QGraphicsSceneResizeEvent* event )
{
    HbGridView::resizeEvent( event );
//    QModelIndex index = filterModel()->modelIndexFromFrequency( mUiEngine.currentFrequency() );
//    setCurrentIndex( index, QItemSelectionModel::SelectCurrent );
//    scrollTo( index );
}

/*!
 *
 */
RadioStationItem* RadioStationCarousel::currentStationItem()
{
    return static_cast<RadioStationItem*>( currentViewItem() );
}

/*!
 *
 */
RadioStationModel* RadioStationCarousel::stationModel() const
{
    return static_cast<RadioStationModel*>( model() );
}

/*!
 *
 */
void RadioStationCarousel::scrollToIndex( const QModelIndex& index, int time )
{
    RadioStationItem* item = static_cast<RadioStationItem*>( itemByIndex( index ) );
    if ( index.isValid() && item ) {
//        int posX = item->pos().x();
        int width = this->size().width();
        int posX = index.row() * width;
        const int currentRow = currentIndex().row();
        const int nextRow = index.row();
        if ( currentRow != nextRow ) {
            LOG_FORMAT( "Current row is %d, scrolling to row %d", currentRow, nextRow);
        }

        setCurrentIndex( index, QItemSelectionModel::SelectCurrent );
        mCurrentItem = static_cast<RadioStationItem*>( item );
        uint frequency = model()->data( index, RadioStationModel::RadioStationRole ).value<RadioStation>().frequency();
        emit frequencyChanged( frequency, TuneReason::StationCarousel );

        mScrollPos = posX;
        scrollContentsTo( QPointF( posX, 0 ) , time );
    }
}
