#pragma once

#include "Kernel/IntrusivePtr.h"
#include "Kernel/MemoryAllocator.h"

namespace Mengine
{
    template<class Base>
    class FactorableUnique
        : public Base
        , public MemoryAllocator<Base>
    {
    public:
        template<class ... Args>
        FactorableUnique( Args ... args )
            : Base( args ... )
        {
        }

        ~FactorableUnique() override
        {
        }

    public:
        void destroy() override
        {
            this->_destroy();

            delete this;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class Base, class ... Args>
        IntrusivePtr<Base> makeFactorableUnique( Args && ... _args )
        {
            Base * factorable = new FactorableUnique<Base>( std::forward<Args>( _args ) ... );

            return IntrusivePtr<Base>( factorable );
        }
    }
}
