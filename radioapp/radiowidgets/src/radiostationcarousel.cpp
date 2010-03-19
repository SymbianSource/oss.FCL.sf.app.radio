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
#include <qgraphicslinearlayout>
#include <hbanchorlayout.h>
#include <hbpushbutton.h>
#include <qpixmap>
#include <QGraphicsSceneMouseEvent>
#include <hbeffect>

// User includes
#include "radiostationcarousel.h"
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

// =============================================
// Station Item
// =============================================

/*!
 *
 */
RadioStationItem::RadioStationItem( QGraphicsItem* parent ) :
    HbAbstractViewItem( parent ),
    mLayout( 0 ),
    mNameLabel( 0 ),
    mIconButton( 0 ),
    mGenreLabel( 0 ),
    mRadiotextLabel( 0 )
{
    setFlag( QGraphicsItem::ItemIsFocusable, true );
}

/*!
 * From HbAbstractViewItem
 *
 */
HbAbstractViewItem* RadioStationItem::createItem()
{
    return new RadioStationItem( *this ); // Calls copy constructor
}

/*!
 * From HbAbstractViewItem
 */
void RadioStationItem::updateChildItems()
{
    if ( !mLayout )
    {
        mNameLabel = new RadioFadingLabel( this );
        HbFontSpec spec = mNameLabel->fontSpec();
        spec.setTextPaneHeight( 40 );
        spec.setRole( HbFontSpec::Primary );
        mNameLabel->setFontSpec( spec );
        mNameLabel->setAlignment( Qt::AlignLeft );

        spec.setRole( HbFontSpec::Secondary );
        spec.setPointSize( 6 );

        mIconButton = new HbPushButton( this );
        QPixmap background( QSize( 50, 50 ) );
        background.fill( Qt::transparent );        
        mIconButton->setBackground( HbIcon( background ) );
        HbIcon favoriteIcon( KFavoriteIconPath );
        mIconButton->setOrientation( Qt::Horizontal );
        mIconButton->setIcon( favoriteIcon );
        mIconButton->setPreferredSize( 50, 50 );
        connectAndTest( mIconButton, SIGNAL(clicked()), this, SLOT(toggleFavorite()));

        mGenreLabel = new RadioFadingLabel( this );
        mGenreLabel->setAlignment( Qt::AlignCenter );
//        mGenreLabel->setFadingEnabled( true );    TODO
        mGenreLabel->setFontSpec( spec );
        mGenreLabel->setTextColor( Qt::white );

        mRadiotextLabel = new RadioFadingLabel( this );
        mRadiotextLabel->setAlignment( Qt::AlignCenter );
        mRadiotextLabel->setTextWrapping( Hb::TextWordWrap );
//        mRadiotextLabel->setFadingEnabled( true );    TODO
        mRadiotextLabel->setFontSpec( spec );
        mRadiotextLabel->setTextColor( Qt::white );

        mLayout = new HbAnchorLayout();

        mLayout->setAnchor( mLayout, Hb::TopEdge, mIconButton, Hb::TopEdge, 40.0 );
        mLayout->setAnchor( mLayout, Hb::LeftEdge, mIconButton, Hb::LeftEdge, 20.0 );

        mLayout->setAnchor( mLayout, Hb::TopEdge, mNameLabel, Hb::TopEdge, 40.0 );
        mLayout->setAnchor( mIconButton, Hb::RightEdge, mNameLabel, Hb::LeftEdge, 10.0 );
        mLayout->setAnchor( mLayout, Hb::RightEdge, mNameLabel, Hb::RightEdge, 10.0 );

        mLayout->setAnchor( mNameLabel, Hb::BottomEdge, mGenreLabel, Hb::TopEdge, 0.0 );

        mLayout->setAnchor( mLayout, Hb::LeftEdge, mGenreLabel, Hb::LeftEdge, 10.0 );
        mLayout->setAnchor( mLayout, Hb::CenterHEdge, mGenreLabel, Hb::CenterHEdge, 0.0 );

        mLayout->setAnchor( mGenreLabel, Hb::BottomEdge, mRadiotextLabel, Hb::TopEdge, 0.0 );
        mLayout->setAnchor( mLayout, Hb::LeftEdge, mRadiotextLabel, Hb::LeftEdge, 10.0 );
        mLayout->setAnchor( mLayout, Hb::CenterHEdge, mRadiotextLabel, Hb::CenterHEdge, 0.0 );
        mLayout->setAnchor( mLayout, Hb::BottomEdge, mRadiotextLabel, Hb::BottomEdge, -20.0 );

        setLayout( mLayout );
    }

    update();
}

/*!
 * Private slot
 *
 */
void RadioStationItem::toggleFavorite()
{
    carousel()->uiEngine().model().setData( modelIndex(), mFrequency, RadioStationModel::ToggleFavoriteRole );
}

/*!
 *
 */
uint RadioStationItem::frequency() const
{
    return mFrequency;
}

/*!
 *
 */
void RadioStationItem::update( const RadioStation* station )
{
    QModelIndex index = modelIndex();
    if ( !( station && station->isValid() ) && !index.isValid() )
        {
        return;
        }

    RadioStation tempStation = ( station && station->isValid() ) ? *station
                    : index.data( RadioStationModel::RadioStationRole ).value<RadioStation>();

    mNameLabel->setTextWithoutFading( RadioUiEngine::nameOrFrequency( tempStation ) );
    QString dynamicPs = tempStation.dynamicPsText();
    mGenreLabel->setText( dynamicPs.isEmpty() ? carousel()->uiEngine().genreToString( tempStation.genre() ) : dynamicPs );
    mRadiotextLabel->setText( carousel()->isAntennaAttached() ? tempStation.radioText() : TRANSLATE(KConnectHeadsetAntenna) );
    mFrequency = tempStation.frequency();

    updateFavoriteIcon( tempStation.isFavorite() );
}

/*!
 *
 */
void RadioStationItem::setFrequency( uint frequency )
{
    LOG_FORMAT( "RadioStationItem::setFrequency: %u", frequency );
    mNameLabel->setTextWithoutFading( RadioUiEngine::parseFrequency( frequency ) );
    mGenreLabel->setTextWithoutFading( "" );
    mRadiotextLabel->setTextWithoutFading( carousel()->isAntennaAttached() ? "" : TRANSLATE(KConnectHeadsetAntenna) );
    mFrequency = frequency;
    updateFavoriteIcon( false );
}

/*!
 *
 */
void RadioStationItem::setSeekingText()
{
    mNameLabel->setTextWithoutFading( TRANSLATE( KHeadingSeeking ) );
    mGenreLabel->setTextWithoutFading( "" );
    mRadiotextLabel->setTextWithoutFading( "" );
}

/*!
 *
 */
void RadioStationItem::updateFavoriteIcon( bool isFavorite )
{
//    mIconButton->setOpacity( isFavorite ? 1.0 : 0.5 );
    mIconButton->setIcon( isFavorite ? KFavoriteIconPath : KNonFavoriteIconPath );
}

/*!
 *
 */
RadioStationCarousel* RadioStationItem::carousel()
{
    return static_cast<RadioStationCarousel*>( itemView() );
}

// =============================================
// Station Carousel
// =============================================

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
    mCurrentItem( 0 )
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
    setItemPrototype( new RadioStationItem( this ) );
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
                    this,           SLOT(update(RadioStation)));
    connectAndTest( stationModel,   SIGNAL(dynamicPsChanged(RadioStation)),
                    this,           SLOT(update(RadioStation)));
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
    RadioStationItem* item = currentStationItem();
    if ( item ) {
        item->setSeekingText();
    }
}

/*!
 * Public slot
 */
void RadioStationCarousel::updateHeadsetStatus( bool connected )
{
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
            emit frequencyChanged( static_cast<RadioStationItem*>( item )->frequency(), CommandSender::StationCarousel );
        }
        scrollContentsTo( QPointF( posX, 0 ) , time );
    }
}
