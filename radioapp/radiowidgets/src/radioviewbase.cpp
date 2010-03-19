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

#include <hbmenu.h>
#include <hbaction.h>
#include <hbeffect.h>
#include <qcoreapplication>

#include "radioviewbase.h"
#include "radiomainwindow.h"
#include "radiostationmodel.h"
#include "radioxmluiloader.h"
#include "radiouiengine.h"
#include "radiolocalization.h"
#include "radiologger.h"

/*!
 *
 */
RadioViewBase::RadioViewBase( RadioXmlUiLoader* uiLoader, bool transient ) :
    HbView( 0 ),
    mMainWindow( 0 ),
    mModel( 0 ),
    mUiLoader( uiLoader ),
    mTransientView( transient ),
    mUseLoudspeakerAction( 0 ),
    mSecondarySoftkeyction( 0 ),
    mOrientation( Qt::Vertical )
{
}

/*!
 *
 */
RadioViewBase::~RadioViewBase()
{
    if ( mUiLoader )
    {
        mUiLoader->reset();
    }
}

/*!
 *
 */
void RadioViewBase::init( RadioMainWindow* aMainWindow, RadioStationModel* aModel )
{
    // Default implementation does nothing
    Q_UNUSED( aMainWindow );
    Q_UNUSED( aModel );
}

/*!
 *
 */
bool RadioViewBase::isTransient() const
{
    return mTransientView;
}

/*!
 *
 */
void RadioViewBase::initSecondarySoftkey()
{
    // The default back button activates the tuning view
    mSecondarySoftkeyction = new HbAction( Hb::BackAction, this );
    connectAndTest( mSecondarySoftkeyction, SIGNAL(triggered()),
                    mMainWindow,            SLOT(activateTuningView()) );
}

/*!
 *
 */
HbAction* RadioViewBase::secondarySoftkey() const
{
    return mSecondarySoftkeyction;
}

/*!
 *
 */
void RadioViewBase::updateOrientation( Qt::Orientation orientation, bool forceUpdate )
{
    if ( orientation != mOrientation || forceUpdate )
    {
        mOrientation = orientation;
        setOrientation();
    }
}

/*!
 * Protected slot
 *
 */
void RadioViewBase::updateAudioRouting( bool loudspeaker )
{
    if ( mUseLoudspeakerAction ) {
        mUseLoudspeakerAction->setText( loudspeaker ? TRANSLATE( KMenuUseHeadset ) : TRANSLATE( KMenuUseLoudspeaker ) );
    }
}

/*!
 * Protected slot
 *
 */
void RadioViewBase::activatePreviousView()
{
    mMainWindow->activateTuningView();
}

/*!
 * Protected slot
 *
 */
void RadioViewBase::quit()
{
    qApp->quit();
}

/*!
 *
 */
HbAction* RadioViewBase::addMenuItem( const QString& aTitle, QObject* aRecipient, const char* aSlot )
{
    HbAction* action = menu()->addAction( aTitle );
    connectAndTest( action, SIGNAL(triggered()), aRecipient, aSlot );
    return action;
}

/*!
 *
 */
void RadioViewBase::connectCommonMenuItem( int menuItem )
{
    RadioUiEngine* engine = &mMainWindow->uiEngine();
    switch ( menuItem )
    {
        case MenuItem::UseLoudspeaker:
            mUseLoudspeakerAction = mUiLoader->findObject<HbAction>( DOCML_NAME_LOUDSPEAKERACTION );
            if ( mUseLoudspeakerAction ) {
                connectAndTest( mUseLoudspeakerAction, SIGNAL(triggered()), engine, SLOT(toggleAudioRoute()) );
                updateAudioRouting( engine->isUsingLoudspeaker() );
                connectAndTest( engine, SIGNAL(audioRouteChanged(bool)), this, SLOT(updateAudioRouting(bool)) );
            }
            break;

        default:
            break;
    }
}

/*!
 *
 */
void RadioViewBase::connectXmlElement( const char* name, const char* signal, QObject* receiver, const char* slot )
{
    if ( QObject* action = mUiLoader->findObject<QObject>( name ) )
    {
        connectAndTest( action, signal, receiver, slot );
    }
}

/*!
 *
 */
void RadioViewBase::connectViewChangeMenuItem( QString name, const char* slot )
{
    if ( QObject* action = mUiLoader->findObject<QObject>( name ) )
    {
        connectAndTest( action, SIGNAL(triggered()), mMainWindow, slot );
    }
}

/*!
 *
 */
void RadioViewBase::setOrientation()
{
    // Default implementation does nothing
}
