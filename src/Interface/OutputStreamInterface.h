#pragma once

#include "Config/Typedef.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    class OutputStreamInterface
        : public Mixin
    {
    public:
        virtual size_t write( const void * _data, size_t _size ) = 0;
        virtual size_t size() const = 0;

    public:
        virtual bool flush() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OutputStreamInterface> OutputStreamInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
