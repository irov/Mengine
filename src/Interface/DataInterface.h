#pragma once

#include "Config/Config.h"
#include "Config/Typedef.h"

#include "Interface/Interface.h"
#include "Kernel/Pointer.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DataInterface
        : public Interface
    {
    public:
        virtual bool acquire() = 0;
        virtual void release() = 0;

    public:
        virtual Pointer allocateMemory( size_t _size ) const = 0;

    public:
        template<class T>
        Pointer allocateMemoryT( size_t _count ) const
        {
            size_t total_size = sizeof( T ) * _count;

            return this->allocateMemory( total_size );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DataInterface> DataInterfacePtr;
    //////////////////////////////////////////////////////////////////////////    
}
