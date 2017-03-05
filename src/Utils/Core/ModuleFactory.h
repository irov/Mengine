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
		: public ServantBase<ModuleFactoryInterface>
	{
    public:
        bool initialize( const ConstString & _name )
        {
            m_name = _name;

            m_factory = new FactoryDefault<T>();
        }

	public:
		ModuleInterfacePtr createModule() override
		{
			ModuleInterface * module = m_factory->createObject();

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
		ConstString m_name;

		FactoryPtr m_factory;
	};
}
