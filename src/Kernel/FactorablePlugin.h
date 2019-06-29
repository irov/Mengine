#pragma once

#include "Kernel/IntrusivePtr.h"
#include "Kernel/MemoryAllocator.h"

namespace Mengine
{
    template<class Base>
    class FactorablePlugin
        : public Base
        , public MemoryAllocator<FactorablePlugin<Base>>
    {
    public:
        template<class ... Args>
        FactorablePlugin( Args && ... _args )
            : Base( std::forward<Args>( _args ) ... )
        {
        }

        ~FactorablePlugin() override
        {
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class Type, class ... Args>
        IntrusivePtr<Type> makeFactorablePlugin( Args && ... _args )
        {
            Type * factorable = new FactorablePlugin<Type>( std::forward<Args>( _args ) ... );

            return IntrusivePtr<Base>( factorable );
        }
    }
}
