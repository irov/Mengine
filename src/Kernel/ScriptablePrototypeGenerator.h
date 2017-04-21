#	pragma once

#	include "BasePrototypeGenerator.h"

#	include "Kernel/Scriptable.h"

#	include "Core/ConstString.h"
#   include "Core/MemoryAllocator.h"

#	include "Factory/FactoryPool.h"

#	include "Logger/Logger.h"

namespace Menge
{
	template<class Type, uint32_t Count>
	class ScriptablePrototypeGenerator
		: public BasePrototypeGenerator
	{
    protected:
        bool _initialize() override
        {
            m_factory = new FactoryPool<Type, Count>( m_serviceProvider );

            return true;
        }

	protected:
		PointerFactorable generate() override
		{
			Type * scriptable = m_factory->createObject();

			if( scriptable == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("ScriptablePrototypeGenerator::generate can't generate %s %s"
					, m_category.c_str()
					, m_prototype.c_str()
					);

				return nullptr;
			}

			scriptable->setServiceProvider( m_serviceProvider );
			scriptable->setScriptWrapper( m_scriptWrapper );

			return scriptable;
		}

		uint32_t count() const override
		{
			uint32_t count = m_factory->countObject();

			return count;
		}

	protected:
		FactoryPtr m_factory;
	};
}