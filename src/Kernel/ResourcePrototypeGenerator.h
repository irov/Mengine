#pragma once

#include "Interface/EnumeratorServiceInterface.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "Kernel/ScriptablePrototypeGenerator.h"
#else
#include "Kernel/DefaultPrototypeGenerator.h"
#endif

#include "Kernel/Resource.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    template<class Type, uint32_t Count>
    class ResourcePrototypeGenerator
#ifdef MENGINE_USE_SCRIPT_SERVICE
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
        FactorablePointer generate( const DocumentPtr & _doc ) override
        {
            const FactoryPtr & factory = this->getPrototypeFactory();

            ResourcePtr resource = factory->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( resource, "can't generate '%s' '%s' doc '%s'"
                , this->getCategory().c_str()
                , this->getPrototype().c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            UniqueId uniqueIdentity = ENUMERATOR_SERVICE()
                ->generateUniqueIdentity();

            resource->setUniqueIdentity( uniqueIdentity );

#if MENGINE_DOCUMENT_ENABLE
            DocumentPtr doc = MENGINE_DOCUMENT_MESSAGE( "Resource '%s' type '%s' create '%s'"
                , resource->getName().c_str()
                , resource->getType().c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            resource->setDocument( doc );
#endif

#ifdef MENGINE_USE_SCRIPT_SERVICE
            this->setupScriptable( resource );
#endif

            return resource;
        }
    };
}