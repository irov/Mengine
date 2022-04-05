#pragma once

#include "FactoryPrototypeGenerator.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstString.h"
#include "Kernel/FactoryPoolVirtualInheritance.h"
#include "Kernel/Logger.h"


namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class Type, uint32_t Count>
    class DefaultVirtualInheritancePrototypeGenerator
        : public FactoryPrototypeGenerator
    {
    public:
        typedef IntrusivePtr<Type> TypePtr;

    public:
        DefaultVirtualInheritancePrototypeGenerator()
        {
        }

        ~DefaultVirtualInheritancePrototypeGenerator() override
        {
        }

    protected:
        FactoryPtr _initializeFactory() override
        {
            FactoryPtr factory = Helper::makeFactoryPoolVirtualInheritance<Type, Count>( MENGINE_DOCUMENT_FACTORABLE );

            return factory;
        }

        void _finalizeFactory() override
        {
            //Empty
        }

    protected:
        FactorablePointer generate( const DocumentPtr & _doc ) override
        {
            const FactoryPtr & factory = this->getPrototypeFactory();

            TypePtr object = factory->createVirtualInheritanceObject( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( object, "can't generate '%s::%s' doc '%s'"
                , this->getCategory().c_str()
                , this->getPrototype().c_str()
                , MENGINE_DOCUMENT_STR( _doc )
            );

#if MENGINE_DOCUMENT_ENABLE
            object->setDocument( _doc );
#endif

            return object;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class Type, uint32_t Count>
        FactoryPrototypeGeneratorPtr makeDefaultVirtualInheritancePrototypeGenerator( const DocumentPtr & _doc )
        {
            FactoryPrototypeGeneratorPtr generator = Helper::makeFactorableUnique<DefaultVirtualInheritancePrototypeGenerator<Type, Count>>( _doc );

            return generator;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}