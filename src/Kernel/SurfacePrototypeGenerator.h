#pragma once

#include "ScriptablePrototypeGenerator.h"

#include "Kernel/Surface.h"

#include "Core/ConstString.h"
#include "Core/MemoryAllocator.h"

#include "Factory/FactoryPool.h"

#include "Logger/Logger.h"

namespace Mengine
{
	template<class Type, uint32_t Count>
	class SurfacePrototypeGenerator
		: public ScriptablePrototypeGenerator<Type, Count>
	{
	protected:
		PointerFactorable generate() override
		{
            const FactoryPtr & factory = this->getFactory();

			SurfacePtr surface = factory->createObject();

			if( surface == nullptr )
			{
				LOGGER_ERROR("NodePrototypeGenerator::generate can't generate %s %s"
					, this->getCategory().c_str()
					, this->getPrototype().c_str()
					);

				return nullptr;
			}

            const ConstString & prototype = this->getPrototype();
			surface->setType( prototype );

            this->setupScriptable( surface );

			return surface;
		}
	};
}