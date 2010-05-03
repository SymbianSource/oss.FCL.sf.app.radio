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

#ifndef _RADIOVIEWBASE_H_
#define _RADIOVIEWBASE_H_

// System includes
#include <HbView>

#include <QScopedPointer>

// User includes

// Forward declarations
class RadioMainWindow;
class RadioStationModel;
class RadioXmlUiLoader;
class HbAction;

// Constants
namespace MenuItem
{
    enum CommonMenuItem
    {
        UseLoudspeaker
    };
}

// Class declaration
class RadioViewBase : public HbView
{
    Q_OBJECT
    Q_DISABLE_COPY( RadioViewBase )

public:
    explicit RadioViewBase( bool transient = true );

    virtual ~RadioViewBase();

    virtual void init( RadioXmlUiLoader* uiLoader, RadioMainWindow* mainWindow );

    bool isTransient() const;

    void updateOrientation( Qt::Orientation orientation, bool forceUpdate = false );

protected slots:

    void updateAudioRouting( bool loudspeaker );
    void activatePreviousView();
    void quit();

protected:

// New functinos

    void initBackAction();
    
    void connectCommonMenuItem( int menuItem );

    void connectXmlElement( const char* name, const char* signal, QObject* receiver, const char* slot );

    void connectViewChangeMenuItem( QString name, const char* slot );

    void loadSection( const QString& docml, const QString& section );

private:

    virtual void setOrientation();

protected: // data

    /**
     * Pointer to the main window.
     * Not own.
     */
    RadioMainWindow*                    mMainWindow;

    /**
     * Pointer to the XML UI (DocML) loader
     * Own.
     */
    QScopedPointer<RadioXmlUiLoader>    mUiLoader;

    /**
     * Flag indicating whether or not the view is transient
     * Transient views are deleted when they are hidden.
     */
    bool                                mTransientView;

    /**
     * Route audio to Loudspeaker/Headset menu item
     */
    HbAction*                           mUseLoudspeakerAction;

    /**
     * View orientation.
     */
    Qt::Orientation                     mOrientation;

};


#endif // _RADIOVIEWBASE_H_