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

#ifndef RADIOSTATIONCONTROLWIDGET_H
#define RADIOSTATIONCONTROLWIDGET_H

// System includes
#include <hbwidget.h>
#include <hbicon.h>
#include <hbeffect.h>

// Forward declarations
class RadioUiEngine;
class HbPushButton;
class RadioXmlUiLoader;
class RadioMainWindow;

// Class declaration
class RadioStationControlWidget : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY(HbIcon background READ background WRITE setBackground)
    Q_PROPERTY(QString tagBtnText READ tagBtnText WRITE setTagBtnText)
    Q_PROPERTY(QString recognizeBtnText READ recognizeBtnText WRITE setRecognizeBtnText)
    Q_PROPERTY(QString stationsBtnText READ stationsBtnText WRITE setStationsBtnText)

public:

    RadioStationControlWidget( RadioUiEngine& uiEngine, QGraphicsItem* parent = 0 );
    ~RadioStationControlWidget();

    void setBackground( const HbIcon& background );
    HbIcon background() const;
    
    void setTagBtnText( const QString& text );
    QString tagBtnText() const;

    void setRecognizeBtnText( const QString& text );
    QString recognizeBtnText() const;

    void setStationsBtnText( const QString& text );
    QString stationsBtnText() const;

    void init( RadioMainWindow* aMainWindow );

private slots:

    void updateStationsButton();
    void recognizePressed();
    void effectFinished( HbEffect::EffectStatus status );
    void disableTagButton();
    void enableTagButton();

private:

// from base class QGraphicsItem

    void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );
    
// New functions

private: // data
    
    RadioUiEngine&      mUiEngine;

    HbIcon              mBackground;
    HbPushButton*       mTagSongButton;
    HbPushButton*       mRecognizeButton;
    HbPushButton*       mStationsViewButton;

    RadioMainWindow*    mMainWindow;

};

#endif // RADIOSTATIONCONTROLWIDGET_H
