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
#include <qpushbutton>
#include <qgridlayout>
#include <qvboxlayout>
#include <hbmainwindow.h>
#include <qtimer.h>
#include <qmessagebox>

// User includes
#include "testwindow_win32.h"
#include "radioapplication.h"
#include "radiologger.h"
#include "radio_global.h"
#include "radioenginewrapper_win32_p.h"

const int KWindowWidth = 360;
const int KWindowHeight = 640;

const QString KBtnDisconnectHeadset = "Disconnect Headset";
const QString KBtnConnectHeadset = "Connect Headset";

const QString KBtnGoOffline = "Go Offline";
const QString KBtnGoOnline = "Go Online";

struct Song
{
    const char* mArtist;
    const char* mTitle;
};

const Song KRecognizedSongs[] = {
    { "Metallica", "One" },
    { "Madonna", "Like a Virgin" },
    { "Rammstein", "Der Meister" },
    { "System of a Down", "Aerials" },
    { "Buckethead", "Jordan" },
    { "Faith No More", "Epic" }
};
const int KSongsCount = sizeof( KRecognizedSongs ) / sizeof( KRecognizedSongs[0] );

/*!
 *
 */
Win32Window::Win32Window() :
    QWidget( 0 ),
    mOrientationButton( new QPushButton( "Change Orientation", this ) ),
    mVolUpButton( new QPushButton( "Volume Up", this ) ),
    mVolDownButton( new QPushButton( "Volume Down", this ) ),
    mHeadsetButton( new QPushButton( KBtnDisconnectHeadset, this ) ),
    mAddSongButton( new QPushButton( "Add Song", this ) ),
    mClearSongButton( new QPushButton( "Clear Song", this ) ),
    mOfflineButton( new QPushButton( KBtnGoOffline, this ) ),
    mHeadsetConnected( true ),
    mVolume( 5 ),
    mToolbarHeight( 0 ),
    mRadioWindow( 0 ),
    mSongIndex( 0 )
{
    connectAndTest( mOrientationButton, SIGNAL(clicked()), this, SLOT(changeOrientation()) );
    connectAndTest( mVolUpButton, SIGNAL(clicked()), this, SLOT(volumeUp()) );
    connectAndTest( mVolDownButton, SIGNAL(clicked()), this, SLOT(volumeDown()) );
    connectAndTest( mHeadsetButton, SIGNAL(clicked()), this, SLOT(toggleHeadsetStatus()) );
    connectAndTest( mAddSongButton, SIGNAL(clicked()), this, SLOT(addSong()) );
    connectAndTest( mClearSongButton, SIGNAL(clicked()), this, SLOT(clearSong()) );
    connectAndTest( mOfflineButton, SIGNAL(clicked()), this, SLOT(toggleOffline()) );

    QTimer::singleShot( 0, this, SLOT(updateWindowSize()) );
}

/*!
 *
 */
Win32Window::~Win32Window()
{
}

/*!
 *
 */
void Win32Window::addHbWindow( HbMainWindow* radioWindow )
{
    mRadioWindow = radioWindow;
    mOrientation = mRadioWindow->orientation();
    updateWindowSize();

    QVBoxLayout* layout = new QVBoxLayout( this );
    layout->setMargin( 5 );
    layout->setSpacing( 5 );

    mToolbarLayout = new QGridLayout( this );
    mToolbarLayout->setHorizontalSpacing( 5 );
    mToolbarLayout->setVerticalSpacing( 5 );

    mToolbarLayout->addWidget( mOrientationButton, 0, 0 );
    mToolbarLayout->addWidget( mVolUpButton, 0, 1 );
    mToolbarLayout->addWidget( mVolDownButton, 1, 1 );
    mToolbarLayout->addWidget( mHeadsetButton, 1, 0 );
    mToolbarLayout->addWidget( mAddSongButton, 2, 0 );
    mToolbarLayout->addWidget( mClearSongButton, 2, 1 );
    mToolbarLayout->addWidget( mOfflineButton, 3, 0 );

    layout->addItem( mToolbarLayout );
    layout->addWidget( radioWindow );

    setLayout( layout );
}

/*!
 *
 */
void Win32Window::init()
{
    RadioEngineWrapperPrivate* wrapper = RadioEngineWrapperPrivate::instance();
    if ( wrapper ) {
        QString error = wrapper->dataParsingError();
        if ( !error.isEmpty() ) {
            QMessageBox msg( QMessageBox::Warning, "Unable to parse radio settings", error, QMessageBox::Ok );
            msg.exec();
        }

        if ( wrapper->isOffline() ) {
            mOfflineButton->setText( KBtnGoOnline );
        }
    }
}

/*!
 * Private slot
 */
void Win32Window::changeOrientation()
{
    if ( mOrientation == Qt::Horizontal ) {
        mOrientation = Qt::Vertical;
    } else {
        mOrientation = Qt::Horizontal;
    }

    mRadioWindow->setOrientation( mOrientation );
    updateWindowSize();
}

/*!
 * Private slot
 */
void Win32Window::volumeUp()
{
    if ( ++mVolume > KMaximumVolumeLevel ) {
        mVolume = KMaximumVolumeLevel;
    }
    RadioEngineWrapperPrivate::instance()->setVolume( mVolume );
}

/*!
 * Private slot
 */
void Win32Window::volumeDown()
{
    if ( --mVolume < 0 ) {
        mVolume = 0;
    }
    RadioEngineWrapperPrivate::instance()->setVolume( mVolume );
}

/*!
 * Private slot
 */
void Win32Window::toggleHeadsetStatus()
{
    mHeadsetConnected = !mHeadsetConnected;
    if ( mHeadsetConnected ) {
        mHeadsetButton->setText( KBtnDisconnectHeadset );
    } else {
        mHeadsetButton->setText( KBtnConnectHeadset );
    }
    RadioEngineWrapperPrivate::instance()->setHeadsetStatus( mHeadsetConnected );
}

/*!
 * Private slot
 */
void Win32Window::updateWindowSize()
{
    if ( mOrientation == Qt::Horizontal ) {
        resize( KWindowHeight, KWindowWidth + mToolbarHeight );
    } else {
        resize( KWindowWidth, KWindowHeight + mToolbarHeight );
    }
}

/*!
 * Private slot
 */
void Win32Window::addSong()
{
    Song song = KRecognizedSongs[mSongIndex++];
    mSongIndex %= KSongsCount;

    RadioEngineWrapperPrivate::instance()->addSong( song.mArtist, song.mTitle );
}

/*!
 * Private slot
 */
void Win32Window::clearSong()
{
    RadioEngineWrapperPrivate::instance()->clearSong();
}

/*!
 * Private slot
 */
void Win32Window::toggleOffline()
{
    bool offline = !RadioEngineWrapperPrivate::instance()->isOffline();
    RadioEngineWrapperPrivate::instance()->setOffline( offline );
    if ( offline ) {
        mOfflineButton->setText( KBtnGoOnline );
    } else {
        mOfflineButton->setText( KBtnGoOffline );
    }
}

/*!
 *
 */
void Win32Window::resizeEvent( QResizeEvent* event )
{
    QWidget::resizeEvent( event );
    mToolbarHeight = mToolbarLayout->contentsRect().height();
}
