#include "ToolMosaicAllocator.h"

#include "Kernel/AllocatorHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ToolMosaicAllocator::ToolMosaicAllocator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ToolMosaicAllocator::~ToolMosaicAllocator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void * ToolMosaicAllocator::allocate( size_t _size, size_t _alignment ) noexcept
    {
        MENGINE_UNUSED( _alignment );

        void * memory = Helper::allocateMemory( _size, "mosaic" );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    void ToolMosaicAllocator::deallocate( void * _memory, size_t _size, size_t _alignment ) noexcept
    {
        MENGINE_UNUSED( _size );
        MENGINE_UNUSED( _alignment );

        Helper::deallocateMemory( _memory, "mosaic" );
    }
    //////////////////////////////////////////////////////////////////////////
}
