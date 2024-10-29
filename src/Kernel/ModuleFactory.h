#pragma once

#include "Interface/FactoryInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionFactory.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T>
        FactoryInterfacePtr makeModuleFactory( const DocumentInterfacePtr & _doc )
        {
            FactoryInterfacePtr factory = Helper::makeFactoryDefault<T>( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( factory, "invalid create module factory (doc %s)"
                , MENGINE_DOCUMENT_STR( _doc )
            );

            return factory;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
