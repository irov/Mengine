#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/PrefetcherObserverInterface.h"
#include "Interface/ContentInterface.h"

#include "Kernel/Unknowable.h"
#include "Kernel/CompilableReference.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TextFontEffectInterface
        : public ServantInterface
        , public CompilableReference
        , public Unknowable
    {
    public:
        virtual void setContent( const ContentInterfacePtr & _content ) = 0;
        virtual const ContentInterfacePtr & getContent() const = 0;

    public:
        virtual void setEffectSample( uint32_t _effectSample ) = 0;
        virtual uint32_t getEffectSample() const = 0;
        virtual float getEffectSampleInv() const = 0;

    public:
        virtual uint32_t getLayoutCount() const = 0;

    public:
        typedef Lambda<void( uint32_t _index, int32_t _x, int32_t _y, uint32_t _width, uint32_t _height, const void * _buffer, uint32_t _pitch, uint32_t _bytespp )>  LambdaFontEffectProvider;

        virtual bool apply( uint32_t _width, uint32_t _rows, uint32_t _pitch, const void * _buffer, uint32_t _channel, int32_t _left, int32_t _top, uint32_t _height, const LambdaFontEffectProvider & _provider ) = 0;

    public:
        virtual bool prefetch( const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual bool unfetch() = 0;

    public:
        virtual bool isValid() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextFontEffectInterface> TextFontEffectInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
