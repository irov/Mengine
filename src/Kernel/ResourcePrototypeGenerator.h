#	pragma once

#	include "Interface/PrototypeManagerInterface.h"

#	include "Kernel/ResourceReference.h"

#	include "Core/ConstString.h"
#	include "Factory/FactoryStore.h"

#	include "Logger/Logger.h"

namespace Menge
{
	template<class Type, uint32_t Count>
	class ResourcePrototypeGenerator
		: public PrototypeGeneratorInterface
	{
	public:
		ResourcePrototypeGenerator()
			: m_serviceProvider(nullptr)
			, m_scriptWrapper(nullptr)
		{
		}

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override
		{
			m_serviceProvider = _serviceProvider;
		}

		ServiceProviderInterface * getServiceProvider() const override
		{
			return m_serviceProvider;
		}

	protected:
		bool initialize( const ConstString & _category, const ConstString & _prototype ) override
		{
			m_category = _category;
			m_prototype = _prototype;

			m_scriptWrapper = SCRIPT_SERVICE( m_serviceProvider )
				->getWrapper( m_prototype );

			return true;
		}

	protected:
		Factorable * generate() override
		{
			Type * resource = m_factory.createObject();

			if( resource == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourcePrototypeGenerator can't generate %s %s"
					, m_category.c_str()
					, m_prototype.c_str()
					);

				return nullptr;
			}

			resource->setServiceProvider( m_serviceProvider );
			resource->setType( m_prototype );
			resource->setScriptWrapper( m_scriptWrapper );


			return resource;
		}

		uint32_t count() const override
		{
			uint32_t count = m_factory.countObject();

			return count;
		}

		void destroy() override
		{
			delete this;
		}

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConstString m_category;
		ConstString m_prototype;

		ScriptWrapperInterface * m_scriptWrapper;

		typedef FactoryPoolStore<Type, Count> TResourceFactory;
		TResourceFactory m_factory;
	};
}