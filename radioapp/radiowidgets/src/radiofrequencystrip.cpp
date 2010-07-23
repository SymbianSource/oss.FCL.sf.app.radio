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
#include <QPainter>
#include <QStringListModel>
#include <QGraphicsSceneResizeEvent>
#include <QPen>
#include <HbPushButton>
#include <QTimer>
#include <HbColorScheme>
#include <HbEvent>
#include <HbSwipeGesture>
#include <HbDeviceProfile>

#include "radiofrequencystrip.h"
#include "radiofrequencyitem.h"
#include "radiouiengine.h"
#include "radiostation.h"
#include "radiostationmodel.h"
#include "radioutil.h"
#include "radiouiloader.h"
#include "radiologger.h"

// Frequency lines
const int TAB_HEIGHT_SMALL = 12;
const int TAB_HEIGHT_BIG = 18;
const int PEN_WIDTH_FAVORITE = 4;
const qreal INDICATOR_WIDTH = 2.0;

const qreal ROUNDER = 0.5;
const int SELECTOR_WIDTH = 2;
const int SELECTOR_Z_POS = 100;

const int HALF_HERTZ = ONE_HERTZ / 2;
const int ONE_TAB_DISTANCE = 20;
const uint ONE_TAB_IN_HZ = uint( 0.2 * ONE_HERTZ );
const qreal PIXEL_IN_HZ = ONE_TAB_IN_HZ / ONE_TAB_DISTANCE;
const qreal PIXMAP_OVERLAP = qreal( ONE_TAB_DISTANCE ) / 2;
const int ITEM_WIDTH = ONE_TAB_DISTANCE * 5;
const int PIXMAP_WIDTH = ITEM_WIDTH + (int)PIXMAP_OVERLAP;

const uint ONE_HUNDRED_KHZ = 100000;

const char* DECIMAL_TEXT = ".0";

#ifdef BUILD_WIN32
    const int FREQUENCY_FONT_SIZE = 12;
    const int DECIMAL_FONT_SIZE = 8;
#else
    const int FREQUENCY_FONT_SIZE = 6;
    const int DECIMAL_FONT_SIZE = 4;
#endif // BUILD_WIN32

const QFont FREQUENCY_FONT( "", FREQUENCY_FONT_SIZE );
const QFont DECIMAL_FONT( "", DECIMAL_FONT_SIZE );

const char* SLIDE_TO_LEFT       = "SlideToLeft";
const char* SLIDE_FROM_LEFT     = "SlideFromLeft";
const char* SLIDE_TO_RIGHT      = "SlideToRight";
const char* SLIDE_FROM_RIGHT    = "SlideFromRight";
const char* LEFT_BUTTON         = "tv:left_button";
const char* RIGHT_BUTTON        = "tv:right_button";

const char* TEXT_COLOR_ATTRIBUTE = "text";
const int MANUALSEEK_START_TIMEOUT = 400;
const int MANUALSEEK_END_TIMEOUT = 2000;
const int MANUALSEEK_SIGNAL_DELAY = 300;

/*!
 *
 */
RadioFrequencyStrip::RadioFrequencyStrip() :
    RadioStripBase(),
    mUiEngine( NULL ),
    mItemHeight( 8 ),
    mMinFrequency( 87500000 ),
    mMaxFrequency( 108000000 ),
    mFrequencyStepSize( 100000 ),
    mFrequency( 87500000 ),
    mSelectorImage( new QGraphicsPixmapItem( this ) ),
    mSeparatorPos( 0.0 ),
    mMaxWidth( 0 ),
    mSelectorPos( 0.0 ),
    mLeftButton( 0 ),
    mRightButton( 0 ),
    mManualSeekTimer( NULL ),
    mManualSeekMode( false ),
    mLastReportedFrequency( 0 ),
    mManualSeekTimerId( 0 ),
    mForegroundColor( Qt::white )//HbColorScheme::color( TEXT_COLOR_ATTRIBUTE ) )
{
    RadioUtil::setFrequencyStrip( this );

    setScrollingStyle( HbScrollArea::PanWithFollowOn );
    setItemSize( QSizeF( ITEM_WIDTH + PIXMAP_OVERLAP / 2, mItemHeight ) );
    setOverlap( PIXMAP_OVERLAP / 2 );
}

/*!
 * Property
 */
void RadioFrequencyStrip::setItemHeight( int itemHeight )
{
    mItemHeight = itemHeight;
}

/*!
 * Property
 */
int RadioFrequencyStrip::itemHeight() const
{
    return mItemHeight;
}

/*!
 *
 */
void RadioFrequencyStrip::init( RadioUiEngine* engine, RadioUiLoader& uiLoader )
{
    mUiEngine = engine;
    mMinFrequency       = mUiEngine->minFrequency();
    mMaxFrequency       = mUiEngine->maxFrequency();
    mFrequencyStepSize  = mUiEngine->frequencyStepSize();
    mFrequency          = RadioUiEngine::lastTunedFrequency();

    initModel();
    initEmptyItems();
    initPositions();

    HbDeviceProfile deviceProfile;
    mItemHeight = static_cast<int>( mItemHeight * deviceProfile.unitValue() );

    mLeftButton = uiLoader.findWidget<HbPushButton>( DOCML::MV_NAME_PREV_BUTTON );
    mRightButton = uiLoader.findWidget<HbPushButton>( DOCML::MV_NAME_NEXT_BUTTON );
    initButtons();

    mManualSeekTimer = new QTimer( this );
    mManualSeekTimer->setInterval( MANUALSEEK_START_TIMEOUT );
    mManualSeekTimer->setSingleShot( true );
    Radio::connect( mManualSeekTimer,   SIGNAL(timeout()),
                    this,               SLOT(toggleManualSeek()) );

    Radio::connect( mLeftButton,    SIGNAL(clicked()),
                    this,           SLOT(handleLeftButton()) );
    Radio::connect( mRightButton,   SIGNAL(clicked()),
                    this,           SLOT(handleRightButton()) );
    Radio::connect( mLeftButton,    SIGNAL(longPress(QPointF)),
                    this,           SLOT(handleLongLeftButton()) );
    Radio::connect( mRightButton,   SIGNAL(longPress(QPointF)),
                    this,           SLOT(handleLongRightButton()) );

    Radio::connect( this,           SIGNAL(scrollingEnded()),
                    this,           SLOT(handleScrollingEnd()) );

    grabGesture( Qt::SwipeGesture );

    //TODO: Remove. Stepsize hardcoded to 100 Khz in europe region during demo
    if ( mFrequencyStepSize < ONE_HUNDRED_KHZ ) {
        mFrequencyStepSize = ONE_HUNDRED_KHZ;
    }

    RadioStationModel* stationModel = &mUiEngine->stationModel();
    Radio::connect( stationModel,   SIGNAL(rowsInserted(QModelIndex,int,int)),
                    this,           SLOT(updateStation(QModelIndex,int,int)) );
    Radio::connect( stationModel,   SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                    this,           SLOT(removeStation(QModelIndex,int,int)) );
    Radio::connect( stationModel,   SIGNAL(modelReset()),
                    this,           SLOT(initEmptyItems()) );

    updateAllItems();

    showButtons();
}

/*!
 *
 */
void RadioFrequencyStrip::setFrequency( const uint frequency, int reason, Scroll::Direction direction )
{
    Q_UNUSED( reason );
    LOG_FORMAT( "RadioFrequencyStrip::setFrequency, frequency: %d, reason: %d", frequency, reason );
    if ( mFrequencies.contains( frequency ) ) {
        if ( frequency != mFrequency || reason == TuneReason::Skip ) {
            scrollToFrequency( frequency, direction, mAutoScrollTime, RadioUtil::tuneReason( reason ) );
        }
    }
}

/*!
 *
 */
uint RadioFrequencyStrip::frequency() const
{
    return mFrequency;
}

/*!
 *
 */
bool RadioFrequencyStrip::isInManualSeekMode() const
{
    return mManualSeekMode;
}

/*!
 *
 */
void RadioFrequencyStrip::cancelManualSeek()
{
    mManualSeekTimer->stop();
    if ( mManualSeekMode ) {
        toggleManualSeek();
    }
}

/*!
 *
 */
void RadioFrequencyStrip::addScannedStation( const RadioStation& station )
{
    FrequencyPos pos = mFrequencies.value( station.frequency() );
    updateItem( pos.mItem, station.frequency() );
}

/*!
 *
 */
void RadioFrequencyStrip::updateFavorite( const RadioStation& station )
{
    LOG_SLOT_CALLER;
    FrequencyPos pos = mFrequencies.value( station.frequency() );
    updateItem( pos.mItem );
}

/*!
 * Public slot
 *
 */
void RadioFrequencyStrip::setScanningMode( bool isScanning )
{
    mManualSeekTimer->stop();
    if ( isScanning ) {
        initEmptyItems();
        hideButtons();
        scrollToFrequency( mMinFrequency, Scroll::Shortest, mAutoScrollTime );
    } else {
        showButtons();
    }
    setEnabled( !isScanning );
}

/*!
 * Private slot
 *
 */
void RadioFrequencyStrip::removeStation( const QModelIndex& parent, int first, int last )
{
    Q_UNUSED( parent );
    updateStationsInRange( first, last, true );
}

/*!
 * Private slot
 *
 */
void RadioFrequencyStrip::updateStation( const QModelIndex& parent, int first, int last )
{
    Q_UNUSED( parent );
    updateStationsInRange( first, last );
}

/*!
 * Private slot
 *
 */
void RadioFrequencyStrip::initEmptyItems()
{
    LOG_METHOD;
    //TODO: Remove the weird hack of checking the sender
    if ( !sender() || RadioUtil::scanStatus() != Scan::ScanningInMainView ) {
        QList<RadioStation> emptyList;
        foreach ( RadioFrequencyItem* item, mFrequencyItems ) {
            QPixmap pixmap = drawPixmap( item->frequency(), emptyList, item );
            item->setPixmap( pixmap );
        }
    }
}

/*!
 * Private slot
 */
void RadioFrequencyStrip::handleLeftButton()
{
    emit skipRequested( StationSkip::PreviousFavorite );
}

/*!
 * Private slot
 */
void RadioFrequencyStrip::handleLongLeftButton()
{
    emit seekRequested( Seek::Down );
}

/*!
 * Private slot
 */
void RadioFrequencyStrip::handleRightButton()
{
    emit skipRequested( StationSkip::NextFavorite );
}

/*!
 * Private slot
 */
void RadioFrequencyStrip::handleLongRightButton()
{
    emit seekRequested( Seek::Up );
}

/*!
 * Private slot
 *
 */
void RadioFrequencyStrip::toggleManualSeek()
{
    mManualSeekMode = !mManualSeekMode;
    emit manualSeekChanged( mManualSeekMode );

    if ( mManualSeekMode ) {
        grabMouse();
        hideButtons();
        mManualSeekTimerId = startTimer( MANUALSEEK_SIGNAL_DELAY );
    } else {
        ungrabMouse();
        showButtons();
        killTimer( mManualSeekTimerId );
        mManualSeekTimerId = 0;
    }
}

/*!
 * Private slot
 */
void RadioFrequencyStrip::handleScrollingEnd()
{
    // Check if the selector is in the invalid area where the strip loops around
    const int selectorPosition = selectorPos();
    if ( mManualSeekMode ) {
        if ( !mPositions.contains( selectorPosition ) ) {
            if ( selectorPosition < mMaxWidth - ITEM_WIDTH + mSeparatorPos ) {
                scrollToFrequency( mMaxFrequency, Scroll::Shortest, 500 );
                emit frequencyChanged( mMaxFrequency, TuneReason::ManualSeekUpdate, Scroll::Shortest );
            } else {
                scrollToFrequency( mMinFrequency, Scroll::Shortest, 500 );
                emit frequencyChanged( mMinFrequency, TuneReason::ManualSeekUpdate, Scroll::Shortest );
            }
        }

        mManualSeekTimer->start( MANUALSEEK_END_TIMEOUT );
    }
}

/*!
 * \reimp
 */
void RadioFrequencyStrip::updateItemPrimitive( QGraphicsItem* itemToUpdate, int itemIndex )
{
    QGraphicsPixmapItem* item = static_cast<QGraphicsPixmapItem*>( itemToUpdate );
    if ( itemIndex < mFrequencyItems.count() ) {
        item->setPixmap( mFrequencyItems.at( itemIndex )->updatePrimitive( item ) );
    }
}

/*!
 * \reimp
 */
QGraphicsItem* RadioFrequencyStrip::createItemPrimitive( QGraphicsItem* parent )
{
    return new QGraphicsPixmapItem( parent );
}

/*!
 * \reimp
 */
void RadioFrequencyStrip::scrollPosChanged()
{
    if ( mManualSeekMode ) {
        const int pos = selectorPos();
        const uint frequency = mPositions.value( pos );
        if ( frequency > 0 ) {
            mFrequency = mPositions.value( pos );
            emit frequencyChanged( mFrequency, TuneReason::ManualSeekUpdate, Scroll::Shortest );
        }
    }
}

/*!
 * \reimp
 */
void RadioFrequencyStrip::resizeEvent ( QGraphicsSceneResizeEvent* event )
{
    LOG_METHOD;
    RadioStripBase::resizeEvent( event );

    initSelector();

    const qreal height = event->newSize().height();
    const qreal width = event->newSize().width();

    mSelectorPos = width / 2;
    mSelectorImage->setOffset( mSelectorPos - (INDICATOR_WIDTH / 2), 0.0 );

    if ( mLeftButton && mRightButton ) {
        mLeftButton->resize( height, height );
        mRightButton->resize( height, height );
        mRightButton->setPos( QPointF( width - height, 0 ) );
    }

    scrollToFrequency( mFrequency );
}

/*!
 * \reimp
 */
//void RadioFrequencyStrip::showEvent( QShowEvent* event )
//{
//    RadioStripBase::showEvent( event );
//    scrollToFrequency( mFrequency );
//}

/*!
 * \reimp
 */
void RadioFrequencyStrip::changeEvent( QEvent* event )
{
    if ( event->type() == HbEvent::ThemeChanged ) {
        // Update the foreground color and redraw each item
        mForegroundColor = Qt::white;// HbColorScheme::color( TEXT_COLOR_ATTRIBUTE );
        updateAllItems();
    }

    return RadioStripBase::changeEvent(event);
}

/*!
 * \reimp
 */
void RadioFrequencyStrip::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    RadioStripBase::mousePressEvent( event );

    mManualSeekTimer->stop();
    if ( mManualSeekMode ) {
        const bool insideStrip = rect().contains( event->pos() );
        if ( !insideStrip ) {
            toggleManualSeek();
        }
    } else {
        mManualSeekTimer->start( MANUALSEEK_START_TIMEOUT );
    }
}

/*!
 * \reimp
 */
void RadioFrequencyStrip::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
    RadioStripBase::mouseReleaseEvent( event );

    mManualSeekTimer->stop();
    if ( mManualSeekMode && !isScrolling() ) {
        mManualSeekTimer->start( MANUALSEEK_END_TIMEOUT );
    }
}

/*!
 * \reimp
 */
void RadioFrequencyStrip::gestureEvent( QGestureEvent* event )
{
    HbSwipeGesture* swipeGesture = static_cast<HbSwipeGesture*>( event->gesture( Qt::SwipeGesture ) );
    if ( swipeGesture && !mManualSeekMode ) {
        if ( swipeGesture->state() == Qt::GestureFinished ) {
            if ( swipeGesture->horizontalDirection() == QSwipeGesture::Left ) {
                emit skipRequested( StationSkip::Next );
            } else if ( swipeGesture->horizontalDirection() == QSwipeGesture::Right ) {
                emit skipRequested( StationSkip::Previous );
            }
        }
    } else if ( mManualSeekMode ) {
        RadioStripBase::gestureEvent( event );
    }
}

/*!
 * \reimp
 */
void RadioFrequencyStrip::timerEvent( QTimerEvent* event )
{
    Q_UNUSED( event );
    if ( mLastReportedFrequency != mFrequency ) {
        mLastReportedFrequency = mFrequency;
        emit frequencyChanged( mFrequency, TuneReason::ManualSeekTune, Scroll::Shortest );
    }
}

/*!
 *
 */
void RadioFrequencyStrip::initModel()
{
    const uint minFreq = uint( qreal(mMinFrequency) / ONE_HERTZ + ROUNDER );
    const uint maxFreq = uint( qreal(mMaxFrequency) / ONE_HERTZ + 0.9 ); // always round up

    QStringList list;
    QString freqText;
    for ( uint i = minFreq; i <= maxFreq; ++i ) {
        freqText = QString::number( i );
        list.append( freqText );
        mFrequencyItems.append( new RadioFrequencyItem( freqText ) );
    }
    mFrequencyItems.append( new RadioFrequencyItem( "" ) );
    list.append( "" );

    mMaxWidth = list.count() * ITEM_WIDTH;

    mSeparatorPos = qreal(ITEM_WIDTH) / 2;
    const uint minDrawableFreq = minFreq * ONE_HERTZ - HALF_HERTZ;;
    const uint maxDrawableFreq = maxFreq * ONE_HERTZ + HALF_HERTZ;
    mSeparatorPos += qreal( ( mMinFrequency  - minDrawableFreq ) / 2 ) / PIXEL_IN_HZ;
    mSeparatorPos -= qreal( ( maxDrawableFreq - mMaxFrequency ) / 2 ) / PIXEL_IN_HZ;

    setModel( new QStringListModel( list, this ) );
}

/*!
 *
 */
void RadioFrequencyStrip::initSelector()
{
    QPixmap selectorPixmap = QPixmap( QSize( SELECTOR_WIDTH, (int)size().height() ) );
    selectorPixmap.fill( Qt::red );
    mSelectorImage->setPixmap( selectorPixmap );
    mSelectorImage->setZValue( SELECTOR_Z_POS );
}

/*!
 *
 */
void RadioFrequencyStrip::initPositions()
{
    int prevPos = 0;
    int nextPos = 0;
    const int lastPos = mFrequencies.value( mMaxFrequency ).mPosition;
    for ( int i = mFrequencies.value( mMinFrequency ).mPosition; i < lastPos; ++i ) {
        if ( mPositions.contains( i ) ) {
            prevPos = i;
            const uint freq = mPositions.value( prevPos ) + mFrequencyStepSize;
            if ( mFrequencies.contains( freq ) ) {
                nextPos = mFrequencies.value( freq ).mPosition;
            } else {
                nextPos = prevPos;
            }
        } else {
            const int nearestHit = ( i - prevPos ) < ( nextPos - i ) ? prevPos : nextPos;
            mPositions.insert( i, mPositions.value( nearestHit ) );
        }
    }
}

/*!
 *
 */
void RadioFrequencyStrip::initButtons()
{
    mLeftButton->setZValue( SELECTOR_Z_POS );
    mLeftButton->setObjectName( LEFT_BUTTON );
    mRightButton->setZValue( SELECTOR_Z_POS );
    mRightButton->setObjectName( RIGHT_BUTTON );

    const qreal height = size().height();
    const qreal width = size().width();
    mLeftButton->resize( height, height );
    mRightButton->resize( height, height );
    mRightButton->setPos( QPointF( width - height, 0 ) );

    QEffectList effectList;
    effectList.append( EffectInfo( mLeftButton, ":/effects/slide_to_left.fxml", SLIDE_TO_LEFT ) );
    effectList.append( EffectInfo( mLeftButton, ":/effects/slide_from_left.fxml", SLIDE_FROM_LEFT ) );
    effectList.append( EffectInfo( mRightButton, ":/effects/slide_to_right.fxml", SLIDE_TO_RIGHT ) );
    effectList.append( EffectInfo( mRightButton, ":/effects/slide_from_right.fxml", SLIDE_FROM_RIGHT ) );
    RadioUtil::addEffects( effectList );
}

/*!
 *
 */
void RadioFrequencyStrip::addFrequencyPos( int pos, uint frequency, RadioFrequencyItem* item )
{
    mFrequencies.insert( frequency, FrequencyPos( pos, item ) );
    mPositions.insert( pos, frequency );
}

/*!
 *
 */
void RadioFrequencyStrip::updateStationsInRange( int first, int last, bool stationRemoved )
{
    if ( RadioUtil::scanStatus() != Scan::ScanningInMainView ) {
        RadioStationModel& model = mUiEngine->stationModel();
        uint frequency = 0;
        for ( int i = first; i <= last; ++i ) {
            frequency = model.data( model.index( i, 0, QModelIndex() ),
                                    RadioRole::RadioStationRole ).value<RadioStation>().frequency();
            if ( mFrequencies.contains( frequency ) ) {
                FrequencyPos pos = mFrequencies.value( frequency );
                updateItem( pos.mItem, 0, stationRemoved ? frequency : 0 );
            }
        }
    }
}

/*!
 *
 */
void RadioFrequencyStrip::updateItem( RadioFrequencyItem* item, uint upperRange, uint ignoredFrequency )
{
    if ( item ) {
        uint frequency = item->frequency();
        QList<RadioStation> stations;

        if ( upperRange == 0 ) {
            upperRange = frequency + HALF_HERTZ;
        }

        if ( mUiEngine ) {
            stations = mUiEngine->stationsInRange( frequency - HALF_HERTZ, upperRange );
            for ( int i = stations.count() - 1; ignoredFrequency > 0 && i >= 0; --i ) {
                if ( stations[i].frequency() == ignoredFrequency ) {
                    stations.removeAt( i );
                    break;
                }
            }
        }

        QPixmap pixmap = drawPixmap( frequency, stations, item );
        item->setPixmap( pixmap );

        foreach ( const RadioStation& station, stations ) {
            frequency = station.frequency();
            FrequencyPos pos = mFrequencies.value( frequency );
            mFrequencies.insert( frequency, pos );
        }
    }
}

/*!
 *
 */
void RadioFrequencyStrip::updateAllItems()
{
    foreach ( RadioFrequencyItem* item, mFrequencyItems ) {
        updateItem( item );
    }
}

/*!
 *
 */
QPixmap RadioFrequencyStrip::drawPixmap( uint frequency, QList<RadioStation> stations, RadioFrequencyItem* item )
{
    QPixmap pixmap( PIXMAP_WIDTH, mItemHeight );
    pixmap.fill( Qt::transparent );
    QPainter painter( &pixmap );
    QPen normalPen = painter.pen();
    QPen favoritePen = normalPen;
    normalPen.setColor( mForegroundColor );
    painter.setPen( normalPen );

    if ( frequency == 0 ) {
        painter.drawLine( makeTab( mSeparatorPos - 1 + ROUNDER, mItemHeight ) );
        painter.drawLine( makeTab( mSeparatorPos + ROUNDER, mItemHeight ) );
        return pixmap;
    }

    const QString itemText = QString::number( frequency / ONE_HERTZ );
    const uint startFrequency = frequency - HALF_HERTZ;
    const uint endFrequency = startFrequency + ONE_HERTZ;
    const uint  roundedMin = int( qreal(mMinFrequency) / ONE_HERTZ + ROUNDER );
    const uint freq = frequency / ONE_HERTZ;
    const int diff = freq - roundedMin;
    const qreal startPixel = diff * ITEM_WIDTH;
    qreal pixels = 0.0;
    const qreal leftOverlap = PIXMAP_OVERLAP / 2;

    for ( uint frequency = startFrequency; frequency <= endFrequency; frequency += mFrequencyStepSize ) {

        if ( frequency < mMinFrequency || frequency > mMaxFrequency ) {
            continue;
        }

        pixels = qreal( frequency - startFrequency ) / PIXEL_IN_HZ;
        if ( frequency % ONE_HERTZ == 0 ) {

            // Draw the high frequency tab and the frequency text for the even number
            normalPen.setWidth( 3 );
            painter.setPen( normalPen );
            painter.drawLine( makeTab( pixels + leftOverlap - 1, TAB_HEIGHT_BIG ) );
            normalPen.setWidth( 1 );
            painter.setPen( normalPen );

            // Draw the frequency text and its 00 decimals
            painter.setFont( DECIMAL_FONT );
            const int decimalWidth = painter.fontMetrics().width( DECIMAL_TEXT );
            const int decimalSpace = painter.fontMetrics().leftBearing( '.' );
            painter.setFont( FREQUENCY_FONT );
            const int frequencyWidth = painter.fontMetrics().width( itemText );
            const int textPosX = int( pixels + leftOverlap - ( frequencyWidth + decimalSpace + decimalWidth ) / 2 );
            painter.drawText( QPoint( textPosX, 20 ), itemText );
            painter.setFont( DECIMAL_FONT );
            painter.drawText( QPoint( textPosX + frequencyWidth + decimalSpace, 20 ), DECIMAL_TEXT );

        } else if ( frequency % ONE_TAB_IN_HZ == 0 ) {

            // Draw the low frequency tab for the uneven number
            painter.drawLine( makeTab( pixels + leftOverlap, TAB_HEIGHT_SMALL ) );

        }

        addFrequencyPos( int( startPixel + pixels + ROUNDER ), frequency, item );
    }

    // Draw favorites and local stations
    favoritePen.setColor( Qt::yellow );

    const int markerYPos = mItemHeight - 28;
    foreach ( const RadioStation& station, stations ) {
        const uint frequency = station.frequency();
        pixels = qreal( frequency - startFrequency ) / PIXEL_IN_HZ;

        if ( station.isFavorite() ) {
            favoritePen.setWidth( PEN_WIDTH_FAVORITE );
            painter.setPen( favoritePen );
            painter.drawEllipse( int( pixels + leftOverlap - 3 ), markerYPos - 3, 6, 6 );
        } else if ( station.isType( RadioStation::LocalStation ) ) {

            favoritePen.setWidth( 1 );
            painter.setPen( favoritePen );
            painter.drawEllipse( int( pixels + leftOverlap - 4 ), markerYPos - 4, 8, 8 );
        }
    }

    return pixmap;
}

/*!
 *
 */
QLineF RadioFrequencyStrip::makeTab( qreal pos, int height )
{
    return QLineF( pos, mItemHeight - height, pos, mItemHeight );
}

/*!
 *
 */
void RadioFrequencyStrip::emitFrequencyChanged( uint frequency )
{
    if ( frequency > 0 && frequency != mFrequency ) {
        mFrequency = frequency;
        //TOD: Get direction
        emit frequencyChanged( frequency, TuneReason::FrequencyStrip, Scroll::Shortest );
    }
}

/*!
 *
 */
int RadioFrequencyStrip::selectorPos() const
{
    const int pos = int( -contentWidget()->x() + mSelectorPos + ROUNDER );
    return pos % mMaxWidth;
}

/*!
 *
 */
void RadioFrequencyStrip::scrollToFrequency( uint frequency, Scroll::Direction direction, int time, TuneReason::Reason reason )
{
    int targetFreqPos = mFrequencies.value( frequency ).mPosition;
    const int currentFreqPos = mFrequencies.value( mFrequency ).mPosition;
    const int currentPos = int( -contentWidget()->x() + mSelectorPos + ROUNDER );
    const bool isInSecondHalf = currentPos >= mMaxWidth - ITEM_WIDTH + mSeparatorPos;

    // Special case: When there is only one favorite and the user presses skip
    // the strip must scroll around to the same frequency.
    if ( mFrequency == frequency && reason == TuneReason::Skip ) {
        if ( direction == Scroll::Left ) {
            targetFreqPos += mMaxWidth;
        } else {
        targetFreqPos -= mMaxWidth;
        }
    }

    mFrequency = frequency;

    qreal newPos = targetFreqPos;
    if ( direction == Scroll::Left ) {

        if ( currentFreqPos > targetFreqPos ) {
            newPos += mMaxWidth;
        }

    } else if ( direction == Scroll::Right ) {

        if ( currentFreqPos < targetFreqPos ) {
            newPos -= mMaxWidth;
        }

    }

    if ( isInSecondHalf ) {
        newPos += mMaxWidth;
    }

    newPos -= mSelectorPos - ROUNDER;

//    scrollContentsTo( QPointF( newPos, 0 ), time );
// TODO: Remove this and uncomment the above line. This is a temporary workaround to get the strip to move
    scrollContentsTo( QPointF( newPos, 0 ), 0 );
}

/*!
 *
 */
void RadioFrequencyStrip::hideButtons()
{
    HbEffect::start( mLeftButton, SLIDE_TO_LEFT );
    HbEffect::start( mRightButton, SLIDE_TO_RIGHT );
}

/*!
 *
 */
void RadioFrequencyStrip::showButtons()
{
    HbEffect::start( mLeftButton, SLIDE_FROM_LEFT );
    HbEffect::start( mRightButton, SLIDE_FROM_RIGHT );
}
