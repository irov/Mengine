#pragma once

#include "Kernel/IntrusivePtr.h"
#include "Kernel/MemoryAllocator.h"

namespace Mengine
{
    template<class Base>
    class FactorablePlugin
        : public Base
        , public MemoryAllocator<Base>
    {
    public:
        template<class ... Args>
        FactorablePlugin( Args ... args )
            : Base( args ... )
        {
        }

        ~FactorablePlugin() override
        {
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class Base, class ... Args>
        IntrusivePtr<Base> makeFactorablePlugin( Args && ... _args )
        {
            Base * factorable = new FactorablePlugin<Base>( std::forward<Args>( _args ) ... );

            return IntrusivePtr<Base>( factorable );
        }
    }
}
