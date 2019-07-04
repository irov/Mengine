#pragma once

#include "ScriptablePrototypeGenerator.h"

#include "Kernel/Surface.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    template<class Type, uint32_t Count>
    class SurfacePrototypeGenerator
        : public ScriptablePrototypeGenerator<Type, Count>
    {
    protected:
		FactorablePointer generate( const Char * _doc ) override
        {
            const FactoryPtr & factory = this->getFactory();

            SurfacePtr surface = factory->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( surface, nullptr, "can't generate '%s' '%s' doc '%s'"
                , this->getCategory().c_str()
                , this->getPrototype().c_str()
                , _doc
            );

            const ConstString & prototype = this->getPrototype();
            surface->setType( prototype );

            this->setupScriptable( surface );

            if( surface->initialize() == false )
            {
                return nullptr;
            }

            return surface;
        }
    };
}