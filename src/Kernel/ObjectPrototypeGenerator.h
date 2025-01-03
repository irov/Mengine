#pragma once

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#   include "Kernel/ScriptablePrototypeGenerator.h"
#else
#   include "Kernel/DefaultPrototypeGenerator.h"
#endif

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    template<class Type, uint32_t Count>
    class ObjectPrototypeGenerator
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        : public ScriptablePrototypeGenerator<Type, Count>
#else
        : public DefaultPrototypeGenerator<Type, Count>
#endif
    {
    public:
        ObjectPrototypeGenerator()
        {
        }

        ~ObjectPrototypeGenerator() override
        {
        }

    protected:
        FactorablePointer generate( const DocumentInterfacePtr & _doc ) override
        {
            const FactoryInterfacePtr & factory = this->getPrototypeFactory();

            IntrusivePtr<Type> object = factory->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( object, "can't generate '%s' '%s' doc '%s'"
                , this->getCategory().c_str()
                , this->getPrototype().c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc = MENGINE_DOCUMENT_MESSAGE( "Object type '%s' create '%s'"
                , object->getType().c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            object->setDocument( doc );
#endif

#if defined(MENGINE_USE_SCRIPT_SERVICE)
            this->setupScriptable( object );
#endif

            return object;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class Type, uint32_t Count>
        FactoryPrototypeGeneratorPtr makeObjectPrototypeGenerator( const DocumentInterfacePtr & _doc )
        {
            FactoryPrototypeGeneratorPtr generator = Helper::makeFactorableUnique<ObjectPrototypeGenerator<Type, Count>>( _doc );

            return generator;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}