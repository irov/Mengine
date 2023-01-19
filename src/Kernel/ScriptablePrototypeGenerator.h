#pragma once

#include "Kernel/BaseScriptablePrototypeGenerator.h"

#include "Kernel/Scriptable.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    template<class Type, uint32_t Count>
    class ScriptablePrototypeGenerator
        : public BaseScriptablePrototypeGenerator
    {
    public:
        ScriptablePrototypeGenerator()
        {
        }

        ~ScriptablePrototypeGenerator() override
        {
        }

    protected:
        typedef IntrusivePtr<Type> TypePtr;

        FactoryInterfacePtr _initializeFactory() override
        {
            this->registerScriptWrapperObserver();

            FactoryInterfacePtr factory = Helper::makeFactoryPool<Type, Count>( MENGINE_DOCUMENT_FACTORABLE );

            return factory;
        }

        void _finalizeFactory() override
        {
            this->unregisterScriptWrapperObserver();
        }

    protected:
        FactorablePointer generate( const DocumentPtr & _doc ) override
        {
            const FactoryInterfacePtr & factory = this->getPrototypeFactory();

            TypePtr scriptable = factory->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( scriptable, "can't generate category '%s' prototype '%s' doc '%s'"
                , this->getCategory().c_str()
                , this->getPrototype().c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

            this->setupScriptable( scriptable );

            return scriptable;
        }
    };
}