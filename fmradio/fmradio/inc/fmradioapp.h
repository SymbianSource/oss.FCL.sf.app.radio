/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main application class definitions.
*
*/

/*! \mainpage
*
* @section intro Introduction
*
* @subsection abstract Abstract
*
* Application provides FM radio functionality for
* finding, saving and playing radio stations. 
* 
* @subsection architecture Basic architecture
*
* FMRadio constructs from the following program modules:
* application UI, application engine, plugin extensions and interfaces to external components.
** 
* @subsection containedcomponents Contained Components/Executables
*
* - fmradio.exe / UI application
* - fmradioengine.dll / Application engine
* - fmradioactiveidleengine200.dll / Active idle engine. Notify observers about P&S changes
* - fmradioactionhandler.dll / Action handler plugin. Handles P&S commands
* - fmradiomcpplugin100.dll / Content publisher plugin. Publishes radio data to other components
*
* @subsection providedapis Provided APIs
*
* - FM Radio command line parameters.
* - Publish & Subscribe interface.
* 
* @section usecases Use cases
*
* \image html fmradio_use_case.png "Use cases"
*

*
*/

#ifndef FMRADIOAPP_H
#define FMRADIOAPP_H

// INCLUDES
#include <aknapp.h>

// CONSTANTS

// CLASS DECLARATION

/**
* CFMRadioApp application class.
* This class is a part of the standard application framework. The application 
* gets instantiated by this class. Provides a factory method for 
* instantiating the document object.
*
* @since 2.6
*/
class CFMRadioApp : public CAknApplication
    {
    
    private:
        /**
        * From CApaApplication, creates CFMRadioDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        /**
        * From CApaApplication, returns application's UID (KUidFMRadio).
        * @return The value of KUidFMRadio.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

