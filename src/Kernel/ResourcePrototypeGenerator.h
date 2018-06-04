#pragma once

#include "ScriptablePrototypeGenerator.h"

#include "Kernel/Resource.h"

#include "Core/ConstString.h"
#include "Factory/FactoryPool.h"

#include "Logger/Logger.h"

namespace Mengine
{
	template<class Type, uint32_t Count>
	class ResourcePrototypeGenerator
		: public ScriptablePrototypeGenerator<Type, Count>
	{
	protected:
		PointerFactorable generate() override
		{
            const FactoryPtr & factory = this->getFactory();

			ResourcePtr resource = factory->createObject();

			if( resource == nullptr )
			{
				LOGGER_ERROR("ResourcePrototypeGenerator can't generate %s %s"
					, this->getCategory().c_str()
					, this->getPrototype().c_str()
					);

				return nullptr;
			}

            const ConstString & prototype = this->getPrototype();
			resource->setType( prototype );

            this->setupScriptable( resource );

			return resource;
		}
	};
}