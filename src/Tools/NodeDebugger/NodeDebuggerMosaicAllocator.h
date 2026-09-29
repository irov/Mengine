#pragma once

#include "Config/StdDef.h"

#include "Mosaic/Allocator.hpp"

namespace Mengine
{
    class NodeDebuggerMosaicAllocator
        : public Mosaic::Allocator
    {
    public:
        NodeDebuggerMosaicAllocator();
        ~NodeDebuggerMosaicAllocator() override;

    public:
        void * allocate( size_t _size, size_t _alignment ) noexcept override;
        void deallocate( void * _memory, size_t _size, size_t _alignment ) noexcept override;
    };
}
