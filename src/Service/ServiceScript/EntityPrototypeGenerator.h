#	pragma once

#	include "Interface/PrototypeManagerInterface.h"
#	include "Interface/ScriptSystemInterface.h"

#	include "Kernel/Eventable.h"
#	include "Kernel/Entity.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class EntityPrototypeGenerator
		: public PrototypeGeneratorInterface
		, public Eventable
	{
	public:
		EntityPrototypeGenerator( ServiceProviderInterface * _serviceProvider, const ConstString & _category, const ConstString & _prototype, PyObject * _module );
		~EntityPrototypeGenerator();

	public:
		PyObject * preparePythonType();

	protected:
		Factorable * generate( const ConstString & _category, const ConstString & _prototype ) override;

	public:
		PyObject * getGenerator() const;

	public:
		void destroy() override;

	protected:
		uint32_t count() const override;

	protected:
		ServiceProviderInterface * m_serviceProvider;
		ConstString m_category;
		ConstString m_prototype;
		PyObject * m_generator;
		PyObject * m_type;

		uint32_t m_count;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<EntityPrototypeGenerator> EntityPrototypeGeneratorPtr;
	
}