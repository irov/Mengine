#pragma once

#include "Config/StdInt.h"

#include "Mosaic/Allocator.hpp"

namespace Mengine
{
    class MosaicAllocator
        : public Mosaic::Allocator
    {
    public:
        MosaicAllocator();
        ~MosaicAllocator() override;

    public:
        void * allocate( size_t _size, size_t _alignment ) noexcept override;
        void deallocate( void * _memory, size_t _size, size_t _alignment ) noexcept override;
    };
}
