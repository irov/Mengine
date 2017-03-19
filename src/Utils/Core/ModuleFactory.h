#	pragma once

#	include "Interface/ModuleInterface.h"

#	include "Core/ConstString.h"

#	include "Factory/FactoryDefault.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	class ModuleFactory
		: public ServantBase<ModuleFactoryInterface>
	{
    public:
		ModuleFactory()
        {
            m_factory = new FactoryDefault<T>( m_serviceProvider );
        }

	public:
		ModuleInterfacePtr createModule( const ConstString & _name ) override
		{
			ModuleInterface * module = m_factory->createObject();

			module->setServiceProvider( m_serviceProvider );
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
