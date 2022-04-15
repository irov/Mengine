#pragma once

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/ObjectPrototypeGenerator.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T, uint32_t Count>
        bool addDefaultPrototype( const ConstString & _category, const ConstString & _prototype, const DocumentPtr & _doc )
        {
            PrototypeGeneratorInterfacePtr generator = Helper::makeDefaultPrototypeGenerator<T, Count>( _doc );

            bool result = PROTOTYPE_SERVICE()
                ->addPrototype( _category, _prototype, generator );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T, uint32_t Count>
        bool addObjectPrototype( const ConstString & _category, const ConstString & _prototype, const DocumentPtr & _doc )
        {
            PrototypeGeneratorInterfacePtr generator = Helper::makeObjectPrototypeGenerator<T, Count>( _doc );

            bool result = PROTOTYPE_SERVICE()
                ->addPrototype( _category, _prototype, generator );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}