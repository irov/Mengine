#pragma once

#include "Interface/ModuleInterface.h"

#include "Kernel/ConstString.h"

#include "Kernel/FactoryDefault.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
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

    public:
        ModuleInterfacePtr createModule( const ConstString & _name ) override
        {
            IntrusivePtr<T> module = m_factory->createObject();

            module->setName( _name );

            return module;
        }

    protected:
        void destroy() override
        {
            delete this;
        }

    protected:
        FactoryPtr m_factory;
    };
}
