#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/String.h"

#include "Figma/ActionRouter.h"

#include "math/vec2.h"

namespace Mengine
{
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
        ::Figma::EActionInputKind inputKind = ::Figma::EActionInputKind::Pointer;
        ::Figma::ETriggerType triggerType = ::Figma::ETriggerType::Click;
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
        ::Figma::EActionInputKind inputKind = ::Figma::EActionInputKind::Pointer;
        ::Figma::ETriggerType triggerType = ::Figma::ETriggerType::Click;
        ::Figma::EConnectionType connectionType = ::Figma::EConnectionType::None;
        ::Figma::ENavigationType navigationType = ::Figma::ENavigationType::Navigate;
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
        ::Figma::EActionResult result = ::Figma::EActionResult::AllowDefault;
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
