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
* Description:  Definition of the class CFMRadioAlfVisualizer.
*
*/


#ifndef FMRADIOALFVISUALIZER_H
#define FMRADIOALFVISUALIZER_H

// INCLUDES

#include <AknUtils.h>
#include <alf/alfcontrol.h>
#include <alf/alfanchorlayout.h>
#include <gesturecontrol.h>
#include <gestureobserver.h>

#include "fmradiodefines.h"
#include "fmradio.hrh"

// CONSTANTS
        
/** Scroll directions of the visual objects. */
enum TScrollDirection
    {    
    EScrollLeftToRight = 0xA0,
    EScrollRightToLeft,
    EScrollNone
    };
        
/** Possible display styles. */
enum TFMRadioDisplayStyle
    {
    EFMRadioDisplaySingleLine = 0,  /**< Single line display. */
    EFMRadioDisplayDoubleLine,       /**< Double line display. */
    EFMRadioDisplayTuning,           /**< Tuning display. */
    EFMRadioDisplayNoDisplay         /**< No display used. */
    };
        
/** Display orientations. */
enum TDisplayOrientation
    {
    EPortrait = 0x4A, 
    ELandscape,
    EOrientationNone
    };

// FORWARD DECLARATIONS

class CAlfTextVisual;
class CAlfViewportLayout;
class CAlfGradientBrush;
class CFMRadioAlfBitmapAnimation;
class CFMRadioAppUi;
class MTouchFeedback;

using namespace GestureHelper;
// CLASS DEFINITION

/**
* CFMRadioAlfVisualizer class
* 
* Visualizes informations of the FMRadio to the display by Hitchcock UI Toolkit
*/ 
NONSHARABLE_CLASS(CFMRadioAlfVisualizer) : public CGestureControl, public MGestureObserver
    {
public:        // Constructors and destructor

        /**
         * Epoc two-phased constructor.
         * @param aEnv Reference to the HitchCock environment instance.
         * @return Pointer to this Visualizer component.
         */
        static CFMRadioAlfVisualizer* NewL( CAlfEnv& aEnv );
        /**
         * Destructor
         */
        virtual ~CFMRadioAlfVisualizer();
public:     // Functions from base classes        
        /**
          * From CGestureControl/CAlfControl, takes care of alfred event handling.
          * @param aEvent
          */
        TBool OfferEventL( const TAlfEvent& aEvent );    

        /**
         * From CGestureControl, takes care of gesture event handling.
         * @param aEvent
         */
        void HandleGestureL( const MGestureEvent& aEvent );
public:        // New functions               
        /**
         * Changes station to next with scrolling and fading station's 
         * information (name, frequency, etc.).
         * @param aChangeType The change type
         * @param aFirstLine The text shown in the first line
         * @param aSecondLine The text shown in the second line, or NULL
         */
        void ChangeStationL( TFMRadioStationChangeType aChangeType, 
                             const TDesC& aFirstLine, 
                             const TDesC* aSecondLine = NULL );
        /**
         * Scrolls out the current station information lines by 
         * hiding actual visuals and scrolling out the created temporary information lines.
         * @param aChangeType Station change type
         */
        void ScrollOutCurrentStationInformationL( TFMRadioStationChangeType aChangeType );
        /**
         * Resets the station information and sets static text to the third line
         * @param aInfoText an information text to show
         * @param aChangeType The change type
         */
        void SetStaticInfoTextL( TFMRadioStationChangeType aChangeType, const TDesC& aInfoText );
        /**
         * Change text color of the text visuals
         * @param aTextColor Color of the visual texts
         */        
        void SetStationInformationTextColor( const TRgb& aTextColor );
        /**
         * Sets the display style in use.
         * @param   aDisplayStyle Display style to use.
         */
        void SetDisplayStyle( TFMRadioDisplayStyle aDisplayStyle );        
        /**
         * Returns the display style.
         * @return  Display style.
         */
        TFMRadioDisplayStyle DisplayStyle() const;        
        /** 
         * Calculates display layout
         */            
        void UpdateLayout();            
        /**
         * Return orientation of the display
         */
        TDisplayOrientation Orientation();
        /**
         * Set orientation of the display
         * @param aOrientation Orientation of the display
         */
        void SetOrientation( TDisplayOrientation aOrientation );
        /**
         * Set mirrored layout state
         * @param aIsMirrored state
         */
        void SetMirrored( const TBool aIsMirrored );
        /**
         * @return Mirrored layout state
         */
        TBool IsMirrored() const;    
        
        /**
         * Starts tuning animation after delay
         */        
        void StartTuningAnimation();
        
        /**
         * Stops tuning animation by fading it away
         */            
        void StopTuningAnimation();
private:
        /**
         * C++ default constructor
         */
        CFMRadioAlfVisualizer( );
        /**
         * EPOC second phase constructor.
         * @param aEnv Reference to the Hitchcock environment instance.
         */
        void ConstructL( CAlfEnv& aEnv );
private:    // New functions
        
        /** The visuals of the station information display. */
        enum TStationInformationVisual
            {
            EFirstLineTextVisual = 0x1,  /**< Text visual of the first line. */
            ESecondLineTextVisual = 0x2, /**< Text visual of the second line. */
            ETuningTextVisual = 0x4,     /* Tuning text visual. */
            EAllVisuals = EFirstLineTextVisual |
                          ESecondLineTextVisual |
                          ETuningTextVisual
            };
        
        /**
         * Specifies the axis that is currently used in the station info drag
         */
        enum TFMRadioDragAxis
            {
            EFMRadioDragAxisNone = 0,
            EFMRadioDragAxisX,
            EFMRadioDragAxisY
            };
        
        /**
         * Sets text to the defined station information visual. 
         * @param  aVisual     The text visual object for showing
         * @param  aText      Text   
         */
        void SetVisualTextL( TStationInformationVisual aVisual, const TDesC& aText );    
        /**
         * Shows the defined station information visual. 
         * @param  aVisual     The text visual object for showing
         * @param  aFade     <code>ETrue</code> if fade effect it to be used, <code>EFalse</code> otherwise.
         */
        void ShowVisual( TStationInformationVisual aVisual, TBool aFade );
        /**
         * Hides the defined station information visual. 
         * @param aVisual The text visual object for hiding
         */
        void HideVisual( TStationInformationVisual aVisual );    
        /**
         * Sets and starts scrolling and fading animations to CAlfTextVisual.
         * The visual object is scrolled from right side of the display to 
         * original position of the visual object.
         * @param aVisual a pointer to the visual object
         */
        void ScrollRightToLeftWithFadeIn( CAlfVisual& aVisual ) const;
        /**
         * Sets and starts scrolling and fading animations to CAlfTextVisual.
         * The visual object is scrolled from left side of the display to 
         * original position of the visual object.
         * @param aVisual a pointer to the visual object
         */
        void ScrollLeftToRightWithFadeIn( CAlfVisual& aVisual ) const;
        /**
         * Sets and starts scrolling and fading animations to CAlfTextVisual.
         * The visual object is scrolled from top of the display to 
         * original position of the visual object.
         * @param aVisual a pointer to the visual object
         */
        void ScrollInFromTop( CAlfVisual& aVisual ) const;
        /**
         * Sets and starts scrolling and fading animations to CAlfTextVisual.
         * The visual object is scrolled from bottom of the display to 
         * original position of the visual object.
         * @param aVisual a pointer to the visual object
         */
        void ScrollInFromBottom( CAlfVisual& aVisual ) const;
        /**
         * Moves the visual back to it's original position
         * @param aVisual The visual to be moved
         */
        void RevertVisual( CAlfVisual& aVisual );        
        /**
          * Sets and starts scrolling animation to CAlfVisual.
          * The visual object is scrolled from current position to 
          * right side of the display
          * @param aVisual a reference to the visual object
          * @param aTime Time duration after the visual object has been scrolled to the target. 
          * Scrolling duration in milliseconds.
          */
        void ScrollToRight( CAlfVisual& aVisual, TInt aTime ) const;
        /**
          * Sets and starts scrolling animation to CAlfVisual.
          * The visual object is scrolled from current position to 
          * left side of the display
          * @param aVisual a reference to the visual object
          * @param aTime Time duration after the visual object has been scrolled to the target. 
          * Scrolling duration in milliseconds.
          */
        void ScrollToLeft( CAlfVisual& aVisual, TInt aTime ) const;
        /**
         * Sets and starts scrolling animation to CAlfVisual.
         * The visual object is scrolled from current position above the the display
         * @param aVisual a reference to the visual object
         * @param aTime Time duration after the visual object has been scrolled to the target. 
         * Scrolling duration in milliseconds.
         */
        void ScrollUp( CAlfVisual& aVisual, TInt aTime ) const;
        /**
         * Sets and starts scrolling animation to CAlfVisual.
         * The visual object is scrolled from current position below the the display
         * @param aVisual a reference to the visual object
         * @param aTime Time duration after the visual object has been scrolled to the target. 
         * Scrolling duration in milliseconds.
         */
        void ScrollDown( CAlfVisual& aVisual, TInt aTime ) const;
        /**
         * Sets fade-in animation to the CAlfVisual.
         * @param aVisual a reference to the visual object
         * @param aFadingTime Time duration after the visual object has been faded in.
         *                    Fading duration in milliseconds
         * @param aOpacity Target opacity value
         */
        void FadeIn( CAlfVisual& aVisual, TInt aFadingTime, TReal aOpacity = 1.0f ) const;
        /**
         * Sets fade-out animation to the CAlfVisual.
         * @param aVisual a reference to the visual object
         * @param aFadingTime Time duration after the visual object has been faded out.
         *                    Fading duration in milliseconds
         * @param aOpacity Target opacity value
         */
        void FadeOut( CAlfVisual& aVisual, TInt aFadingTime, TReal aOpacity = 0.0f ) const;
        /**
          * Translates the visual object with passed arguments
          * @param aTextVisual a pointer to the visual object 
          * @param aX Translation value in the horizontal direction
          * @param aY Translation value in the vertical direction
          */
        void Translate( CAlfTextVisual* aTextVisual, const TAlfTimedValue& aX, const TAlfTimedValue& aY );    
        /**
         * Removes all transformations of the visual object such as the scaling and translating.
         * @param aTextVisual a pointer to the visual object 
         */
        void LoadTextVisualIdentity( CAlfTextVisual* aTextVisual );
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
          * for visualiazing the station information.
          */
        void AddInformationLayersL();
        /**
          * Creates the text styles to be used by station information visualiazing
          */
        void CreateTextStylesForStationInformationL();
        /**
         * Creates copy of current station information
         */
        void CreateTemporaryStationInformationVisualsL();
        /**
         * Animates temporary station infoout of the display
         * @param aChangeType Change type
         */
        void AnimateTemporaryStationInformationVisuals( TFMRadioStationChangeType aChangeType );    
        /**
         * Triggers the command to view handling
         */
        void TriggerCommandL();
        /**
         * Static call back for the long key press timer.
         * @param   aSelf   Pointer to self.
         * @return  KErrNone
         */
        static TInt StaticLongPressCallBack( TAny* aSelf );
private:    // Data
            
        /** Layout for the station information */
        TAknLayoutRect iStationInformationLayout; 
        /** Layout for the transparent background layer of the station information */
        TAknLayoutRect iTransparentBackgroundLayout;
        
        /** Layout for the first line. */
        TAknLayoutText iFirstLineLayout;
        /** Layout for the second line */
        TAknLayoutText iSecondLineLayout;
        /** Layout for the tuning text. */
        TAknLayoutText iTuningTextLayout;
        /** Layout for the tuning animation. */
        TAknLayoutRect iTuningAnimationLayout;
                
        /** Visual object of the first line. Not owned. */
        CAlfTextVisual* iFirstLineVisualText;  
        /** Visual object of the second line. Not owned. */
        CAlfTextVisual* iSecondLineVisualText;
        /** Visual text for tuning text. Not owned */
        CAlfTextVisual* iTuningVisualText;

        /** The layout for the station info visuals */
        CAlfAnchorLayout* iStationInformationAnchorLayout;
        /** The layout for temporary station info fade out effect */
        CAlfAnchorLayout* iStationInformationTempAnchorLayout;
        
        /** Text color of the station information visuals */
        TRgb iStationInformationTextColor;
                
        /** Text style id of the first line */
        TInt iFirstLineTextStyleId;
        /** Text style id of the second line */
        TInt iSecondLineTextStyleId;
        /** Text style id of the tuning text line */
        TInt iTuningTextStyleId;
        
        /** Display style         */
        TFMRadioDisplayStyle iDisplayStyle;
        /** Display orientation */
        TDisplayOrientation iOrientation;
        /** Scroll direction */
        TScrollDirection iScrollDirection;
        // Bitmap Animation
        CFMRadioAlfBitmapAnimation* iBmpAnimation;
        // Flag to indicate mirrored layout usage
        TBool iMirrored;
        // visual size for bitmap animation positioning
        TSize iTuningTextVisualSize;
        // counts rt text (3rd line) scrollout -events for rt-timeout.
        TInt iScrollOutCounter;
        // Pointer to CFMRadioAppUi. Not owned
        CFMRadioAppUi* iAppUi;
        // key event
        TInt iKeyScanCode;
        /**
        * If the long key event was detected or not. If it was, the subsequent key up should be ignored
        */
        TBool iLongKeyTriggered;
        /** 
        * Long key press timer. Owned.
        */
        CPeriodic* iLongPressTimer;
        // The position of the visual before the dragging started
        TAlfTimedPoint iDragStartPosition;
        // Status for determinging whether or not to revert the visual back to it's original position
        TBool iRevertGestureDrag;
        // The axis for the currently ongoing stroke
        TFMRadioDragAxis iDragAxis;
        // Tactile feedback for closing the popup
        MTouchFeedback* iTouchFeedback; // For Tactile feedback
    };

#endif //FMRADIOALFVISUALIZER_H
