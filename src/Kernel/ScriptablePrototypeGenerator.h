#pragma once

#include "BasePrototypeGenerator.h"

#include "Kernel/Scriptable.h"

#include "Core/ConstString.h"
#include "Core/MemoryAllocator.h"

#include "Factory/FactoryPool.h"

#include "Logger/Logger.h"

namespace Mengine
{
	template<class Type, uint32_t Count>
	class ScriptablePrototypeGenerator
		: public BasePrototypeGenerator
	{
        typedef IntrusivePtr<Type> TypePtr;

    protected:
        bool _initialize() override
        {
            m_factory = new FactoryPool<Type, Count>();

            return true;
        }

	protected:
		PointerFactorable generate() override
		{
            TypePtr scriptable = m_factory->createObject();

			if( scriptable == nullptr )
			{
				LOGGER_ERROR("ScriptablePrototypeGenerator::generate can't generate %s %s"
					, m_category.c_str()
					, m_prototype.c_str()
					);

				return nullptr;
			}

			scriptable->setScriptWrapper( m_scriptWrapper );

			return scriptable;
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