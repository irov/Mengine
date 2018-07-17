#pragma once

#include "BasePrototypeGenerator.h"

#include "Kernel/ConstString.h"

#include "Kernel/FactoryPool.h"

#include "Kernel/Logger.h"

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
					, this->getCategory().c_str()
					, this->getPrototype().c_str()
					);

				return nullptr;
			}

			return object;
		}
	};
}