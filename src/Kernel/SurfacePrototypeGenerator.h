#pragma once

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#include "Kernel/ScriptablePrototypeGenerator.h"
#else
#include "Kernel/DefaultPrototypeGenerator.h"
#endif

#include "Kernel/Surface.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    template<class Type, uint32_t Count>
    class SurfacePrototypeGenerator
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        : public ScriptablePrototypeGenerator<Type, Count>
#else
        : public DefaultPrototypeGenerator<Type, Count>
#endif
    {
    public:
        SurfacePrototypeGenerator()
        {
        }

        ~SurfacePrototypeGenerator() override
        {
        }

    protected:
        FactorablePointer generate( const DocumentInterfacePtr & _doc ) override
        {
            const FactoryInterfacePtr & factory = this->getPrototypeFactory();

            SurfacePtr surface = factory->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( surface, "can't generate '%s' '%s' doc '%s'"
                , this->getCategory().c_str()
                , this->getPrototype().c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            UniqueId uniqueIdentity = Helper::generateUniqueIdentity();

            surface->setUniqueIdentity( uniqueIdentity );

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc = MENGINE_DOCUMENT_MESSAGE( "Surface '%s' type '%s' create '%s'"
                , surface->getName().c_str()
                , surface->getType().c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            surface->setDocument( doc );
#endif

#if defined(MENGINE_USE_SCRIPT_SERVICE)
            this->setupScriptable( surface );
#endif

            if( surface->initialize() == false )
            {
                return nullptr;
            }

            return surface;
        }
    };
}