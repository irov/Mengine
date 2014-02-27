#	pragma once

#	include "Interface/PrototypeManagerInterface.h"

#	include "Kernel/Node.h"

#	include "Core/ConstString.h"
#	include "Factory/FactoryStore.h"

#	include "Logger/Logger.h"

namespace Menge
{
	template<class Type, size_t Count>
	class NodePrototypeGenerator
		: public PrototypeGeneratorInterface
	{
	public:
		NodePrototypeGenerator( ServiceProviderInterface * _serviceProvider )
			: m_serviceProvider(_serviceProvider)
		{
		}

	protected:
		Factorable * generate( const ConstString & _category, const ConstString & _prototype ) override
		{
			Node * node = m_factory.createObjectT();

			if( node == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("NodePrototypeGenerator::generate can't generate %s %s"
					, _category.c_str()
					, _prototype.c_str()
					);

				return nullptr;
			}

			node->setServiceProvider( m_serviceProvider );
			node->setType( _prototype );                

			return node;
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

		typedef FactoryPoolStore<Type, Count> TNodeFactory;
		TNodeFactory m_factory;
	};
}