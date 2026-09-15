#pragma once

#include "Mosaic/Allocator.hpp"

#include <cstddef>

namespace Mengine
{
    class ToolMosaicAllocator
        : public Mosaic::Allocator
    {
    public:
        ToolMosaicAllocator();
        ~ToolMosaicAllocator() override;

    public:
        void * allocate( size_t _size, size_t _alignment ) noexcept override;
        void deallocate( void * _memory, size_t _size, size_t _alignment ) noexcept override;
    };
}
