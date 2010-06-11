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
#include <HbStyleLoader>
#include <HbPushButton>
#include <HbAnchorLayout>
#include <HbMessageBox>

// User includes
#include "radiocarouselitem.h"
#include "radiostation.h"
#include "radiostationcarousel.h"
#include "radiouiengine.h"
#include "radiofadinglabel.h"
#include "radiostationmodel.h"
#include "radiologger.h"

//static const char* FILE_PATH_WIDGETML   = ":/layout/radiostationitem.widgetml";
//static const char* FILE_PATH_CSS        = ":/layout/radiostationitem.css";
static const char* GENRE_LABEL            = "tv:genre_label";
static const char* NAME_LABEL             = "tv:name_label";
static const char* RADIOTEXT_LABEL        = "tv:radiotext_label";
static const char* URL_LABEL              = "tv:url_label";
//static const char* FAVORITE_BUTTON      = "favorite_button";

const char* SEEKING_TEXT = "txt_rad_list_tuning";
const char* CONNECT_HEADSET_TEXT = "txt_rad_list_connect_wireless_antenna_headset_with";

/*!
 *
 */
RadioCarouselItem::RadioCarouselItem( RadioStationCarousel& carousel ) :
    HbWidget( &carousel ),
    mCarousel( carousel ),
    mData( new Data() )
{
    mData->mStation.reset( new RadioStation() );

    mData->mIconButton.reset( new HbPushButton( this ) );
    QPixmap background( QSize( 70, 70 ) );
    background.fill( Qt::transparent );
    mData->mIconButton->setBackground( HbIcon( background ) );
    mData->mIconButton->setIcon( mCarousel.nonFavoriteIcon() );

    Radio::connect( mData->mIconButton.data(),  SIGNAL(clicked()),
                    &mCarousel,                 SLOT(toggleFavorite()));

    mData->mNameLabel.reset( new RadioFadingLabel( this ) );
    mData->mNameLabel->setAlignment( Qt::AlignCenter );
    mData->mNameLabel->setObjectName( NAME_LABEL );
    HbFontSpec spec = mData->mNameLabel->fontSpec();
    spec.setRole( HbFontSpec::Primary );
    spec.setTextHeight( 40 );
    mData->mNameLabel->setFontSpec( spec );

    spec.setRole( HbFontSpec::Secondary );

    mData->mGenreLabel.reset( new RadioFadingLabel( this ) );
    mData->mGenreLabel->setAlignment( Qt::AlignCenter );
    mData->mGenreLabel->setObjectName( GENRE_LABEL );
    mData->mGenreLabel->setTextColor( Qt::white );

    mData->mRadiotextLabel.reset( new RadioFadingLabel( this ) );
    mData->mRadiotextLabel->setAlignment( Qt::AlignCenter );
    mData->mRadiotextLabel->setObjectName( RADIOTEXT_LABEL );
    mData->mRadiotextLabel->setTextWrapping( Hb::TextWordWrap );
//        mRadiotextLabel->setFadingEnabled( true );    TODO
//        mRadiotextLabel->setFontSpec( spec );
    mData->mRadiotextLabel->setTextColor( Qt::white );

    mData->mUrlLabel.reset( new RadioFadingLabel( this ) );
    mData->mUrlLabel->setAlignment( Qt::AlignCenter );
    mData->mUrlLabel->setObjectName( URL_LABEL );
    mData->mUrlLabel->setTextColor( Qt::white );

    mData->mLayout = new HbAnchorLayout();
    HbAnchorLayout* layout = mData->mLayout;

    HbPushButton* iconButton = mData->mIconButton.data();
    RadioFadingLabel* nameLabel = mData->mNameLabel.data();
    RadioFadingLabel* genreLabel = mData->mGenreLabel.data();
    RadioFadingLabel* urlLabel = mData->mUrlLabel.data();
    RadioFadingLabel* radiotextLabel = mData->mRadiotextLabel.data();

    layout->setAnchor( layout, Hb::TopEdge, iconButton, Hb::TopEdge, 20.0 );
    layout->setAnchor( layout, Hb::LeftEdge, iconButton, Hb::LeftEdge, 20.0 );

    layout->setAnchor( iconButton, Hb::CenterVEdge, genreLabel, Hb::CenterVEdge, 0.0 );
    layout->setAnchor( iconButton, Hb::RightEdge, genreLabel, Hb::LeftEdge, 20.0 );
    layout->setAnchor( layout, Hb::RightEdge, genreLabel, Hb::RightEdge, -70.0 );

    layout->setAnchor( genreLabel, Hb::BottomEdge, nameLabel, Hb::TopEdge, 0.0 );
    layout->setAnchor( layout, Hb::LeftEdge, nameLabel, Hb::LeftEdge, 10.0 );
    layout->setAnchor( layout, Hb::RightEdge, nameLabel, Hb::RightEdge, -10.0 );

    layout->setAnchor( nameLabel, Hb::BottomEdge, radiotextLabel, Hb::TopEdge, 10.0 );
    layout->setAnchor( layout, Hb::LeftEdge, radiotextLabel, Hb::LeftEdge, 10.0 );
    layout->setAnchor( layout, Hb::RightEdge, radiotextLabel, Hb::RightEdge, -10.0 );

    layout->setAnchor( radiotextLabel, Hb::BottomEdge, urlLabel, Hb::TopEdge, 10.0 );
    layout->setAnchor( layout, Hb::LeftEdge, urlLabel, Hb::LeftEdge, 10.0 );
    layout->setAnchor( layout, Hb::RightEdge, urlLabel, Hb::RightEdge, -10.0 );
    layout->setAnchor( layout, Hb::BottomEdge, urlLabel, Hb::BottomEdge, -10.0 );

    setLayout( layout );
}

/*!
 *
 */
RadioCarouselItem::~RadioCarouselItem()
{
}

/*!
 *
 */
void RadioCarouselItem::setStation( const RadioStation& station )
{
    *mData->mStation = station;
    update();
}

/*!
 *
 */
void RadioCarouselItem::swapData( RadioCarouselItem& other )
{
    RadioCarouselItem::Data* tempData = mData.take();
    mData.reset( other.mData.data() );
    other.mData.reset( tempData );

    other.setLayout( other.mData->mLayout );
    setLayout( mData->mLayout );
}

/*!
 *
 */
uint RadioCarouselItem::frequency() const
{
    return mData->mStation->frequency();
}

/*!
 *
 */
void RadioCarouselItem::update( const RadioStation* station )
{
    if ( station ) {
        *mData->mStation = *station;
    }

    if ( mData->mStation->isValid() ) {
        mData->mNameLabel->setTextWithoutFading( mData->mStation->nameOrFrequencyMhz() );

//        mData->mGenreLabel->setText( "Rock Music" );
//        mData->mRadiotextLabel->setText( "This is a bit of radio text that should span many lines" );
//        mData->mUrlLabel->setText( "www.radiorock.fi" );

        mData->mGenreLabel->setText( mCarousel.uiEngine()->genreToString( mData->mStation->genre(), GenreTarget::Carousel ) );

        if ( !mData->mStation->radioText().isEmpty() ) {
            mData->mRadiotextLabel->setText( mData->mStation->radioText() );
        } else if ( !mData->mStation->dynamicPsText().isEmpty() ) {
            mData->mRadiotextLabel->setText( mData->mStation->dynamicPsText() );
        } else {
            mData->mRadiotextLabel->setText( "" );
        }

        mData->mUrlLabel->setText( mData->mStation->url() );

        updateFavoriteIcon( mData->mStation->isFavorite() );
    } else {
        cleanRdsData();
    }
}

/*!
 *
 */
void RadioCarouselItem::setFrequency( uint frequency )
{
//    LOG_FORMAT( "RadioCarouselItem::setFrequency: %d", frequency );

    if ( !mCarousel.mManualSeekMode ) {
        mData->mStation->setFrequency( frequency );
        mData->mNameLabel->setTextWithoutFading( parseFrequency( frequency ) );
        mData->mGenreLabel->setTextWithoutFading( "" );
        mData->mRadiotextLabel->setTextWithoutFading( "" );
        mData->mUrlLabel->setTextWithoutFading( "" );
        updateFavoriteIcon( false );
    } else {
        mData->mNameLabel->setTextWithoutFading( parseFrequency( frequency ) );
    }
}

/*!
 *
 */
void RadioCarouselItem::cleanRdsData()
{
    if ( !mCarousel.mManualSeekMode ) {
        mData->mNameLabel->setTextWithoutFading( "" );
    }
    mData->mGenreLabel->setTextWithoutFading( "" );
    mData->mRadiotextLabel->setTextWithoutFading( "" );
    mData->mUrlLabel->setTextWithoutFading( "" );
    mData->mIconButton->setIcon( HbIcon( "" ) );
}

/*!
 *
 */
void RadioCarouselItem::handleLongPress( const QPointF& /*coords*/ )
{
//    QString text = QString( "Selected frequency: %1" ).arg( mFrequency );
//    HbMessageBox::information( text );
}

/*!
 *
 */
void RadioCarouselItem::setRadioText( const QString& text )
{
    mData->mRadiotextLabel->setHtml( text );
}

/*!
 *
 */
void RadioCarouselItem::setSeekLayout( bool seekLayout )
{
    HbFontSpec spec = mData->mNameLabel->fontSpec();
    mData->mSeekLayout = seekLayout;
    if ( seekLayout ) {
        cleanRdsData();
        spec.setTextHeight( 60 );
    } else {
        spec.setTextHeight( 40 );
    }
    mData->mNameLabel->setFontSpec( spec );
}

/*!
 *
 */
void RadioCarouselItem::setItemVisibility( ItemVisibility visibility )
{
    mData->mIconButton->setEnabled( true );
    if ( visibility == AllVisible ) {
        mData->mNameLabel->setVisible( true );
        mData->mGenreLabel->setVisible( true );
        mData->mRadiotextLabel->setVisible( true );
        mData->mUrlLabel->setVisible( true );
        mData->mIconButton->setVisible( true );
    } else if ( visibility == AllHidden ) {
        mData->mNameLabel->setVisible( false );
        mData->mGenreLabel->setVisible( false );
        mData->mRadiotextLabel->setVisible( false );
        mData->mUrlLabel->setVisible( false );
        mData->mIconButton->setVisible( false );
    } else if ( visibility == IconVisible ) {
        mData->mNameLabel->setVisible( false );
        mData->mGenreLabel->setVisible( false );
        mData->mRadiotextLabel->setVisible( false );
        mData->mUrlLabel->setVisible( false );
        mData->mIconButton->setVisible( true );
        mData->mIconButton->setEnabled( false );
    }
}

/*!
 *
 */
void RadioCarouselItem::setIconOpacity( qreal opacity )
{
    mData->mIconButton->setOpacity( opacity );
}

/*!
 *
 */
void RadioCarouselItem::updateFavoriteIcon( bool isFavorite )
{
    if ( !mData->mSeekLayout ) {
        if ( isFavorite ) {
            mData->mIconButton->setIcon( mCarousel.favoriteIcon() );
        } else {
            mData->mIconButton->setIcon( mCarousel.nonFavoriteIcon() );
        }
    } else {
        mData->mIconButton->setIcon( HbIcon( "" ) );
    }
}

/*!
 *
 */
QString RadioCarouselItem::parseFrequency( const uint frequency )
{
    QString loc = "%L1 MHz";// hbTrId( "txt_rad_list_l1_mhz_big" );
    return loc.arg( RadioStation::parseFrequency( frequency ) );
}

/*!
 *
 */
QString RadioCarouselItem::nameOrFrequency( const RadioStation& station, uint frequency )
{
    if ( frequency == 0 ) {
        frequency = station.frequency();
    }

    QString text = "";
    if ( station.isValid() && !station.name().isEmpty() ) {
        text = station.name();
    } else {
        text = parseFrequency( frequency );
    }

    return text;
}

/*!
 *
 */
RadioCarouselItem::Data::Data() :
    mLayout( NULL ),
    mSeekLayout( false )
{
}

/*!
 *
 */
RadioCarouselItem::Data::~Data()
{
}
