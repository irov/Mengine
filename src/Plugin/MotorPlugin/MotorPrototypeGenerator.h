#	pragma once

#	include "Kernel/BasePrototypeGenerator.h"

#	include "Motor.h"

#	include "Core/ConstString.h"
#   include "Core/MemoryAllocator.h"

#	include "Factory/FactoryStore.h"

#	include "Logger/Logger.h"

namespace Menge
{
	template<class Type, uint32_t Count>
	class MotorPrototypeGenerator
		: public BasePrototypeGenerator
	{
	protected:
		Factorable * generate() override
		{
			Motor * motor = m_factory.createObject();

			if(node == nullptr)
			{
				LOGGER_ERROR(m_serviceProvider)("MotorPrototypeGenerator::generate can't generate %s %s"
					, m_category.c_str()
					, m_prototype.c_str()
					);

				return nullptr;
			}

			motor->setServiceProvider(m_serviceProvider);
			motor->setScriptWrapper(m_scriptWrapper);

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