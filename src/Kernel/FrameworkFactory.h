#pragma once

#include "Interface/FrameworkFactoryInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class Type>
    class FrameworkFactory
        : public FrameworkFactoryInterface
    {
    public:
        FrameworkFactory()
        {
        }

        ~FrameworkFactory() override
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
        FrameworkInterfacePtr createFramework( const DocumentPtr & _doc ) override
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
        FrameworkFactoryInterfacePtr makeFrameworkFactory( const DocumentPtr & _doc )
        {
            FrameworkFactoryInterfacePtr factory = Helper::makeFactorableUnique<FrameworkFactory<T>>( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( factory );

            if( factory->initialize() == false )
            {
                LOGGER_ERROR( "invalid initialize framework (doc %s)"
                    , MENGINE_DOCUMENT_STR( _doc )
                );

                return nullptr;
            }

            return factory;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
