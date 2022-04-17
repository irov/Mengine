#pragma once

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "Kernel/ScriptablePrototypeGenerator.h"
#else
#include "Kernel/DefaultPrototypeGenerator.h"
#endif

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/EnumeratorHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    template<class Type, uint32_t Count>
    class IdentityPrototypeGenerator
#ifdef MENGINE_USE_SCRIPT_SERVICE
        : public ScriptablePrototypeGenerator<Type, Count>
#else
        : public DefaultPrototypeGenerator<Type, Count>
#endif
    {
    public:
        IdentityPrototypeGenerator()
        {
        }

        ~IdentityPrototypeGenerator() override
        {
        }

    protected:
        FactorablePointer generate( const DocumentPtr & _doc ) override
        {
            const FactoryPtr & factory = this->getPrototypeFactory();

            IntrusivePtr<Type> object = factory->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( object, "can't generate '%s' '%s' doc '%s'"
                , this->getCategory().c_str()
                , this->getPrototype().c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            UniqueId uniqueIdentity = Helper::generateUniqueIdentity();

            object->setUniqueIdentity( uniqueIdentity );

#if MENGINE_DOCUMENT_ENABLE
            DocumentPtr doc = MENGINE_DOCUMENT_MESSAGE( "Identity '%s' type '%s' create '%s'"
                , object->getName().c_str()
                , object->getType().c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            object->setDocument( doc );
#endif

#ifdef MENGINE_USE_SCRIPT_SERVICE
            this->setupScriptable( object );
#endif

            return object;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class Type, uint32_t Count>
        FactoryPrototypeGeneratorPtr makeIdentityPrototypeGenerator( const DocumentPtr & _doc )
        {
            FactoryPrototypeGeneratorPtr generator = Helper::makeFactorableUnique<IdentityPrototypeGenerator<Type, Count>>( _doc );

            return generator;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}