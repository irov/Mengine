#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/String.h"

#include "figma/figma.hpp"

#include "math/vec2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum class EFigmaActionInputKind
    {
        Pointer = FIGMA_ACTION_INPUT_POINTER,
        Key = FIGMA_ACTION_INPUT_KEY,
        Timer = FIGMA_ACTION_INPUT_TIMER,
        Programmatic = FIGMA_ACTION_INPUT_PROGRAMMATIC
    };
    //////////////////////////////////////////////////////////////////////////
    enum class EFigmaTriggerType
    {
        Click = FIGMA_TRIGGER_CLICK,
        HoverEnter = FIGMA_TRIGGER_HOVER_ENTER,
        HoverLeave = FIGMA_TRIGGER_HOVER_LEAVE,
        Press = FIGMA_TRIGGER_PRESS,
        PointerDown = FIGMA_TRIGGER_POINTER_DOWN,
        PointerUp = FIGMA_TRIGGER_POINTER_UP,
        AfterTimeout = FIGMA_TRIGGER_AFTER_TIMEOUT,
        KeyDown = FIGMA_TRIGGER_KEY_DOWN,
        Unsupported = FIGMA_TRIGGER_UNSUPPORTED
    };
    //////////////////////////////////////////////////////////////////////////
    enum class EFigmaConnectionType
    {
        None = FIGMA_CONNECTION_NONE,
        InternalNode = FIGMA_CONNECTION_INTERNAL_NODE,
        Back = FIGMA_CONNECTION_BACK,
        Close = FIGMA_CONNECTION_CLOSE,
        Unsupported = FIGMA_CONNECTION_UNSUPPORTED
    };
    //////////////////////////////////////////////////////////////////////////
    enum class EFigmaNavigationType
    {
        Navigate = FIGMA_NAVIGATION_NAVIGATE,
        Overlay = FIGMA_NAVIGATION_OVERLAY,
        Swap = FIGMA_NAVIGATION_SWAP,
        ScrollTo = FIGMA_NAVIGATION_SCROLL_TO,
        Unsupported = FIGMA_NAVIGATION_UNSUPPORTED
    };
    //////////////////////////////////////////////////////////////////////////
    enum class EFigmaActionResult
    {
        AllowDefault = FIGMA_ACTION_RESULT_ALLOW_DEFAULT,
        Consume = FIGMA_ACTION_RESULT_CONSUME,
        NavigateFrame = FIGMA_ACTION_RESULT_NAVIGATE_FRAME,
        OpenOverlay = FIGMA_ACTION_RESULT_OPEN_OVERLAY,
        CloseOverlay = FIGMA_ACTION_RESULT_CLOSE_OVERLAY
    };
    //////////////////////////////////////////////////////////////////////////
    enum class EFigmaBindingValueType
    {
        Text,
        Number,
        Boolean,
        Image
    };
    //////////////////////////////////////////////////////////////////////////
    struct FigmaBindingValue
    {
        EFigmaBindingValueType type = EFigmaBindingValueType::Text;
        String stringValue;
        double numberValue = 0.0;
        bool boolValue = false;
    };
    //////////////////////////////////////////////////////////////////////////
    struct FigmaTriggerEvent
    {
        EFigmaActionInputKind inputKind = EFigmaActionInputKind::Pointer;
        EFigmaTriggerType triggerType = EFigmaTriggerType::Click;
        String interactionId;
        String sourceNodeId;
        String currentFrameId;
        uint32_t pointerId = 0;
        float x = 0.f;
        float y = 0.f;
        uint32_t button = 0;
        uint32_t keyCode = 0;
        uint32_t modifiers = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    struct FigmaActionEvent
    {
        EFigmaActionInputKind inputKind = EFigmaActionInputKind::Pointer;
        EFigmaTriggerType triggerType = EFigmaTriggerType::Click;
        EFigmaConnectionType connectionType = EFigmaConnectionType::None;
        EFigmaNavigationType navigationType = EFigmaNavigationType::Navigate;
        String actionId;
        String interactionId;
        String sourceNodeId;
        String currentFrameId;
        String targetFrameId;
        uint32_t pointerId = 0;
        float x = 0.f;
        float y = 0.f;
        uint32_t button = 0;
        uint32_t keyCode = 0;
        uint32_t modifiers = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    struct FigmaActionResponse
    {
        EFigmaActionResult result = EFigmaActionResult::AllowDefault;
        String targetFrameId;
    };
    //////////////////////////////////////////////////////////////////////////
    class UnknownFigmaInterface
        : public UnknownInterface
    {
    public:
        virtual void setResourceFigma( const ResourcePtr & _resource ) = 0;
        virtual const ResourcePtr & getResourceFigma() const = 0;

    public:
        virtual void setViewportSize( const mt::vec2f & _size ) = 0;
        virtual const mt::vec2f & getViewportSize() const = 0;

        virtual void setViewportScale( float _scale ) = 0;
        virtual float getViewportScale() const = 0;

    public:
        virtual void setStartFrameId( const String & _startFrameId ) = 0;
        virtual const String & getStartFrameId() const = 0;

    public:
        virtual void setFontSearchPath( const String & _fontSearchPath ) = 0;
        virtual const String & getFontSearchPath() const = 0;

    public:
        virtual void setPlaybackRate( float _playbackRate ) = 0;
        virtual float getPlaybackRate() const = 0;
        virtual bool replay() = 0;

    public:
        virtual bool inputPointerMove( float _x, float _y ) = 0;
        virtual bool inputPointerDown( float _x, float _y, uint32_t _button ) = 0;
        virtual bool inputPointerUp( float _x, float _y, uint32_t _button ) = 0;
        virtual bool inputPointerCancel( float _x, float _y ) = 0;
        virtual bool inputKeyDown( uint32_t _keyCode ) = 0;
        virtual bool inputKeyUp( uint32_t _keyCode ) = 0;

    public:
        virtual bool setBindingText( const String & _key, const String & _value ) = 0;
        virtual bool setBindingNumber( const String & _key, double _value ) = 0;
        virtual bool setBindingVisible( const String & _key, bool _value ) = 0;
        virtual bool setBindingEnabled( const String & _key, bool _value ) = 0;
        virtual bool setBindingImage( const String & _key, const String & _assetId ) = 0;
        virtual bool setBindingState( const String & _key, bool _value ) = 0;
        virtual bool setBindingValue( const String & _key, const FigmaBindingValue & _value ) = 0;
        virtual bool clearBindingValue( const String & _key ) = 0;

    public:
        virtual bool navigateToFrame( const String & _targetFrameId ) = 0;
        virtual bool openOverlay( const String & _targetFrameId ) = 0;
        virtual bool closeOverlay() = 0;
        virtual bool goBack() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
