#pragma once

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "Kernel/ScriptablePrototypeGenerator.h"
#else
#   include "Kernel/DefaultPrototypeGenerator.h"
#endif

#include "Kernel/Resource.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/EnumeratorHelper.h"

namespace Mengine
{
    template<class Type, uint32_t Count>
    class ResourcePrototypeGenerator
#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
        : public ScriptablePrototypeGenerator<Type, Count>
#else
        : public DefaultPrototypeGenerator<Type, Count>
#endif
    {
    public:
        ResourcePrototypeGenerator()
        {
        }

        ~ResourcePrototypeGenerator() override
        {
        }

    protected:
        FactorablePointer generate( const DocumentInterfacePtr & _doc ) override
        {
            const FactoryInterfacePtr & factory = this->getPrototypeFactory();

            ResourcePtr resource = factory->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( resource, "can't generate '%s' '%s' doc '%s'"
                , this->getCategory().c_str()
                , this->getPrototype().c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            UniqueId uniqueIdentity = Helper::generateUniqueIdentity();

            resource->setUniqueIdentity( uniqueIdentity );

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc = MENGINE_DOCUMENT_MESSAGE( "Resource '%s' type '%s' create '%s'"
                , resource->getName().c_str()
                , resource->getType().c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            resource->setDocument( doc );
#endif

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
            this->setupScriptable( resource );
#endif

            return resource;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class Type, uint32_t Count>
        FactoryPrototypeGeneratorPtr makeResourcePrototypeGenerator( const DocumentInterfacePtr & _doc )
        {
            FactoryPrototypeGeneratorPtr generator = Helper::makeFactorableUnique<ResourcePrototypeGenerator<Type, Count>>( _doc );

            return generator;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}