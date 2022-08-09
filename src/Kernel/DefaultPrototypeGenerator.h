#pragma once

#include "FactoryPrototypeGenerator.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstString.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/Logger.h"


namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class Type, uint32_t Count>
    class DefaultPrototypeGenerator
        : public FactoryPrototypeGenerator
    {
    public:
        typedef IntrusivePtr<Type> TypePtr;

    public:
        DefaultPrototypeGenerator()
        {
        }

        ~DefaultPrototypeGenerator() override
        {
        }

    protected:
        FactoryInterfacePtr _initializeFactory() override
        {
            FactoryInterfacePtr factory = Helper::makeFactoryPool<Type, Count>( MENGINE_DOCUMENT_FACTORABLE );

            return factory;
        }

        void _finalizeFactory() override
        {
            //Empty
        }

    protected:
        FactorablePointer generate( const DocumentPtr & _doc ) override
        {
            const FactoryInterfacePtr & factory = this->getPrototypeFactory();

            TypePtr object = factory->createObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( object, "can't generate '%s::%s' doc '%s'"
                , this->getCategory().c_str()
                , this->getPrototype().c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

#ifdef MENGINE_DOCUMENT_ENABLE
            object->setDocument( _doc );
#endif

            return object;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class Type, uint32_t Count>
        FactoryPrototypeGeneratorPtr makeDefaultPrototypeGenerator( const DocumentPtr & _doc )
        {
            FactoryPrototypeGeneratorPtr generator = Helper::makeFactorableUnique<DefaultPrototypeGenerator<Type, Count>>( _doc );

            return generator;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}