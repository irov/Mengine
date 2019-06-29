#pragma once

#include "Kernel/IntrusivePtr.h"
#include "Kernel/MemoryAllocator.h"

namespace Mengine
{
    template<class Base>
    class FactorableUnique
        : public Base
        , public MemoryAllocator<FactorableUnique<Base>>
    {
    public:
        template<class ... Args>
        FactorableUnique( Args && ... _args )
            : Base( std::forward<Args>( _args ) ... )
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
        template<class Type, class ... Args>
        IntrusivePtr<Type> makeFactorableUnique( Args && ... _args )
        {
            Type * factorable = new FactorableUnique<Type>( std::forward<Args>( _args ) ... );

            return IntrusivePtr<Type>( factorable );
        }
    }
}
