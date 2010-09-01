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
* Description:  Definitaion of the CFMRadioDocument class, which
*                      handles application data serialization. Owns the Ui 
*                      (controller) and the engine (model)
*
*/


#ifndef FMRADIODOCUMENT_H
#define FMRADIODOCUMENT_H

// INCLUDES
#include <AknDoc.h>
   
// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
* Part of the standard application framework. Declares the document class 
* for this application.
* @since 2.6
*/
class CFMRadioDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        * @since 2.6
        */
        static CFMRadioDocument* NewL( CEikApplication& aApp );
        /**
        * Destructor.
        * @since 2.6
        */
        virtual ~CFMRadioDocument();
    private: // New functions
        /**
        * EPOC default constructor.
        * @since 2.6
        */
        CFMRadioDocument( CEikApplication& aApp );
    private: // Functions from base classes
        /**
        * From CEikDocument, create CFMRadioAppUi "App UI" object.
        * @since 2.6
        */
        CEikAppUi* CreateAppUiL();
    };

#endif

// End of File

