#include "NodeDebuggerMosaicAllocator.h"

#include "Kernel/AllocatorHelper.h"
#include "Kernel/Assertion.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerMosaicAllocator::NodeDebuggerMosaicAllocator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    NodeDebuggerMosaicAllocator::~NodeDebuggerMosaicAllocator()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void * NodeDebuggerMosaicAllocator::allocate( size_t _size, size_t _alignment ) noexcept
    {
        MENGINE_ASSERTION_FATAL( _alignment <= alignof( max_align_t ), "mosaic request over-aligned memory '%zu'"
            , _alignment
        );

        MENGINE_UNUSED( _alignment );

        void * memory = Helper::allocateMemory( _size, "mosaic" );

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    void NodeDebuggerMosaicAllocator::deallocate( void * _memory, size_t _size, size_t _alignment ) noexcept
    {
        MENGINE_UNUSED( _size );
        MENGINE_UNUSED( _alignment );

        Helper::deallocateMemory( _memory, "mosaic" );
    }
    //////////////////////////////////////////////////////////////////////////
}
