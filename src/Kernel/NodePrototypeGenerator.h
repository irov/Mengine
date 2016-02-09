#	pragma once

#	include "BasePrototypeGenerator.h"

#	include "Kernel/Node.h"

#	include "Core/ConstString.h"
#   include "Core/MemoryAllocator.h"

#	include "Factory/FactoryStore.h"

#	include "Logger/Logger.h"

namespace Menge
{
	template<class Type, uint32_t Count>
	class NodePrototypeGenerator
		: public BasePrototypeGenerator
	{
	protected:
		Factorable * generate() override
		{
			Node * node = m_factory.createObject();

			if( node == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("NodePrototypeGenerator::generate can't generate %s %s"
					, m_category.c_str()
					, m_prototype.c_str()
					);

				return nullptr;
			}

			node->setServiceProvider( m_serviceProvider );
			node->setType( m_prototype );
			node->setScriptWrapper( m_scriptWrapper );

			return node;
		}

		uint32_t count() const override
		{
			uint32_t count = m_factory.countObject();

			return count;
		}

	protected:
		typedef FactoryPoolStore<Type, Count> TNodeFactory;
		TNodeFactory m_factory;
	};
}