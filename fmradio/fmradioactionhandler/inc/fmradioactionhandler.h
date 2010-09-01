/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration for the class CFMRadioActionHandler.
*
*/

#ifndef FMRADIOACTIONHANDLER_H
#define FMRADIOACTIONHANDLER_H

#include <ahplugin.h>

/**
 * Action handler provides LIW -based interface for external use (e.g. Homescreen) 
 * 
 * The action handler communicates with FM Radio via FM Radio's Publish & Subscribe API.
 * 
 * @lib fmradioactionhandler.lib
 */
NONSHARABLE_CLASS( CFMRadioActionHandler ) : public CAHPlugin
    {
public:
    /**
     * Two-phased constructor. 
     */
    static CFMRadioActionHandler* NewL();

    /**
     * Destructor
     */
    ~CFMRadioActionHandler();
    
    // From base class CAHPlugin
    TInt ExecuteActionL( const CLiwMap* aMap );
    
private:
    /**
     * C++ constructor.
     */
    CFMRadioActionHandler();
    
    /**
     * Second phase constructor.
     */
    void ConstructL();
    
private:
    /**
     * Invokes step to previous preset.
     */
    void StepPrevious();
    
    /**
     * Invokes step to next preset. 
     */
    void StepNext();
    
    /**
     * Mutes radio.
     */
    void Mute();
    
    /**
     * Unmutes radio.
     */
    void Unmute();
    
    /**
     * Invokes seek up.
     */
    void SeekUp();
    
    /**
     * Invokes seek down.
     */
    void SeekDown();

    /**
     * Invokes to go to now playing view.
     */
    void StartToNowPlayingL();
    
    /**
     * Generates start now playing command tail.
     * 
     * @return start now playing command tail. 
     */
    const TPtrC8 GenerateStartNowPlayingCommandTail();
    
    /**
     * Extracts the value of the given mapname to given string.
     * Reserves memory to argument aString.
     * 
     * @param aMap The map containing the mapname-string pairs.
     * @param aString Descriptor where the value is extracted.
     * @param aMapName The searched map name. 
     */
    TInt ExtractStringL( const CLiwMap* aMap, RBuf& aString, const TDesC8& aMapName );
    };

#endif /* FMRADIOACTIONHANDLER_H*/
