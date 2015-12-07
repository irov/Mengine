#	pragma once

#	include "Interface/PrototypeManagerInterface.h"

#	include "Kernel/Node.h"

#	include "Core/ConstString.h"
#   include "Core/MemoryAllocator.h"

#	include "Factory/FactoryStore.h"

#	include "Logger/Logger.h"

namespace Menge
{
	template<class Type, uint32_t Count>
	class NodePrototypeGenerator
		: public PrototypeGeneratorInterface
		, public MemoryAllocator
	{
	public:
		NodePrototypeGenerator()
			: m_serviceProvider(nullptr)
			, m_scriptWrapper(nullptr)
		{
		}

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override
		{
			m_serviceProvider = _serviceProvider;
		}

		ServiceProviderInterface * getServiceProvider() const override
		{
			return m_serviceProvider;
		}

	protected:
		bool initialize( const ConstString & _category, const ConstString & _prototype ) override
		{
			m_category = _category;
			m_prototype = _prototype;

			m_scriptWrapper = SCRIPT_SERVICE( m_serviceProvider )
				->getWrapper( m_prototype );

			return true;
		}

	protected:
		Factorable * generate() override
		{
			Node * node = m_factory.createObject();

			if( node == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("NodePrototypeGenerator::generate can't generate %s %s"
					, m_category.c_str()
					, m_prototype.c_str()
					);

				return nullptr;
			}

			node->setServiceProvider( m_serviceProvider );
			node->setType( m_prototype );
			node->setScriptWrapper( m_scriptWrapper );

			return node;
		}

		uint32_t count() const override
		{
			uint32_t count = m_factory.countObject();

			return count;
		}

		void destroy() override
		{
			delete this;
		}

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConstString m_category;
		ConstString m_prototype;

		ScriptWrapperInterface * m_scriptWrapper;

		typedef FactoryPoolStore<Type, Count> TNodeFactory;
		TNodeFactory m_factory;
	};
}