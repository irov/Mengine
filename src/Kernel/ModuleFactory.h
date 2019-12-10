#pragma once

#include "Interface/ModuleFactoryInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    template<class T>
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
            FactoryPtr factory = Helper::makeFactoryDefault<T>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( factory, false );

            m_factory = factory;

            return true;
        }

    public:
        ModuleInterfacePtr createModule( const DocumentPtr & _doc ) override
        {
            IntrusivePtr<T> module = m_factory->createObject( _doc );

            return module;
        }

    protected:
        FactoryPtr m_factory;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T>
        ModuleFactoryInterfacePtr makeModuleFactory( const DocumentPtr & _doc )
        {
            MENGINE_UNUSED( _doc );

            ModuleFactoryInterface * factory = new FactorableUnique<ModuleFactory<T>>();

            MENGINE_ASSERTION_MEMORY_PANIC( factory, nullptr );

#ifdef MENGINE_DEBUG
            factory->setDocument( _doc );
#endif

            if( factory->initialize() == false )
            {
                LOGGER_ERROR( "invalid initialize module (doc %s)"
                    , MENGINE_DOCUMENT_MESSAGE( _doc )
                );

                return nullptr;
            }

            return ModuleFactoryInterfacePtr( factory );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
