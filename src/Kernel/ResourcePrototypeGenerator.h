#	pragma once

#	include "BasePrototypeGenerator.h"

#	include "Kernel/ResourceReference.h"

#	include "Core/ConstString.h"
#	include "Factory/FactoryStore.h"

#	include "Logger/Logger.h"

namespace Menge
{
	template<class Type, uint32_t Count>
	class ResourcePrototypeGenerator
		: public BasePrototypeGenerator
	{
	protected:
		Factorable * generate() override
		{
			Type * resource = m_factory.createObject();

			if( resource == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourcePrototypeGenerator can't generate %s %s"
					, m_category.c_str()
					, m_prototype.c_str()
					);

				return nullptr;
			}

			resource->setServiceProvider( m_serviceProvider );
			resource->setType( m_prototype );
			resource->setScriptWrapper( m_scriptWrapper );

			if( this->setup( resource ) == false )
			{
				return nullptr;
			}
			
			return resource;
		}

		virtual bool setup( Type * _resource )
		{
			(void)_resource;
			//Empty			

			return true;
		}
		
		uint32_t count() const override
		{
			uint32_t count = m_factory.countObject();

			return count;
		}
		
	protected:
		typedef FactoryPoolStore<Type, Count> TResourceFactory;
		TResourceFactory m_factory;
	};
}