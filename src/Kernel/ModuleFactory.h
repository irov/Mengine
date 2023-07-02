#pragma once

#include "Interface/ModuleFactoryInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionFactory.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class Type>
    class ModuleFactory
        : public ModuleFactoryInterface
    {
    public:
        ModuleFactory()
        {
        }

        ~ModuleFactory() override
        {
        }

    public:
        bool initialize() override
        {
            FactoryInterfacePtr factory = Helper::makeFactoryDefault<Type>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( factory );

            m_factory = factory;

            return true;
        }

        void finalize() override
        {
            MENGINE_ASSERTION_FACTORY_EMPTY( m_factory );

            m_factory = nullptr;
        }

    public:
        ModuleInterfacePtr createModule( const DocumentInterfacePtr & _doc ) override
        {
            IntrusivePtr<Type> module = m_factory->createObject( _doc );

            return module;
        }

    protected:
        FactoryInterfacePtr m_factory;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T>
        ModuleFactoryInterfacePtr makeModuleFactory( const DocumentInterfacePtr & _doc )
        {
            MENGINE_UNUSED( _doc );

            ModuleFactoryInterfacePtr factory = Helper::makeFactorableUnique<ModuleFactory<T>>( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( factory );

            if( factory->initialize() == false )
            {
                LOGGER_ERROR( "invalid initialize module (doc %s)"
                    , MENGINE_DOCUMENT_STR( _doc )
                );

                return nullptr;
            }

            return factory;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
