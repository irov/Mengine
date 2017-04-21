#	pragma once

#	include "BasePrototypeGenerator.h"

#	include "Kernel/Surface.h"

#	include "Core/ConstString.h"
#   include "Core/MemoryAllocator.h"

#	include "Factory/FactoryPool.h"

#	include "Logger/Logger.h"

namespace Menge
{
	template<class Type, uint32_t Count>
	class SurfacePrototypeGenerator
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
			Surface * surface = m_factory->createObject();

			if( surface == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("NodePrototypeGenerator::generate can't generate %s %s"
					, m_category.c_str()
					, m_prototype.c_str()
					);

				return nullptr;
			}

			surface->setServiceProvider( m_serviceProvider );
			surface->setType( m_prototype );
			surface->setScriptWrapper( m_scriptWrapper );

			return surface;
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