#pragma once

#include "BasePrototypeGenerator.h"

#include "Core/ConstString.h"

#include "Factory/FactoryPool.h"

#include "Logger/Logger.h"

namespace Mengine
{
	template<class Type, uint32_t Count>
	class DefaultPrototypeGenerator
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
			Type * object = m_factory->createObject();

			if( object == nullptr )
			{
				LOGGER_ERROR("DefaultPrototypeGenerator::generate can't generate %s %s"
					, m_category.c_str()
					, m_prototype.c_str()
					);

				return nullptr;
			}

			object->setType( m_prototype );
			object->setScriptWrapper( m_scriptWrapper );

			return object;
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