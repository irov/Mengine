#pragma once

#include "Interface/PrototypeManagerInterface.h"
#include "Interface/ScriptSystemInterface.h"

#include "Kernel/Node.h"

#include "Core/ServantBase.h"
#include "Core/ConstString.h"
#include "Core/MemoryAllocator.h"

#include "Logger/Logger.h"

namespace Mengine
{
	class BasePrototypeGenerator
		: public ServantBase<PrototypeGeneratorInterface>
		, public MemoryAllocator<BasePrototypeGenerator>
	{
	public:
		BasePrototypeGenerator();

	protected:
		bool initialize( const ConstString & _category, const ConstString & _prototype ) override;

    protected:
        virtual bool _initialize();

    protected:
		void destroy() override;

	protected:
		ConstString m_category;
		ConstString m_prototype;

		ScriptWrapperInterfacePtr m_scriptWrapper;
	};
}