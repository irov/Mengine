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
        typedef IntrusivePtr<Type> TypePtr;

    protected:
        FactoryPtr _initializeFactory() override
        {
            FactoryPtr factory = new FactoryPool<Type, Count>();

            return factory;
        }

	protected:
		PointerFactorable generate() override
		{
            const FactoryPtr & factory = this->getFactory();

            TypePtr object = factory->createObject();

			if( object == nullptr )
			{
				LOGGER_ERROR("DefaultPrototypeGenerator::generate can't generate %s %s"
					, m_category.c_str()
					, m_prototype.c_str()
					);

				return nullptr;
			}

			return object;
		}
	};
}