#pragma once

#include "GOAP/GOAP.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class GOAPAllocator
        : public GOAP::Allocator
    {
    public:
        GOAPAllocator();
        ~GOAPAllocator() override;

    protected:
        void * allocate( size_t _size ) override;
        void deallocate( void * _ptr ) override;
    };
}