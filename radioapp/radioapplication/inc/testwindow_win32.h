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

#ifndef WIN32_WINDOW_H
#define WIN32_WINDOW_H

// System includes
#include <qwidget>

// Forward declarations
class HbMainWindow;
class QPushButton;
class QGridLayout;

class Win32Window : public QWidget
{
    Q_OBJECT
public:

    Win32Window();
    ~Win32Window();

    void addHbWindow( HbMainWindow* radioWindow );

    void init();

private slots:

    void changeOrientation();
    void volumeUp();
    void volumeDown();
    void toggleHeadsetStatus();
    void updateWindowSize();
    void addSong();
    void clearSong();
    void toggleOffline();

private:

    void resizeEvent( QResizeEvent* event );

private: // data

    QPushButton*    mOrientationButton;

    QPushButton*    mVolUpButton;
    QPushButton*    mVolDownButton;

    QPushButton*    mAddSongButton;
    QPushButton*    mClearSongButton;

    QPushButton*    mHeadsetButton;
    bool            mHeadsetConnected;

    QPushButton*    mOfflineButton;

    QGridLayout*    mToolbarLayout;

    int             mVolume;

    int             mToolbarHeight;

    HbMainWindow*   mRadioWindow;

    Qt::Orientation mOrientation;

    int             mSongIndex;

};

#endif // WIN32_WINDOW_H
