#	pragma once

#	include "Interface/PrototypeManagerInterface.h"

#	include "Kernel/Node.h"

#	include "Core/ServantBase.h"
#	include "Core/ConstString.h"
#   include "Core/MemoryAllocator.h"

#	include "Factory/FactoryStore.h"

#	include "Logger/Logger.h"

namespace Menge
{
	class BasePrototypeGenerator
		: public ServantBase<PrototypeGeneratorInterface>
		, public MemoryAllocator
	{
	public:
		BasePrototypeGenerator();

	protected:
		bool initialize( const ConstString & _category, const ConstString & _prototype ) override;

		void destroy() override;

	protected:
		ConstString m_category;
		ConstString m_prototype;

		ScriptWrapperInterface * m_scriptWrapper;
	};
}