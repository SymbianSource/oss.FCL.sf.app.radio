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

#include "radiofrequencystrip.h"
#include "radiofrequencyitem.h"
#include "radiouiengine.h"
#include "radiostation.h"
#include "radiostationmodel.h"
#include "radiouiutilities.h"
#include "radiologger.h"

// Frequency lines
const int KTabHeightSmall = 10;
const int KTabHeightBig = 15;
const int KTabWidthFavorite = 4;
const qreal KIndicatorWidth = 2.0;

const qreal KRounder = 0.5;
const int KSelectorWidth = 2;
const int KSelectorZPos = 100;

const int KHalfHertz = KOneHertz / 2;
const int KOneTabDistance = 15;
const uint KOneTabInHz = 0.2 * KOneHertz;
const qreal KPixelInHz = KOneTabInHz / KOneTabDistance;
const int KWidth = KOneTabDistance * 5;
//const int KPixmapWidth = KWidth + KOneTabDistance;
const int KHeight = 50;                 //TODO: Remove hardcoding

const int K100Khz = 100000;

const char* KSlideToLeft        = "SlideToLeft";
const char* KSlideFromLeft      = "SlideFromLeft";
const char* KSlideToRight       = "SlideToRight";
const char* KSlideFromRight     = "SlideFromRight";
const char* LEFT_BUTTON         = "tv:left_button";
const char* RIGHT_BUTTON        = "tv:right_button";

const char* TEXT_COLOR_ATTRIBUTE = "text";
const int BUTTON_HIDE_TIMEOUT = 500;
const int BUTTON_SHOW_TIMEOUT = 1000;

/*!
 *
 */
static QLineF makeTab( qreal pos, int height )
{
    return QLineF( pos, KHeight - height, pos, KHeight );
}

/*!
 *
 */
RadioFrequencyStrip::RadioFrequencyStrip() :
    RadioStripBase(),
    mUiEngine( NULL ),
    mMinFrequency( 87500000 ),
    mMaxFrequency( 108000000 ),
    mFrequencyStepSize( 100000 ),
    mFrequency( 87500000 ),
    mSelectorImage( new QGraphicsPixmapItem( this ) ),
    mSeparatorPos( 0.0 ),
    mMaxWidth( 0 ),
    mSelectorPos( 0.0 ),
    mLeftButton( new HbPushButton( this ) ),
    mRightButton( new HbPushButton( this ) ),
    mButtonTimer( NULL ),
    mButtonsVisible( true ),
    mUserIsScrolling( false ),
    mForegroundColor( Qt::white )//HbColorScheme::color( TEXT_COLOR_ATTRIBUTE ) )
{
    RadioUiUtilities::setFrequencyStrip( this );

    setScrollingStyle( HbScrollArea::Pan );
    setItemSize( QSizeF( KWidth, KHeight ) );
    setFrictionEnabled( true );

    initModel();

    initSelector();

    initButtons();

    initEmptyItems();

    mFrequency = RadioUiEngine::lastTunedFrequency();
    scrollToFrequency( mFrequency, 0 );
}

/*!
 *
 */
void RadioFrequencyStrip::setLeftButtonIcon( const HbIcon& leftButtonIcon )
{
    mLeftButtonIcon = leftButtonIcon;
    if ( mLeftButton ) {
        mLeftButtonIcon.setColor( Qt::white );
        mLeftButton->setIcon( mLeftButtonIcon );
    }
}

/*!
 *
 */
HbIcon RadioFrequencyStrip::leftButtonIcon() const
{
    return mLeftButtonIcon;
}

/*!
 *
 */
void RadioFrequencyStrip::setRightButtonIcon( const HbIcon& rightButtonIcon )
{
    mRightButtonIcon = rightButtonIcon;
    if ( mRightButton ) {
        mRightButtonIcon.setColor( Qt::white );
        mRightButton->setIcon( mRightButtonIcon );
    }
}

/*!
 *
 */
HbIcon RadioFrequencyStrip::rightButtonIcon() const
{
    return mRightButtonIcon;
}

/*!
 *
 */
void RadioFrequencyStrip::init( RadioUiEngine* engine )
{
    mUiEngine = engine;
    mMinFrequency       = mUiEngine->minFrequency();
    mMaxFrequency       = mUiEngine->maxFrequency();
    mFrequencyStepSize  = mUiEngine->frequencyStepSize();
    mFrequency          = mUiEngine->currentFrequency();
    scrollToFrequency( mFrequency, 0 );

    mButtonTimer = new QTimer( this );
    mButtonTimer->setInterval( BUTTON_HIDE_TIMEOUT );
    mButtonTimer->setSingleShot( true );
    connectAndTest( mButtonTimer, SIGNAL(timeout()),
                    this,         SLOT(toggleButtons()) );

    connectAndTest( mLeftButton,    SIGNAL(clicked()),
                    this,           SLOT(handleLeftButton()) );
    connectAndTest( mRightButton,   SIGNAL(clicked()),
                    this,           SLOT(handleRightButton()) );
    connectAndTest( mLeftButton,    SIGNAL(longPress(QPointF)),
                    this,           SLOT(handleLongLeftButton()) );
    connectAndTest( mRightButton,   SIGNAL(longPress(QPointF)),
                    this,           SLOT(handleLongRightButton()) );

    connectAndTest( this,           SIGNAL(scrollingEnded()),
                    this,           SLOT(checkIllegalPos()) );

    grabGesture( Qt::SwipeGesture );

    //TODO: Remove. Stepsize hardcoded to 100 Khz in europe region during demo
    if ( mFrequencyStepSize < K100Khz ) {
        mFrequencyStepSize = K100Khz;
    }

    RadioStationModel* stationModel = &mUiEngine->stationModel();
    connectAndTest( stationModel,   SIGNAL(rowsInserted(QModelIndex,int,int)),
                    this,           SLOT(updateStation(QModelIndex,int,int)) );
    connectAndTest( stationModel,   SIGNAL(rowsRemoved(QModelIndex,int,int)),
                    this,           SLOT(updateStation(QModelIndex,int,int)) );
    connectAndTest( stationModel,   SIGNAL(modelReset()),
                    this,           SLOT(initEmptyItems()) );

    initPositions();

    void updateItems();
}

/*!
 *
 */
void RadioFrequencyStrip::setFrequency( const uint frequency, int reason )
{
    LOG_FORMAT( "RadioFrequencyStrip::setFrequency, frequency: %d, reason: %d", frequency, reason );
    if ( reason != TuneReason::FrequencyStrip &&            // Not sent by the FrequencyStrip
         frequency != mFrequency &&                         // Different from the current
         mFrequencies.contains( frequency ) )               // 0 frequency means any illegal value
    {
        scrollToFrequency( frequency, mAutoScrollTime );
        if ( reason != TuneReason::Skip && reason != TuneReason::StationScan ) {
            emitFrequencyChanged( frequency );
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
 * Public slot
 *
 */
void RadioFrequencyStrip::updateFavorite( const RadioStation& station )
{
    LOG_SLOT_CALLER;
    FrequencyPos pos = mFrequencies.value( station.frequency() );
    updateFavorites( pos.mItem );
}

/*!
 * Public slot
 *
 */
void RadioFrequencyStrip::setScanningMode( bool isScanning )
{
    mButtonTimer->stop();
    if ( isScanning ) {
        scrollToFrequency( mMinFrequency, mAutoScrollTime );
        hideButtons();
    } else {
        showButtons();

    }
    setEnabled( !isScanning );
}

/*!
 * Private slot
 *
 */
void RadioFrequencyStrip::updateStation( const QModelIndex& parent, int first, int last )
{
    Q_UNUSED( parent );
    RadioStationModel& model = mUiEngine->stationModel();
    uint frequency = 0;
    for ( int i = first; i <= last; ++i ) {
        frequency = model.data( model.index( i, 0, QModelIndex() ),
                                RadioStationModel::RadioStationRole ).value<RadioStation>().frequency();
        if ( mFrequencies.contains( frequency ) ) {
            FrequencyPos pos = mFrequencies.value( frequency );
            updateFavorites( pos.mItem );
        }
    }
}

/*!
 * Private slot
 *
 */
void RadioFrequencyStrip::initEmptyItems()
{
    LOG_METHOD;
    QList<RadioStation> emptyList;
    foreach ( RadioFrequencyItem* item, mFrequencyItems ) {
        QPixmap pixmap = drawPixmap( item->frequency(), emptyList, item );
        item->setPixmap( pixmap );
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
    emit seekRequested( Seeking::Down );
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
    emit seekRequested( Seeking::Up );
}

/*!
 * Private slot
 */
void RadioFrequencyStrip::toggleButtons()
{
    if ( mButtonsVisible ) {
        hideButtons();
    } else {
        showButtons();
    }
}

/*!
 * Private slot
 */
void RadioFrequencyStrip::checkIllegalPos()
{
    // Check if the selector is in the invalid area where the strip loops around
    const int selectorPosition = selectorPos();
    if ( !mPositions.contains( selectorPosition ) ) {
        if ( selectorPosition < mMaxWidth - KWidth + mSeparatorPos ) {
            scrollToFrequency( mMaxFrequency, 500 );
            emitFrequencyChanged( mMaxFrequency );
        } else {
            scrollToFrequency( mMinFrequency, 500 );
            emitFrequencyChanged( mMinFrequency );
        }
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
void RadioFrequencyStrip::scrollPosChanged( QPointF newPosition )
{
    Q_UNUSED( newPosition );

    if ( mUserIsScrolling ) {
        const int pos = selectorPos();
        emitFrequencyChanged( mPositions.value( pos ) );
    }
}

/*!
 * \reimp
 */
void RadioFrequencyStrip::resizeEvent ( QGraphicsSceneResizeEvent* event )
{
    LOG_METHOD_ENTER;
    RadioStripBase::resizeEvent( event );

    mSelectorPos = event->newSize().width() / 2;
    mSelectorImage->setOffset( mSelectorPos - (KIndicatorWidth / 2), 0.0 );

    const int stripHeight = event->newSize().height();
    if ( !mLeftButtonIcon.isNull() ) {
        mLeftButton->resize( stripHeight, stripHeight );
        mLeftButton->setBackground( mLeftButtonIcon );
    }

    if ( !mRightButtonIcon.isNull() ) {
        mRightButton->resize( stripHeight, stripHeight );
        mRightButton->setBackground( mRightButtonIcon );
    }
    mRightButton->setPos( QPointF( size().width() - mRightButton->size().width(), 0 ) );

    scrollToFrequency( mFrequency, 0 );
}

/*!
 * \reimp
 */
void RadioFrequencyStrip::showEvent( QShowEvent* event )
{
    Q_UNUSED( event );
    scrollToFrequency( mFrequency, 0 );
}

/*!
 * \reimp
 */
void RadioFrequencyStrip::changeEvent( QEvent* event )
{
    if ( event->type() == HbEvent::ThemeChanged ) {
        // Update the foreground color and redraw each item
        mForegroundColor = Qt::white;// HbColorScheme::color( TEXT_COLOR_ATTRIBUTE );
        updateItems();
    }

    return HbWidgetBase::changeEvent(event);
}

/*!
 * \reimp
 */
void RadioFrequencyStrip::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    RadioStripBase::mousePressEvent( event );
    mUserIsScrolling = true;
    mButtonTimer->stop();
    mButtonTimer->start( BUTTON_HIDE_TIMEOUT );
}

/*!
 * \reimp
 */
void RadioFrequencyStrip::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
    mUserIsScrolling = false;
    RadioStripBase::mouseReleaseEvent( event );

//    if ( !mIsPanGesture ) {
//        const qreal touchDelta = event->pos().x() - mSelectorPos;
//        const int touchPos = selectorPosition + touchDelta;
//        const uint frequencyAtPos = mPositions.value( touchPos );
//
//        uint foundFrequency = 0;
//        for ( int i = 0; i < 10; ++i ) {
//            const uint delta = i * mFrequencyStepSize;
//            FrequencyPos leftFreq = mFrequencies.value( frequencyAtPos - delta );
//            FrequencyPos rightFreq = mFrequencies.value( frequencyAtPos + delta );
//
//            if ( touchPos - leftFreq.mPosition > KTouchPosThreshold ) {
//                break;
//            }
//
//            if ( leftFreq.mFavorite || leftFreq.mLocalStation ) {
//                foundFrequency = frequencyAtPos - delta;
//                break;
//            } else if ( rightFreq.mFavorite || rightFreq.mLocalStation ) {
//                foundFrequency = frequencyAtPos + delta;
//                break;
//            }
//        }
//
//        if ( foundFrequency > 0 ) {
//            setFrequency( foundFrequency, 0 );
//        }
//    }

    mButtonTimer->stop();
    if ( !mButtonsVisible ) {
        mButtonTimer->start( BUTTON_SHOW_TIMEOUT );
    }
}

/*!
 * \reimp
 */
void RadioFrequencyStrip::gestureEvent( QGestureEvent* event )
{
    if ( HbSwipeGesture* gesture = static_cast<HbSwipeGesture*>( event->gesture( Qt::SwipeGesture ) ) ) {
        if ( gesture->state() == Qt::GestureFinished ) {
            if ( gesture->horizontalDirection() == QSwipeGesture::Left ) {
                emit skipRequested( StationSkip::Next );
            } else if ( gesture->horizontalDirection() == QSwipeGesture::Right ) {
                emit skipRequested( StationSkip::Previous );
            }
        }
    } else {
        RadioStripBase::gestureEvent( event );
    }
}

/*!
 *
 */
void RadioFrequencyStrip::initModel()
{
    const uint minFreq = uint( qreal(mMinFrequency) / KOneHertz + KRounder );
    const uint maxFreq = uint( qreal(mMaxFrequency) / KOneHertz + 0.9 ); // always round up

    QStringList list;
    QString freqText;
    for ( uint i = minFreq; i <= maxFreq; ++i ) {
        freqText = QString::number( i );
        list.append( freqText );
        mFrequencyItems.append( new RadioFrequencyItem( freqText ) );
    }
    mFrequencyItems.append( new RadioFrequencyItem( "" ) );
    list.append( "" );

    mMaxWidth = list.count() * KWidth;

    mSeparatorPos = qreal(KWidth) / 2;
    const uint minDrawableFreq = minFreq * KOneHertz - KHalfHertz;;
    const uint maxDrawableFreq = maxFreq * KOneHertz + KHalfHertz;
    mSeparatorPos += qreal( ( mMinFrequency  - minDrawableFreq ) / 2 ) / KPixelInHz;
    mSeparatorPos -= qreal( ( maxDrawableFreq - mMaxFrequency ) / 2 ) / KPixelInHz;

    setModel( new QStringListModel( list, this ) );
}

/*!
 *
 */
void RadioFrequencyStrip::initSelector()
{
    QPixmap selectorPixmap = QPixmap( QSize( KSelectorWidth, KHeight ) );
    selectorPixmap.fill( Qt::red );
    mSelectorImage->setPixmap( selectorPixmap );
    mSelectorImage->setZValue( KSelectorZPos );
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
    mLeftButton->setZValue( KSelectorZPos );
    mLeftButton->setObjectName( LEFT_BUTTON );
    mRightButton->setZValue( KSelectorZPos );
    mRightButton->setObjectName( RIGHT_BUTTON );

    QEffectList effectList;
    effectList.append( EffectInfo( mLeftButton, ":/effects/slide_to_left.fxml", KSlideToLeft ) );
    effectList.append( EffectInfo( mLeftButton, ":/effects/slide_from_left.fxml", KSlideFromLeft ) );
    effectList.append( EffectInfo( mRightButton, ":/effects/slide_to_right.fxml", KSlideToRight ) );
    effectList.append( EffectInfo( mRightButton, ":/effects/slide_from_right.fxml", KSlideFromRight ) );
    RadioUiUtilities::addEffects( effectList );
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
void RadioFrequencyStrip::updateFavorites( RadioFrequencyItem* item )
{
    if ( item ) {
        uint frequency = item->frequency();
        QList<RadioStation> stations;
        if ( mUiEngine ) {
            stations = mUiEngine->stationsInRange( frequency - KHalfHertz, frequency + KHalfHertz );
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
void RadioFrequencyStrip::updateItems()
{
    foreach ( RadioFrequencyItem* item, mFrequencyItems ) {
        updateFavorites( item );
    }
}

/*!
 *
 */
QPixmap RadioFrequencyStrip::drawPixmap( uint frequency, QList<RadioStation> stations, RadioFrequencyItem* item )
{
    QPixmap pixmap( KWidth, KHeight );
    pixmap.fill( Qt::transparent );
    QPainter painter( &pixmap );
    QPen normalPen = painter.pen();
    QPen favoritePen = normalPen;
    normalPen.setColor( mForegroundColor );
    painter.setPen( normalPen );

    if ( frequency == 0 ) {
        painter.drawLine( makeTab( mSeparatorPos - 1 + KRounder, KHeight ) );
        painter.drawLine( makeTab( mSeparatorPos + KRounder, KHeight ) );
        return pixmap;
    }

    const QString itemText = QString::number( frequency / KOneHertz );
    const uint startFrequency = frequency - KHalfHertz;
    const uint endFrequency = startFrequency + KOneHertz;
    const uint  roundedMin = int( qreal(mMinFrequency) / KOneHertz + KRounder );
    const uint freq = frequency / KOneHertz;
    const int diff = freq - roundedMin;
    const qreal startPixel = diff * KWidth;
    qreal pixels = 0.0;
    QFont painterFont = painter.font();
    painterFont.setPointSize( 6 );
    painter.setFont( painterFont );

    const int charWidth = painter.fontMetrics().averageCharWidth();
    for ( uint frequency = startFrequency; frequency <= endFrequency; frequency += mFrequencyStepSize ) {

        if ( frequency < mMinFrequency || frequency > mMaxFrequency ) {
            continue;
        }

        pixels = qreal( frequency - startFrequency ) / KPixelInHz;
        if ( frequency % KOneHertz == 0 ) {

            // Draw the high frequency tab and the frequency text for the even number
            painter.drawLine( makeTab( pixels, KTabHeightBig ) );
            const int textPosX = pixels - itemText.length() * charWidth / 2;
            painter.drawText( QPoint( textPosX, 20 ), itemText );

        } else if ( frequency % KOneTabInHz == 0 ) {

            // Draw the low frequency tab for the uneven number
            painter.drawLine( makeTab( pixels, KTabHeightSmall ) );

        }

        addFrequencyPos( startPixel + pixels + KRounder, frequency, item );
    }

    // Draw favorites and local stations
    favoritePen.setColor( Qt::yellow );

    const int markerYPos = KHeight - 20;
    foreach ( const RadioStation& station, stations ) {
        const uint frequency = station.frequency();
        pixels = qreal( frequency - startFrequency ) / KPixelInHz;

        if ( station.isFavorite() ) {
            favoritePen.setWidth( KTabWidthFavorite );
            painter.setPen( favoritePen );
            painter.drawEllipse( pixels - 3, markerYPos - 3, 6, 6 );
//            painter.drawPixmap( pixels - 10, 20, 20, 20, mFavoriteIcon.pixmap() );
        } else if ( station.isType( RadioStation::LocalStation ) ) {
//            painter.setPen( normalPen );
//            painter.drawLine( pixels,                           // Start X
//                              KHeight - KTabHeightFavorite,     // Start Y
//                              pixels,                           // End X
//                              KHeight );                        // End Y

            favoritePen.setWidth( 1 );
            painter.setPen( favoritePen );
            painter.drawEllipse( pixels - 4, markerYPos - 4, 8, 8 );
        }
    }

    return pixmap;
}

/*!
 *
 */
void RadioFrequencyStrip::emitFrequencyChanged( uint frequency )
{
    if ( frequency > 0 && frequency != mFrequency ) {
        mFrequency = frequency;
        emit frequencyChanged( frequency, TuneReason::FrequencyStrip );
    }
}

/*!
 *
 */
int RadioFrequencyStrip::selectorPos() const
{
    const int pos = ( -contentWidget()->x() + mSelectorPos ) + KRounder;
    return pos % mMaxWidth;
}

/*!
 *
 */
void RadioFrequencyStrip::scrollToFrequency( uint frequency, int time )
{
    // Find the shortest route to the requested frequency.
    const int pos = ( -contentWidget()->x() + mSelectorPos ) + KRounder;
    if ( pos >= mMaxWidth - KWidth + mSeparatorPos ) {
        const qreal newPos = qreal( mFrequencies.value( frequency ).mPosition ) - mSelectorPos + mMaxWidth;
        scrollContentsTo( QPointF( newPos, 0 ), time );
    } else {
        scrollContentsTo( QPointF( qreal( mFrequencies.value( frequency ).mPosition ) - mSelectorPos, 0 ), time );
    }
}

/*!
 *
 */
void RadioFrequencyStrip::hideButtons()
{
    mButtonsVisible = false;
    HbEffect::start( mLeftButton, KSlideToLeft );
    HbEffect::start( mRightButton, KSlideToRight );
}

/*!
 *
 */
void RadioFrequencyStrip::showButtons()
{
    mButtonsVisible = true;
    HbEffect::start( mLeftButton, KSlideFromLeft );
    HbEffect::start( mRightButton, KSlideFromRight );
}
