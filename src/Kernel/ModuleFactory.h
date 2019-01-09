#pragma once

#include "Interface/ModuleFactoryInterface.h"

#include "Kernel/ConstString.h"

#include "Kernel/FactoryDefault.h"

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
        ModuleInterfacePtr createModule( const ConstString & _name ) override
        {
            IntrusivePtr<T> module = m_factory->createObject();

            module->setName( _name );

            return module;
        }

    protected:
        FactoryPtr m_factory;
    };
}
