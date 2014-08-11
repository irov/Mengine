#	pragma once

#	include "Interface/ModuleInterface.h"

#	include "Core/ConstString.h"
#	include "Factory/FactoryStore.h"

//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	template<class T>
	class ModuleFactory
		: public ModuleFactoryInterface
	{
	public:
		ModuleFactory( ServiceProviderInterface * _serviceProvider, const ConstString & _name )
			: m_serviceProvider(_serviceProvider)
			, m_name(_name)
		{
		}

	public:
		ModuleInterfacePtr createModule() override
		{
			ModuleInterface * module = m_factory.createObjectT();
			module->setServiceProvider( m_serviceProvider );
			module->setName( m_name );

			return module;
		}

	protected:
		void destroy() override
		{
			delete this;
		}

	protected:
		ServiceProviderInterface * m_serviceProvider;
		ConstString m_name;

		typedef FactoryDefaultStore<T> TFactoryModule;
		TFactoryModule m_factory;
	};
}
