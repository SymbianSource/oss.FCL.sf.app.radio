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
* Description:  Definition of the class CFMRadioAlfIndicator.
*
*/


#ifndef ALF_INDICATOR_H
#define ALF_INDICATOR_H

// INCLUDES

#include <AknUtils.h>
#include <alf/alfcontrol.h>
#include <alf/alfanchorlayout.h>

// FORWARD DECLARATIONS

class CAlfTextVisual;

// CLASS DEFINITION

/**
* Text indicator control class
* 
* Provides means to show and fade in/out a specific text inside the supplied "layout".
* 
*/ 
class CFMRadioAlfIndicator : public CAlfControl
	{
public:		// Constructors and destructor

		/**
	    * Epoc two-phased constructor.
	    * @param aEnv Reference to the HitchCock environment instance.
        * @return Pointer to this Indicator component.
	    */
		static CFMRadioAlfIndicator* NewL( CAlfEnv& aEnv );
		/**
        * Destructor
        */
		virtual ~CFMRadioAlfIndicator();
public: 	// Functions from base classes		
		/**
 	    * From CAlfControl, takes care of alfred event handling.
 	    * @param aEvent
 	    */
		TBool OfferEventL( const TAlfEvent& aEvent );	
public:		// New functions	
		/**
		* Shows the indicator with the defined opacity value.
		* @param aShowWithFading ETrue if the indicator will be shown with fading; EFalse otherwise.
		* @see SetOpacityInVisibleState()
		*/
		void Show( TBool aShowWithFading );
		/**
		* Hides the indicator with the defined opacity value.
		* @param aHideWithFading ETrue if the indicator will be hidden with fading; EFalse otherwise. 
		* @see SetOpacityInHiddenState()
		*/
		void Hide( TBool aHideWithFading );	
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
	    * Sets the indicator text.
	    * @param aText The indicator text to be shown
	    */
		void SetTextL( const TDesC& aText );
		/**
	    * Changes indicator text color
	    * @param aTextColor Color of the indicator text
	    */		
		void SetTextColor( const TRgb& aTextColor );
		/** 
		* Sets the indicator rectangle
		* @param aRect Rectangle to the indicator
	    */	
		void SetRect( const TRect& aRect );
private:
		/**
		* C++ default constructor
		*/
		CFMRadioAlfIndicator( );
		/**
	    * EPOC second phase constructor.
	    * @param aEnv Reference to the Hitchcock environment instance.
	    */
		void ConstructL( CAlfEnv& aEnv );
private:	// New functions
		/**
	    * Sets the fading animation to the CAlfVisual object.
	    * @param aVisual a pointer to the visual object
	    * @param aFadingTime Time duration after the visual object has been faded. Fading duration in milliseconds. 
	    * @param aOpacity Target opacity value
	    */
		void Fade( CAlfVisual* aVisual, TInt aFadingTime, TReal aOpacity ) const;
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
	    * Sets absolute rect of the anchor by top left and size of the rect.
	    * @param aAnchor Anchor layout for setting placement
	    * @param aOrdinal Index of visual element
	    * @param aTopLeftPosition Top left point of the rect
	    * @param aSize Size of the rect
	    */	
		void SetAbsoluteCornerAnchors( CAlfAnchorLayout* aAnchor,
									   TInt aOrdinal,
									   const TPoint& aTopLeftPosition,
									   const TSize& aSize );
		/**
	    * Sets relative rect of the anchor by top left and bottom right points.
	    * @param aAnchor Anchor layout for setting placement
	    * @param aOrdinal Index of visual element
	    * @param aTopLeftPosition Top left point of the rect
	    * @param aBottomRightPosition Bottom right point of the rect
	    */
		void SetRelativeCornerAnchors( CAlfAnchorLayout* aAnchor,
									   TInt aOrdinal,
									   const TAlfRealPoint& aTopLeftPosition,
									   const TAlfRealPoint& aBottomRightPosition );
		/**
	    * Sets relative rect of the anchor by top left and size of the rect.
	    * @param aAnchor Anchor layout for setting placement
	    * @param aOrdinal Index of visual element
	    * @param aTopLeftPosition Top left point of the rect
	    * @param aSize Size of the rect
	    */
		void SetRelativeCornerAnchors( CAlfAnchorLayout* aAnchor, 
									   TInt aOrdinal, 
									   const TAlfRealPoint& aTopLeftPosition, 
									   const TAlfRealSize& aSize );	
 		/** 
	    * Creates the needed drawing layers and visual objects
		* for the indicator.
	    */
		void AddIndicatorLayerL();
		/**
 	    * Creates the text style for the indicator
 	    */
		void SetTextStyleL();
		
private:	// Data
	    	 
	    /** Anchor layout for indicator */
	    CAlfAnchorLayout* iIndicatorTextAnchor;
	    /** Visual object of the indicator. Not owned. */
	    CAlfTextVisual* iText;   		
	    /** Text color of the indicator text */
	    TRgb iTextColor;	    
	    /** Text style id of the indicator text */
	    TInt iTextStyleId;
	    /** Indicator opacity in visible state */
	    TReal iOpacityInVisibleState;
	    /** Indicator opacity in hidden state */
	    TReal iOpacityInHiddenState;
	    /** Indicator rectangle */
	    TRect iRect;    	  
	};

#endif //ALF_INDICATOR_H
