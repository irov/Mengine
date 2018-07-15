#pragma once

#include "Interface/ModuleInterface.h"

#include "Core/ConstString.h"

#include "Factory/FactoryDefault.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	class ModuleFactory
		: public ServantBase<ModuleFactoryInterface>
	{
    public:
		ModuleFactory()
        {
            m_factory = new FactoryDefault<T>();
        }

	public:
		ModuleInterfacePtr createModule( const ConstString & _name ) override
		{
			ModuleInterfacePtr module = m_factory->createObject();

			module->setName(_name);

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
