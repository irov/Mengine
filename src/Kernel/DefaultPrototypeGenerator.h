#	pragma once

#	include "BasePrototypeGenerator.h"

#	include "Core/ConstString.h"

#	include "Factory/FactoryStore.h"

#	include "Logger/Logger.h"

namespace Menge
{
	template<class Type, uint32_t Count>
	class DefaultPrototypeGenerator
		: public BasePrototypeGenerator
	{
	protected:
		Factorable * generate() override
		{
			Type * object = m_factory.createObject();

			if( object == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("DefaultPrototypeGenerator::generate can't generate %s %s"
					, m_category.c_str()
					, m_prototype.c_str()
					);

				return nullptr;
			}

			object->setServiceProvider( m_serviceProvider );
			object->setType( m_prototype );
			object->setScriptWrapper( m_scriptWrapper );

			return object;
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