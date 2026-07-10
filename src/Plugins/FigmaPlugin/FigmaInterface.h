#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/String.h"

#include "math/vec2.h"

namespace Mengine
{
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
        virtual bool inputPointerMove( float _x, float _y ) = 0;
        virtual bool inputPointerDown( float _x, float _y, uint32_t _button ) = 0;
        virtual bool inputPointerUp( float _x, float _y, uint32_t _button ) = 0;
        virtual bool inputPointerCancel( float _x, float _y ) = 0;
        virtual bool inputKeyDown( uint32_t _keyCode ) = 0;
        virtual bool inputKeyUp( uint32_t _keyCode ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
