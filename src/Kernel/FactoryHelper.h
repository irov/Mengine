#pragma once

#include "Interface/FactoryInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    namespace Helper
    {
        template<class F, class Type, class ... Args>
        FactoryInterfacePtr makeFactory( const DocumentInterfacePtr & _doc, Args && ... _args )
        {
            FactoryInterfacePtr factory = Helper::makeFactorableUnique<F>( _doc, std::forward<Args &&>( _args ) ... );

            MENGINE_ASSERTION_MEMORY_PANIC( factory );

            const ConstString & type = Type::getFactorableType();

            factory->setType( type );

            return factory;
        }
    }
}