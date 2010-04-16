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

#include "radiofrequencystrip.h"
#include "radiofrequencyitem.h"
#include "radiouiengine.h"
#include "radiostation.h"
#include "radiouiutilities.h"
#include "radiologger.h"

// Frequency lines
const int KTabHeightSmall = 10;
const int KTabHeightBig = 15;
//const int KTabHeightFavorite = 15;
const int KTabWidthFavorite = 4;
const qreal KIndicatorWidth = 2.0;

const qreal KRounder = 0.5;
const int KSelectorWidth = 2;
const int KSelectorZPos = 100;

const int K100Khz = 100000;

//const int KTouchPosThreshold = 30;

const QString KSlideToLeft      = "SlideToLeft";
const QString KSlideFromLeft    = "SlideFromLeft";
const QString KSlideToRight     = "SlideToRight";
const QString KSlideFromRight   = "SlideFromRight";

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
RadioFrequencyStrip::RadioFrequencyStrip( uint minFrequency,
                                          uint maxFrequency,
                                          uint stepSize,
                                          uint currentFrequency,
                                          RadioUiEngine* engine ) :
    RadioStripBase( 0 ),
    mMinFrequency( minFrequency ),
    mMaxFrequency( maxFrequency ),
    mFrequencyStepSize( stepSize ),
    mEngine( engine ),
    mSelectorImage( new QGraphicsPixmapItem( this ) ),
    mSeparatorPos( 0.0 ),
    mMaxWidth( 0 ),
    mSelectorPos( 0.0 ),
    mFrequency( currentFrequency ),
    mFavoriteSelected( false ),
    mLeftButton( new HbPushButton( this ) ),
    mRightButton( new HbPushButton( this ) ),
    mButtonTimer( new QTimer( this ) ),
    mIsPanGesture( false )
{
    mButtonTimer->setInterval( 500 );
    mButtonTimer->setSingleShot( true );
    connectAndTest( mButtonTimer, SIGNAL(timeout()), this, SLOT(toggleButtons()) );

    //TODO: Remove. Stepsize hardcoded to 100 Khz in europe region during demo
    if ( mFrequencyStepSize < K100Khz ) {
        mFrequencyStepSize = K100Khz;
    }

    setScrollingStyle( HbScrollArea::PanOrFlick );
    setItemSize( QSizeF( KWidth, KHeight ) );
    setFrictionEnabled( true );

    initModel();

    initSelector();

    initItems();

    initButtons();
}

/*!
 *
 */
void RadioFrequencyStrip::setLeftButtonIcon( const HbIcon& leftButtonIcon )
{
    mLeftButtonIcon = leftButtonIcon;
    if ( mLeftButton ) {
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
uint RadioFrequencyStrip::frequency( bool* favorite ) const
{
    if ( favorite ) {
        *favorite = mFrequencies.value( mFrequency ).mFavorite;
    }
    return mFrequency;
}

/*!
 *
 */
void RadioFrequencyStrip::connectLeftButton( const char* signal, QObject* receiver, const char* slot )
{
    connectAndTest( mLeftButton, signal, receiver, slot );
}

/*!
 *
 */
void RadioFrequencyStrip::connectRightButton( const char* signal, QObject* receiver, const char* slot )
{
    connectAndTest( mRightButton, signal, receiver, slot );
}

/*!
 * Public slot
 *
 */
void RadioFrequencyStrip::favoriteChanged( const RadioStation& station )
{
    LOG_SLOT_CALLER;
    FrequencyPos pos = mFrequencies.value( station.frequency() );
    updateFavorites( pos.mItem );

    emitFavoriteSelected( station.isFavorite() );
}

/*!
 * Public slot
 *
 */
void RadioFrequencyStrip::stationAdded( const RadioStation& station )
{
    LOG_SLOT_CALLER;
    FrequencyPos pos = mFrequencies.value( station.frequency() );
    updateFavorites( pos.mItem );
}

/*!
 * Public slot
 *
 */
void RadioFrequencyStrip::stationRemoved( const RadioStation& station )
{
    LOG_SLOT_CALLER;
    uint frequency = station.frequency();
    if ( mFrequencies.contains( frequency ) ) {
        FrequencyPos pos = mFrequencies.value( frequency );
        mFrequencies.remove( frequency );
        updateFavorites( pos.mItem );
    }
}

/*!
 * Public slot
 *
 */
void RadioFrequencyStrip::setFrequency( const uint frequency, int commandSender )
{
//    LOG_SLOT_CALLER;
//    LOG_FORMAT( "RadioFrequencyStrip::setFrequency, frequency: %d, sender: %d", frequency, commandSender );
    if ( commandSender != CommandSender::FrequencyStrip &&  // Not sent by the FrequencyStrip
         frequency != mFrequency &&                         // Different from the current
         mFrequencies.contains( frequency ) )               // 0 frequency means any illegal value
    {
        scrollToFrequency( frequency, mAutoScrollTime );
        emitFrequencyChanged( frequency );
    }
}

/*!
 * Private slot
 *
 */
void RadioFrequencyStrip::leftGesture( int DEBUGVAR( speedPixelsPerSecond ) )
{
    LOG_FORMAT( "RadioFrequencyStrip::leftGesture. speed: %d", speedPixelsPerSecond );
    mButtonTimer->stop();
    mButtonTimer->start();
    emit swipedLeft();
}

/*!
 * Private slot
 *
 */
void RadioFrequencyStrip::rightGesture( int DEBUGVAR( speedPixelsPerSecond ) )
{
    LOG_FORMAT( "RadioFrequencyStrip::rightGesture. speed: %d", speedPixelsPerSecond );
    mButtonTimer->stop();
    mButtonTimer->start();
    emit swipedRight();
}

/*!
 * Private slot
 *
 */
void RadioFrequencyStrip::panGesture( const QPointF& point )
{
    RadioStripBase::panGesture( point );
    mIsPanGesture = true;
}

/*!
 * Private slot
 */
void RadioFrequencyStrip::toggleButtons()
{
    HbEffect::start( mLeftButton, KSlideFromLeft );
    HbEffect::start( mRightButton, KSlideFromRight );
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

    if ( isDragging() ) {
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
//        mLeftButtonIcon.setSize( QSizeF( stripHeight, stripHeight ) );
        mLeftButton->resize( stripHeight, stripHeight );
        mLeftButton->setBackground( mLeftButtonIcon );
    }

    if ( !mRightButtonIcon.isNull() ) {
//        mRightButtonIcon.setSize( QSizeF( stripHeight, stripHeight ) );
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
void RadioFrequencyStrip::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    RadioStripBase::mousePressEvent( event );
    mIsPanGesture = false;
    mButtonTimer->stop();

    HbEffect::start( mLeftButton, KSlideToLeft );
    HbEffect::start( mRightButton, KSlideToRight );
}

/*!
 * \reimp
 */
void RadioFrequencyStrip::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
    RadioStripBase::mouseReleaseEvent( event );

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
    mButtonTimer->start();
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
void RadioFrequencyStrip::initItems()
{
    LOG_METHOD;
    foreach ( RadioFrequencyItem* item, mFrequencyItems ) {
        updateFavorites( item );
    }

    if ( mEngine ) {
        QList<RadioStation> stations = mEngine->stationsInRange( mMinFrequency, mMaxFrequency );
        foreach ( const RadioStation& station, stations ) {
            if ( station.isFavorite() ) {
                mFrequencies[ station.frequency() ].mFavorite = true;
            }
            if ( station.isType( RadioStation::LocalStation ) ) {
                mFrequencies[ station.frequency() ].mLocalStation = true;
            }
        }
    }

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
    mRightButton->setZValue( KSelectorZPos );

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
    mFrequencies.insert( frequency, FrequencyPos( pos, false, false, item ) );
    mPositions.insert( pos, frequency );
}

/*!
 *
 */
void RadioFrequencyStrip::updateFavorites( RadioFrequencyItem* item )
{
    uint frequency = item->frequency();
    QList<RadioStation> stations;
    if ( mEngine ) {
        stations = mEngine->stationsInRange( frequency - KHalfHertz, frequency + KHalfHertz );
    }

    QPixmap pixmap = drawPixmap( frequency, stations, item );
    item->setPixmap( pixmap );

    foreach ( const RadioStation& station, stations ) {
        frequency = station.frequency();
        FrequencyPos pos = mFrequencies.value( frequency );
        pos.mFavorite = station.isFavorite();
        pos.mLocalStation = station.isType( RadioStation::LocalStation );
        mFrequencies.insert( frequency, pos );
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
    normalPen.setColor( Qt::white );
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
//    LOG_FORMAT( "RadioFrequencyStrip::emitFrequencyChanged, frequency: %d", frequency );
    if ( frequency > 0 && frequency != mFrequency ) {
        mFrequency = frequency;
        emit frequencyChanged( frequency, CommandSender::FrequencyStrip );
        emitFavoriteSelected( mFrequencies.value( frequency ).mFavorite );
    }
}

/*!
 *
 */
void RadioFrequencyStrip::emitFavoriteSelected( bool favoriteSelected )
{
    // TODO: remove this
    if ( favoriteSelected != mFavoriteSelected ) {
        mFavoriteSelected = favoriteSelected;
        emit frequencyIsFavorite( mFavoriteSelected );
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
