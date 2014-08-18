#	pragma once

#	include "Interface/PrototypeManagerInterface.h"

#	include "Kernel/ResourceReference.h"

#	include "Core/ConstString.h"
#	include "Factory/FactoryStore.h"

#	include "Logger/Logger.h"

namespace Menge
{
	template<class Type, size_t Count>
	class ResourcePrototypeGenerator
		: public PrototypeGeneratorInterface
	{
	public:
		ResourcePrototypeGenerator( ServiceProviderInterface * _serviceProvider )
			: m_serviceProvider(_serviceProvider)
		{
		}

	protected:
		Factorable * generate( const ConstString & _category, const ConstString & _prototype ) override
		{
			ResourceReference * resource = m_factory.createObjectT();

			if( resource == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourcePrototypeGenerator can't generate %s %s"
					, _category.c_str()
					, _prototype.c_str()
					);

				return nullptr;
			}

			resource->setServiceProvider( m_serviceProvider );
			resource->setType( _prototype );                

			return resource;
		}

		size_t count() const override
		{
			size_t count = m_factory.countObject();

			return count;
		}

		void destroy() override
		{
			delete this;
		}

	protected:
		ServiceProviderInterface * m_serviceProvider;

		typedef FactoryPoolStore<Type, Count> TResourceFactory;
		TResourceFactory m_factory;
	};
}