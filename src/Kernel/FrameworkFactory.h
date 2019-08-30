#pragma once

#include "Interface/FrameworkFactoryInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Document.h"

namespace Mengine
{
    template<class T>
    class FrameworkFactory
        : public FrameworkFactoryInterface
        , public Factorable
    {
    public:
        FrameworkFactory()
        {
            m_factory = Helper::makeFactoryDefault<T>();
        }

        ~FrameworkFactory() override
        {
        }

    public:
        FrameworkInterfacePtr createFramework( const Char * _doc ) override
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
        FrameworkFactoryInterfacePtr makeFrameworkFactory()
        {
            FrameworkFactoryInterface * factory = new FactorableUnique<FrameworkFactory<T>>();

            return FrameworkFactoryInterfacePtr( factory );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
