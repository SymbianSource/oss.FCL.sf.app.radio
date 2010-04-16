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

// User includes
#include "radiostationitem.h"
#include "radiostationcarousel.h"
#include "radiouiengine.h"
#include "radiofadinglabel.h"
#include "radiostationmodel.h"
#include "radiolocalization.h"
#include "radiologger.h"

//static const char* FILE_PATH_WIDGETML   = ":/layout/radiostationitem.widgetml";
//static const char* FILE_PATH_CSS        = ":/layout/radiostationitem.css";
//static const char* GENRE_LABEL          = "genre_label";
//static const char* NAME_LABEL           = "name_label";
//static const char* RADIOTEXT_LABEL      = "radiotext_label";
//static const char* URL_LABEL            = "url_label";
//static const char* FAVORITE_BUTTON      = "favorite_button";

const char* SEEKING_TEXT = "txt_rad_list_tuning";
const char* CONNECT_HEADSET_TEXT = "txt_rad_list_connect_wireless_antenna_headset_with";

/*!
 *
 */
RadioStationItem::RadioStationItem( RadioStationCarousel& carousel ) :
    HbAbstractViewItem( &carousel ),
    mCarousel( carousel ),
    mLayout( 0 ),
    mNameLabel( 0 ),
    mIconButton( 0 ),
    mGenreLabel( 0 ),
    mRadiotextLabel( 0 ),
    mUrlLabel( 0 )
{
    setFlag( QGraphicsItem::ItemIsFocusable, true );
}

/*!
 * From HbAbstractViewItem
 *
 */
HbAbstractViewItem* RadioStationItem::createItem()
{
    RadioStationItem* item = new RadioStationItem( *this ); // Calls copy constructor
//    item->setParent( &mCarousel );
//    item->setParentItem( &mCarousel );
//    item->setItemView( itemView() );
//    item->updateChildItems();
    return item;
}

/*!
 * From HbAbstractViewItem
 */
void RadioStationItem::updateChildItems()
{
    if ( !mLayout )
    {
        mIconButton = new HbPushButton( this );
        QPixmap background( QSize( 50, 50 ) );
        background.fill( Qt::transparent );
        mIconButton->setBackground( HbIcon( background ) );
        mIconButton->setOrientation( Qt::Horizontal );
        mIconButton->setIcon( mCarousel.nonFavoriteIcon() );
        mIconButton->setMaximumSize( 50, 50 );
        connectAndTest( mIconButton, SIGNAL(clicked()), this, SLOT(toggleFavorite()));

        mNameLabel = new RadioFadingLabel( this );
        mNameLabel->setAlignment( Qt::AlignCenter );
        HbFontSpec spec = mNameLabel->fontSpec();
        spec.setRole( HbFontSpec::Primary );
        mNameLabel->setFontSpec( spec );

        spec.setRole( HbFontSpec::Secondary );

        mGenreLabel = new RadioFadingLabel( this );
        mGenreLabel->setAlignment( Qt::AlignCenter );
        mGenreLabel->setTextColor( Qt::white );

        mRadiotextLabel = new RadioFadingLabel( this );
        mRadiotextLabel->setAlignment( Qt::AlignCenter );
        mRadiotextLabel->setTextWrapping( Hb::TextWordWrap );
//        mRadiotextLabel->setFadingEnabled( true );    TODO
//        mRadiotextLabel->setFontSpec( spec );
        mRadiotextLabel->setTextColor( Qt::white );

        mUrlLabel = new RadioFadingLabel( this );
        mUrlLabel->setAlignment( Qt::AlignCenter );
        mUrlLabel->setTextColor( Qt::white );

        mLayout = new HbAnchorLayout();

        mLayout->setAnchor( mLayout, Hb::TopEdge, mIconButton, Hb::TopEdge, 20.0 );
        mLayout->setAnchor( mLayout, Hb::LeftEdge, mIconButton, Hb::LeftEdge, 20.0 );

        mLayout->setAnchor( mIconButton, Hb::CenterVEdge, mGenreLabel, Hb::CenterVEdge, 0.0 );
        mLayout->setAnchor( mIconButton, Hb::RightEdge, mGenreLabel, Hb::LeftEdge, 20.0 );
        mLayout->setAnchor( mLayout, Hb::RightEdge, mGenreLabel, Hb::RightEdge, -70.0 );

        mLayout->setAnchor( mGenreLabel, Hb::BottomEdge, mNameLabel, Hb::TopEdge, 0.0 );
        mLayout->setAnchor( mLayout, Hb::LeftEdge, mNameLabel, Hb::LeftEdge, 10.0 );
        mLayout->setAnchor( mLayout, Hb::RightEdge, mNameLabel, Hb::RightEdge, -10.0 );

        mLayout->setAnchor( mNameLabel, Hb::BottomEdge, mRadiotextLabel, Hb::TopEdge, 10.0 );
        mLayout->setAnchor( mLayout, Hb::LeftEdge, mRadiotextLabel, Hb::LeftEdge, 10.0 );
        mLayout->setAnchor( mLayout, Hb::RightEdge, mRadiotextLabel, Hb::RightEdge, -10.0 );

        mLayout->setAnchor( mRadiotextLabel, Hb::BottomEdge, mUrlLabel, Hb::TopEdge, 10.0 );
        mLayout->setAnchor( mLayout, Hb::LeftEdge, mUrlLabel, Hb::LeftEdge, 10.0 );
        mLayout->setAnchor( mLayout, Hb::RightEdge, mUrlLabel, Hb::RightEdge, -10.0 );
        mLayout->setAnchor( mLayout, Hb::BottomEdge, mUrlLabel, Hb::BottomEdge, -10.0 );

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
    if ( !( station && station->isValid() ) && !index.isValid() ) {
        return;
    }

    RadioStation tempStation = ( station && station->isValid() ) ? *station
                    : index.data( RadioStationModel::RadioStationRole ).value<RadioStation>();

    mNameLabel->setTextWithoutFading( RadioUiEngine::nameOrFrequency( tempStation ) );
    mGenreLabel->setText( carousel()->uiEngine().genreToString( tempStation.genre() ) );
    if ( !carousel()->isAntennaAttached() ) {
        mRadiotextLabel->setText( hbTrId( CONNECT_HEADSET_TEXT ) );
    } else {
        if ( !tempStation.radioText().isEmpty() ) {
            mRadiotextLabel->setText( tempStation.radioText() );
        } else if ( !tempStation.dynamicPsText().isEmpty() ) {
            mRadiotextLabel->setText( tempStation.dynamicPsText() );
        } else {
            mRadiotextLabel->setText( "" );
        }
    }

    mUrlLabel->setText( tempStation.url() );

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
    mRadiotextLabel->setTextWithoutFading( carousel()->isAntennaAttached() ? "" : hbTrId( CONNECT_HEADSET_TEXT ) );
    mUrlLabel->setTextWithoutFading( "" );
    mFrequency = frequency;
    updateFavoriteIcon( false );
}

/*!
 *
 */
void RadioStationItem::setSeekingText()
{
    mNameLabel->setTextWithoutFading( hbTrId( SEEKING_TEXT ) );
    mGenreLabel->setTextWithoutFading( "" );
    mUrlLabel->setTextWithoutFading( "" );
    mRadiotextLabel->setTextWithoutFading( "" );
}

/*!
 *
 */
void RadioStationItem::updateFavoriteIcon( bool isFavorite )
{
    mIconButton->setIcon( isFavorite ? mCarousel.favoriteIcon() : mCarousel.nonFavoriteIcon() );
}

/*!
 *
 */
RadioStationCarousel* RadioStationItem::carousel()
{
    return static_cast<RadioStationCarousel*>( itemView() );
}

