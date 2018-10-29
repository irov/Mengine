#pragma once

#include "Interface/InputStreamInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/Pointer.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MemoryInterface
        : public Mixin
    {
    public:
        virtual Pointer getBuffer() const = 0;
        virtual size_t getSize() const = 0;
        virtual bool empty() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryInterface> MemoryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class MemoryBufferInterface
        : public MemoryInterface
    {
    public:
        virtual void setBuffer( const void * _ptr, size_t _size, const Char * _doc, const Char * _file, uint32_t _line ) = 0;
        virtual Pointer newBuffer( size_t _size, const Char * _doc, const Char * _file, uint32_t _line ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryBufferInterface> MemoryBufferInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class MemoryProxyInterface
        : public MemoryInterface
    {
    public:
        virtual void setBuffer( void * _ptr, size_t _size, const Char * _file, uint32_t _line ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryProxyInterface> MemoryProxyInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class MemoryGetterStreamInterface
        : public InputStreamInterface
    {
    public:
        virtual Pointer getBuffer() const = 0;
        virtual size_t getSize() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class MemoryCacheInputInterface
        : public MemoryGetterStreamInterface
    {
    public:
        virtual Pointer cacheBuffer( size_t _size, const char * _doc, const Char * _file, uint32_t _line ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryCacheInputInterface> MemoryCacheInputInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class MemoryProxyInputInterface
        : public MemoryGetterStreamInterface
    {
    public:
        virtual Pointer setBuffer( void * _memory, size_t _offset, size_t _size ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryProxyInputInterface> MemoryProxyInputInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class MemoryInputInterface
        : public MemoryGetterStreamInterface
    {
    public:
        virtual Pointer newBuffer( size_t _size ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryInputInterface> MemoryInputInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
