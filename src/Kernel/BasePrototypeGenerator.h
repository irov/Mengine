#	pragma once

#	include "Interface/PrototypeManagerInterface.h"

#	include "Kernel/Node.h"

#	include "Core/ConstString.h"
#   include "Core/MemoryAllocator.h"

#	include "Factory/FactoryStore.h"

#	include "Logger/Logger.h"

namespace Menge
{
	class BasePrototypeGenerator
		: public PrototypeGeneratorInterface
		, public MemoryAllocator
	{
	public:
		BasePrototypeGenerator();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	protected:
		bool initialize( const ConstString & _category, const ConstString & _prototype ) override;

		void destroy() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConstString m_category;
		ConstString m_prototype;

		ScriptWrapperInterface * m_scriptWrapper;
	};
}