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

#ifndef _RADIOWINDOW_H_
#define _RADIOWINDOW_H_

// System includes
#include <HbMainWindow>
#include <HbEffect>
#include <QPointer>
#include <QScopedPointer>
#include <QSharedPointer>
#include <qsysteminfo.h>
using namespace QtMobility; // has to be here to be able to connect SLOT queryOfflineUsage

// User includes
#include "radiowidgetsexport.h"

// Forward declarations
class RadioViewBase;
class RadioUiEngine;
class HbVolumeSliderPopup;
class HbMessageBox;

typedef QScopedPointer<HbVolumeSliderPopup> VolumeSliderPtr;

/**
 * QPointer is used to store the views because it tracks the deletion of the object and nulls
 * the reference. Transient view like RadioHistoryView is destroyed after they are closed
 * and QPointer will notice it.
 */
typedef QPointer<RadioViewBase> ViewPtr;

// Class declaration
class WIDGETS_DLL_EXPORT RadioWindow : public HbMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY( RadioWindow )

public:

    RadioWindow( QWidget* parent = 0 );

    ~RadioWindow();

    void showErrorMessage( const QString& text );

    void init( QSystemDeviceInfo* deviceInfo );

    QString orientationSection();

public slots:

    void activateMainView();

    void activateStationsView();

    void activateHistoryView();

private slots:

    void initView();
    void updateOrientation( Qt::Orientation orientation );
    void showVolumeLevel( int volume );
    void updateAntennaStatus( bool connected );
    void queryOfflineUsage( QSystemDeviceInfo::Profile profile );

private:

// New functions

    void activateView( RadioViewBase* aMember, const QString& docmlFile, Hb::ViewSwitchFlags flags = Hb::ViewSwitchDefault );

private: // data

    /*!
     * Pointer to the UI engine
     * Own, shared with views
     */
    QSharedPointer<RadioUiEngine>       mUiEngine;

    /**
     * Tuning view.
     * Own.
     */
    ViewPtr                             mMainView;

    /**
     * Stations view
     * Own.
     */
    ViewPtr                             mStationsView;

    /**
     * Play history view
     * Own.
     */
    ViewPtr                             mHistoryView;

    /**
     * Pointer to the volume slider
     * Own.
     */
    VolumeSliderPtr                     mVolSlider;

    /**
     * Pointer to messagebox
     * Own.
     */
    QScopedPointer<HbMessageBox>        mMessageBox;

    /**
     * Pointer to qsystemdeviceinfo
     * Own.
     */
    QScopedPointer<QSystemDeviceInfo>   mDeviceInfo;

};


#endif // _RADIOWINDOW_H_
