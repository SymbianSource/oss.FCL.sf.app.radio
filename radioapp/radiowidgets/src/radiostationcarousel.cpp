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
#include <HbPanGesture>

// User includes
#include "radiostationcarousel.h"
#include "radiouiloader.h"
#include "radiostationitem.h"
#include "radiostation.h"
#include "radiouiengine.h"
#include "radiostationmodel.h"
#include "radiofadinglabel.h"
#include "radiologger.h"
#include "radiocarouselmodel.h"
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
const int INFOTEXT_NOFAVORITES_TIMEOUT = 15000;

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
    mCarousel.scrollToIndex( mModelIndex, RadioStationCarousel::NoSignal );
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
    mGenericTimer( new QTimer( this ) ),
    mTimerMode( NoTimer ),
    mScanningHelper( 0 ),
    mInfoText( 0 ),
    mCurrentItem( 0 ),
    mPanStartPos( 0 )
#ifdef USE_DEBUGGING_CONTROLS
    ,mRdsLabel( new RadioFadingLabel( this ) )
#endif // USE_DEBUGGING_CONTROLS
{
    RadioUiUtilities::setCarousel( this );
    setClampingStyle( HbScrollArea::StrictClamping );
    setScrollingStyle( HbScrollArea::Pan );
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
void RadioStationCarousel::init( RadioUiLoader& uiLoader, RadioUiEngine* uiEngine )
{
    mUiEngine = uiEngine;
    mAntennaAttached = mUiEngine->isAntennaAttached();

    mInfoText = uiLoader.findWidget<HbLabel>( DOCML::MV_NAME_INFO_TEXT );
    mInfoText->setTextWrapping( Hb::TextWordWrap );

    setRowCount( 1 );
    setColumnCount( 1 );
    setScrollDirections( Qt::Horizontal );
    setFrictionEnabled( true );
    setLongPressEnabled( false );
    setItemRecycling( false );
    setUniformItemSizes( true );
    setItemPrototype( new RadioStationItem( *this ) );
    setSelectionMode( NoSelection );

//    grabGesture( Qt::PanGesture );

    RadioCarouselModel* carouselModel = mUiEngine->carouselModel();
    setCarouselModel( carouselModel );

    mCurrentItem = static_cast<RadioStationItem*>( itemByIndex( carouselModel->index( 0, 0 ) ) );

    RadioStationModel* stationModel = &mUiEngine->stationModel();
    connectAndTest( stationModel,   SIGNAL(favoriteChanged(RadioStation)),
                    this,           SLOT(update(RadioStation)) );
    connectAndTest( stationModel,   SIGNAL(stationDataChanged(RadioStation)),
                    this,           SLOT(update(RadioStation)));
    connectAndTest( stationModel,   SIGNAL(radioTextReceived(RadioStation)),
                    this,           SLOT(updateRadioText(RadioStation)));
    connectAndTest( stationModel,   SIGNAL(dynamicPsChanged(RadioStation)),
                    this,           SLOT(update(RadioStation)));

    updateClampingStyle();

    connectAndTest( this,           SIGNAL(longPressed(HbAbstractViewItem*,QPointF)),
                    this,           SLOT(openContextMenu(HbAbstractViewItem*,QPointF)) );
    setLongPressEnabled( true );

    mGenericTimer->setSingleShot( true );
    connectAndTest( mGenericTimer,  SIGNAL(timeout()),
                    this,           SLOT(timerFired()));

    initToLastTunedFrequency();

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
 *
 */
void RadioStationCarousel::setCarouselModel( RadioCarouselModel* carouselModel )
{
    if ( carouselModel ) {
        connectAndTest( carouselModel,  SIGNAL(rowsInserted(QModelIndex,int,int)),
                        this,           SLOT(insertFrequency(QModelIndex,int,int)) );
        connectAndTest( carouselModel,  SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                        this,           SLOT(prepareToRemoveFrequency(QModelIndex,int,int)) );
        connectAndTest( carouselModel,  SIGNAL(rowsRemoved(QModelIndex,int,int)),
                        this,           SLOT(removeFrequency(QModelIndex,int,int)) );
    } else {
        QAbstractItemModel* currentModel = model();
        disconnect( currentModel,   SIGNAL(rowsInserted(QModelIndex,int,int)),
                    this,           SLOT(insertFrequency(QModelIndex,int,int)) );
        disconnect( currentModel,   SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                    this,           SLOT(prepareToRemoveFrequency(QModelIndex,int,int)) );
        disconnect( currentModel,   SIGNAL(rowsRemoved(QModelIndex,int,int)),
                    this,           SLOT(removeFrequency(QModelIndex,int,int)) );
    }
    setModel( carouselModel );
    updateFrequencies();
    initCurrentStationItem();
}

/*!
 *
 */
void RadioStationCarousel::setFrequency( uint frequency, int reason )
{
    RadioStationItem* item = currentStationItem();
//    if ( item && item->mFrequency == frequency ) {
//        return;
//    }

    if ( mModelIndexes.contains( frequency ) ) {
        QModelIndex index = mModelIndexes.value( frequency );

        if ( reason == TuneReason::FrequencyStrip || reason == TuneReason::StationsList ) {
            scrollToIndex( index, RadioStationCarousel::NoAnim | RadioStationCarousel::NoSignal );
        } else if ( reason == TuneReason::Skip || reason == TuneReason::StationScan ) {
            scrollToIndex( index, RadioStationCarousel::NoSignal );
        } else {
            scrollToIndex( index );
        }
    } else {
        if ( item ) {
            item->setFrequency( frequency );
        }
    }
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
    initCurrentStationItem();

    if ( scanning ) {

        setInfoText( CarouselInfoText::Scanning );
        if ( !mScanningHelper ) {
            mScanningHelper = new ScanningHelper( *this );
        }
    } else {
        delete mScanningHelper;
        mScanningHelper = 0;
        clearInfoText();
    }
    setEnabled( !scanning );
}

/*!
 *
 */
bool RadioStationCarousel::isInScanningMode() const
{
    return RadioUiUtilities::isScannerAlive();
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
void RadioStationCarousel::updateCurrentItem()
{
    RadioStationItem* item = currentStationItem();
    if ( item ) {
        item->update();
    }
}

/*!
 *
 */
void RadioStationCarousel::animateNewStation( const RadioStation& station )
{
    if ( mScanningHelper ) {
        RadioCarouselModel* model = carouselModel();
        const QModelIndex index = model->modelIndexFromFrequency( station.frequency() );
        mScanningHelper->mModelIndex = index;
        mScanningHelper->mCurrentFrequency = station.frequency();
        mScanningHelper->mStationItem = static_cast<RadioStationItem*>( itemByIndex( index ) );

        uint prevFrequency = 0;
        if ( model->rowCount() > 1 ) {
            const int prevIndex = index.row() - 1;
            RadioStation prevStation = model->data( model->index( prevIndex, 0 ), RadioStationModel::RadioStationRole ).value<RadioStation>();
            prevFrequency = prevStation.frequency();
        } else if ( mUiEngine ) {
            prevFrequency = mUiEngine->minFrequency();
        }

        mScanningHelper->mPreviousFrequency = prevFrequency;
        if ( mScanningHelper->mStationItem ) {
            mScanningHelper->mStationItem->setFrequency( prevFrequency );
            mScanningHelper->mStationItem->cleanRdsData();
        }

        mScanningHelper->start();
    }
}

/*!
 *
 */
void RadioStationCarousel::setItemVisible( bool visible )
{
    RadioStationItem* item = currentStationItem();
    if ( item ) {
        item->setVisible( visible );
    }
}

/*!
 *
 */
void RadioStationCarousel::setInfoText( CarouselInfoText::Type type )
{
    mInfoTextType = type;
    if ( type == CarouselInfoText::NoFavorites ) {
        mInfoText->setPlainText( hbTrId( "txt_rad_dialog_long_press_arrow_keys_to_search_str" ) );
        mInfoText->setAlignment( Qt::AlignCenter );
        setItemVisible( false );
        mTimerMode = InfoText;
        mGenericTimer->setInterval( INFOTEXT_NOFAVORITES_TIMEOUT );
        mGenericTimer->start();
    } else if ( type == CarouselInfoText::ConnectAntenna ) {
        cleanRdsData();
        mInfoText->setPlainText( hbTrId( "txt_rad_info_connect_wired_headset1" ) );
        mInfoText->setAlignment( Qt::AlignBottom | Qt::AlignHCenter );
    } else if ( type == CarouselInfoText::Seeking ) {
        cleanRdsData();
        mInfoText->setAlignment( Qt::AlignBottom | Qt::AlignHCenter );
        mInfoText->setPlainText( hbTrId( "txt_rad_list_seeking" ) );
    } else if ( type == CarouselInfoText::Scanning ) {
        cleanRdsData();
        mInfoText->setAlignment( Qt::AlignBottom | Qt::AlignHCenter );
        mInfoText->setPlainText( hbTrId( "txt_rad_list_searching_all_available_stations_ple" ) );
    }

    mInfoText->setVisible( true );
}

/*!
 *
 */
void RadioStationCarousel::clearInfoText()
{
    if ( mInfoTextType != CarouselInfoText::None ) {
        mGenericTimer->stop();
        mInfoTextType = CarouselInfoText::None;
        mInfoText->setVisible( false );
        mInfoText->clear();
        setItemVisible( true );
        updateCurrentItem();
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
            mTimerMode = RtPlusCheck;
            mGenericTimer->stop();
            mGenericTimer->setInterval( KRadioTextPlusCheckTimeout );
            mGenericTimer->start();
        }
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
            scrollToIndex( index, RadioStationCarousel::NoAnim | RadioStationCarousel::NoSignal );
        }
    }

    initCurrentStationItem();

    updateClampingStyle();
}

/*!
 * Private slot
 */
void RadioStationCarousel::prepareToRemoveFrequency( const QModelIndex& parent, int first, int last )
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
void RadioStationCarousel::removeFrequency( const QModelIndex& parent, int first, int last )
{
    Q_UNUSED( parent );
    Q_UNUSED( first );
    Q_UNUSED( last );

    initCurrentStationItem();
    updateClampingStyle();
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
void RadioStationCarousel::timerFired()
{
    if ( mTimerMode == RtPlusCheck ) {
        RadioStationItem* item = currentStationItem();
        if ( item ) {
            item->mRadiotextLabel->setText( mRadioTextHolder );
        }
        mRadioTextHolder = "";
    } else if ( mTimerMode == InfoText ) {
        clearInfoText();
    }

    mTimerMode = NoTimer;
}

/*!
 * Private slot
 */
void RadioStationCarousel::openContextMenu( HbAbstractViewItem* item, const QPointF& coords )
{
    if ( item ) {
        static_cast<RadioStationItem*>( item )->handleLongPress( coords );
    }
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
 */
void RadioStationCarousel::updateAntennaStatus( bool connected )
{
    mAntennaAttached = connected;
    mGenericTimer->stop();

    if ( !connected ) {
        setInfoText( CarouselInfoText::ConnectAntenna );
    } else {
        clearInfoText();
    }
}

/*!
 * \reimp
 */
void RadioStationCarousel::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    if ( mInfoTextType == CarouselInfoText::NoFavorites ) {
        clearInfoText();
    }

    HbGridView::mousePressEvent( event );
}

/*!
 * \reimp
 */
void RadioStationCarousel::gestureEvent( QGestureEvent* event )
{
    HbGridView::gestureEvent( event );

    if ( HbPanGesture* gesture = qobject_cast<HbPanGesture*>( event->gesture( Qt::PanGesture ) ) ) {
        if ( gesture->state() == Qt::GestureFinished ) {
            updatePos( (int)gesture->offset().x() );
        }
    }
}

/*!
 *
 */
void RadioStationCarousel::initToLastTunedFrequency()
{
    const uint currentFrequency = mUiEngine->currentFrequency();
    const QModelIndex currentIndex = carouselModel()->modelIndexFromFrequency( currentFrequency );

    if ( currentIndex.isValid() ) {//&& itemByIndex( currentIndex ) ) {
        scrollToIndex( currentIndex, RadioStationCarousel::NoSignal | RadioStationCarousel::NoAnim );
    } else {
        RadioStationItem* item = static_cast<RadioStationItem*>( itemAt( 0, 0 ) );
        if ( item ) {
            item->setFrequency( currentFrequency );
        }
    }
}

/*!
 *
 */
void RadioStationCarousel::updateClampingStyle()
{
    if ( model()->rowCount() > 1 ) {
        setClampingStyle( HbScrollArea::StrictClamping );
    } else {
        setClampingStyle( HbScrollArea::BounceBackClamping );
        update( mUiEngine->stationModel().currentStation() );
    }
}

/*!
 *
 */
void RadioStationCarousel::initCurrentStationItem()
{
    mCurrentItem = static_cast<RadioStationItem*>( visibleItems().first() );
}

/*!
 *
 */
RadioStationItem* RadioStationCarousel::currentStationItem()
{
    return mCurrentItem;
}

/*!
 *
 */
RadioCarouselModel* RadioStationCarousel::carouselModel() const
{
    return static_cast<RadioCarouselModel*>( model() );
}

/*!
 *
 */
void RadioStationCarousel::scrollToIndex( const QModelIndex& index, RadioStationCarousel::ScrollMode mode )
{
    RadioStationItem* item = static_cast<RadioStationItem*>( itemByIndex( index ) );
    if ( index.isValid() && item ) {
        const int posX = index.row() * (int)size().width();
        setCurrentIndex( index, QItemSelectionModel::ClearAndSelect );

        if ( mode.testFlag( UpdateItem ) ) {
            item->update();
        }

        int scrollTime = mAutoScrollTime;
        if ( mode.testFlag( NoAnim ) ) {
            scrollTime = 0;
        }
        scrollContentsTo( QPointF( posX, 0 ), scrollTime );
        mCurrentItem = static_cast<RadioStationItem*>( item );
        if ( !mode.testFlag( NoSignal ) ) {
            uint frequency = model()->data( index, RadioStationModel::RadioStationRole ).value<RadioStation>().frequency();
            emit frequencyChanged( frequency, TuneReason::StationCarousel );
        }
    }
}

/*!
 *
 */
void RadioStationCarousel::updatePos( int offset )
{
//    QModelIndex index = currentIndex();
//
//    ScrollMode mode = 0;
//    const qreal threshold = size().width() / 3;
//    if ( abs( offset ) >= threshold ) {
//        if ( offset > 0 ) {
//            index = previousIndex( index );
//        } else {
//            index = nextIndex( index );
//        }
//    } else {
//        mode |= RadioStationCarousel::NoSignal;
//    }
//
//    scrollToIndex( index, mode );
}

/*!
 *
 */
void RadioStationCarousel::skip( StationSkip::Mode mode )
{
    RadioStationItem* item = currentStationItem();
    if ( item ) {
        RadioCarouselModel* model = carouselModel();
        const uint frequency = model->findClosest( item->frequency(), mode ).frequency();
        const QModelIndex& index = model->modelIndexFromFrequency( frequency );
        scrollToIndex( index, RadioStationCarousel::NoSignal );
    }
}
