#pragma once

#include "Interface/ExecutorInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    namespace Helper
    {
        template<class T, class ... Args>
        bool execute( const DocumentInterfacePtr & _doc, Args && ... _args )
        {
            ExecutorInterfacePtr executor = Helper::makeFactorableUnique<T>( _doc, std::forward<Args>( _args ) ... );

            bool result = executor->execute();

            return result;
        }
    }
}