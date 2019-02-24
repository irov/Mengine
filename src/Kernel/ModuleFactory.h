#pragma once

#include "Interface/ModuleFactoryInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Document.h"

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
            m_factory = new FactoryDefault<T>();
        }

        ~ModuleFactory() override
        {
        }

    public:
        ModuleInterfacePtr createModule( const ConstString & _name, const Char * _doc ) override
        {
			IntrusivePtr<T> module = m_factory->createObject( _doc );

            module->setName( _name );

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
}
