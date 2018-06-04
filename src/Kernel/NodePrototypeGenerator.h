#pragma once

#include "ScriptablePrototypeGenerator.h"

#include "Kernel/Node.h"

#include "Core/ConstString.h"
#include "Core/MemoryAllocator.h"

#include "Factory/FactoryPool.h"

#include "Logger/Logger.h"

namespace Mengine
{
	template<class Type, uint32_t Count>
	class NodePrototypeGenerator
		: public ScriptablePrototypeGenerator<Type, Count>
	{
	protected:
		PointerFactorable generate() override
		{
            const FactoryPtr & factory = this->getFactory();

            NodePtr node = factory->createObject();

			if( node == nullptr )
			{
				LOGGER_ERROR("NodePrototypeGenerator::generate can't generate %s %s"
					, m_category.c_str()
					, m_prototype.c_str()
					);

				return nullptr;
			}

			node->setType( m_prototype );

            this->setupScriptable( node );

			return node;
		}
	};
}
