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
* Description:  Definition of the class CFMRadioLogo.
*
*/


#ifndef FMRADIOALFLOGO_H
#define FMRADIOALFLOGO_H

// INCLUDES

#include <alf/alfcontrol.h>

#include "fmradioidlecontrolinterface.h"
// FORWARD DECLARATIONS

class CAlfImageVisual;
class MFMRadioLogoObserver;
class CAlfAnchorLayout;

// CLASS DEFINITION

/**
* Class for handling the radio icon
* 
*/ 
class CFMRadioLogo : public CAlfControl,
                        public MFMRadioIdleControlInterface
    {
    public:    // Constructors and destructor
        /**
        * Epoc two-phased constructor.
        * @param aEnv Reference to the Alf environment instance.
        * @return Pointer to this Indicator component.
        */
        static CFMRadioLogo* NewL( CAlfEnv& aEnv );
        
        /**
        * Destructor
        */
        virtual ~CFMRadioLogo();

    public:    // New functions
        /**
        * Sets the indicator opacity in visible state.
        * @param aOpacity The indicator opacity
        */
        void SetOpacityInVisibleState( const TReal aOpacity );
        
        /**
        * Sets the indicator opacity in hidden state.
        * @param aOpacity The indicator opacity
        */
        void SetOpacityInHiddenState( const TReal aOpacity );
        
        /** 
        * Sets the indicator rectangle
        * @param aRect Rectangle to the indicator
        */
        void SetRect( const TRect& aRect );
        
        /** 
        * Sets the Observer
        * @param aObserver The object to observe the control
        */
        void SetObserver( MFMRadioLogoObserver* aObserver );
        
        /** 
        * Removes the observer
        */
        void RemoveObserver( MFMRadioLogoObserver* aObserver );
        
        /** 
        * Switch to landscape image.
        * @param aShow. ETrue for landscape, EFalse for portrait 
        */        
        void SwitchToLandscapeImage( TBool aShow );
        
    private:
        /**
        * C++ default constructor
        */
        CFMRadioLogo ();
        
        /**
        * EPOC second phase constructor.
        * @param aEnv Reference to the Hitchcock environment instance.
        */
        void ConstructL( CAlfEnv& aEnv );
        
    private: // Functions from base classes
        /**
        * from MFMRadioIdleControlInterface
        */
        void Show();
        
        /**
        * from MFMRadioIdleControlInterface
        */
        void Hide();
        
        /**
        * from MFMRadioIdleControlInterface
        */        
        void Deactivate();

        /**
        * from CAlfControl
        */
        TBool OfferEventL( const TAlfEvent& aEvent );
        
    private:    // New functions
        /**
        * Sets the fading animation to the CAlfVisual object.
        * @param aVisual a pointer to the visual object
        * @param aFadingTime Time duration after the visual object has been faded. Fading duration in milliseconds. 
        * @param aOpacity Target opacity value
        */
        void Fade( CAlfVisual* aVisual, TInt aFadingTime, TReal aOpacity );
        
        /**
        * Sets absolute rect of the anchor by top left and bottom right points.
        * @param aAnchor Anchor layout for setting placement
        * @param aOrdinal Index of visual element
        * @param aTopLeftPosition Top left point of the rect
        * @param aBottomRightPosition Bottom right point of the rect
        */
        void SetAbsoluteCornerAnchors( CAlfAnchorLayout* aAnchor,
                                       TInt aOrdinal,
                                       const TPoint& aTopLeftPosition,
                                       const TPoint& aBottomRightPosition );
        /**
        * Creates the needed drawing layers and visual objects
        * for the logo.
        */
        void CreateImageVisualsL();
        
    private:
        /**
         * Alf custom events for the logo fade effects.
         * Logo stays visible couple of seconds after the
         * fade in event. 
         */
        enum TFadeCustomEvent
            {
            EFadeInCompleted,               // Fade in completed.
            ELogoDisplayTimerCompleted      // Visibility time ended.
            };

    private:// Data
        /** Anchor layout for the logo. Not owned */
        CAlfAnchorLayout* iLogoAnchor;
        /** logo opacity in visible state */
        TReal iOpacityInVisibleState;
        /** logo opacity in hidden state */
        TReal iOpacityInHiddenState;
        /** logo rectangle */
        TRect iRect;
        /** Array of observers. */
        RPointerArray<MFMRadioLogoObserver> iObservers;
        // visual containing the default radio icon. Not owned
        CAlfImageVisual* iImageVisual;
        // default radio icon in portrait mode
        TAlfImage iPortraitImage;
        // default radio icon in landscape mode
        TAlfImage iLandscapeImage;
        /** File name for the background bitmap. Owned. */
        HBufC* iBackgroundBitmapFileName;
        // flag for visibility status
        TBool iIsVisible;
    };

#endif //FMRADIOALFLOGO_H
