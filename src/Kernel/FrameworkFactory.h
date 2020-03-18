#pragma once

#include "Interface/FrameworkFactoryInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    template<class T>
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
            FactoryPtr factory = Helper::makeFactoryDefault<T>( MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( factory, false );

            m_factory = factory;

            return true;
        }

    public:
        FrameworkInterfacePtr createFramework( const DocumentPtr & _doc ) override
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
        FrameworkFactoryInterfacePtr makeFrameworkFactory( const DocumentPtr & _doc )
        {
            MENGINE_UNUSED( _doc );

            FrameworkFactoryInterface * factory = Helper::newT<FactorableUnique<FrameworkFactory<T>>>();

            MENGINE_ASSERTION_MEMORY_PANIC( factory, nullptr );

#ifdef MENGINE_DEBUG
            factory->setDocument( _doc );
#endif

            if( factory->initialize() == false )
            {
                LOGGER_ERROR( "invalid initialize framework (doc %s)"
                    , MENGINE_DOCUMENT_STR( _doc )
                );

                return nullptr;
            }

            return FrameworkFactoryInterfacePtr( factory );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
