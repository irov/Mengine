#pragma once

#include "BasePrototypeGenerator.h"

#include "Kernel/Node.h"

#include "Core/ConstString.h"
#include "Core/MemoryAllocator.h"

#include "Factory/FactoryPool.h"

#include "Logger/Logger.h"

namespace Mengine
{
	template<class Type, uint32_t Count>
	class NodePrototypeGenerator
		: public BasePrototypeGenerator
	{
    protected:
        bool _initialize() override
        {
            m_factory = new FactoryPool<Type, Count>();

            return true;
        }

	protected:
		PointerFactorable generate() override
		{
            IntrusivePtr<Type> node = m_factory->createObject();

			if( node == nullptr )
			{
				LOGGER_ERROR("NodePrototypeGenerator::generate can't generate %s %s"
					, m_category.c_str()
					, m_prototype.c_str()
					);

				return nullptr;
			}

			node->setType( m_prototype );
			node->setScriptWrapper( m_scriptWrapper );

			return node;
		}

		uint32_t count() const override
		{
			uint32_t count = m_factory->getCountObject();

			return count;
		}

	protected:
        FactoryPtr m_factory;
	};
}
