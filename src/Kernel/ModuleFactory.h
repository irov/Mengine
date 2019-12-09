#pragma once

#include "Interface/ModuleFactoryInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"

namespace Mengine
{
    template<class T>
    class ModuleFactory
        : public ModuleFactoryInterface
        , public Factorable
    {
    public:
        ModuleFactory()
        {
            m_factory = Helper::makeFactoryDefault<T>();
        }

        ~ModuleFactory() override
        {
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
        ModuleFactoryInterfacePtr makeModuleFactory()
        {
            ModuleFactoryInterface * factory = new FactorableUnique<ModuleFactory<T>>();

            return ModuleFactoryInterfacePtr( factory );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
