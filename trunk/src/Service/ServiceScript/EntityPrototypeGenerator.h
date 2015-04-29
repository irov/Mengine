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
		EntityPrototypeGenerator();
		~EntityPrototypeGenerator();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );
		ServiceProviderInterface * getServiceProvider() const;

	public:
		bool initialize( const ConstString & _category, const ConstString & _prototype, const pybind::object & _generator );

	public:
		pybind::object preparePythonType();

	protected:
		Factorable * generate( const ConstString & _category, const ConstString & _prototype ) override;

	public:
		PyObject * getGenerator() const;

	protected:
		uint32_t count() const override;

	protected:
		ServiceProviderInterface * m_serviceProvider;
		ConstString m_category;
		ConstString m_prototype;
		pybind::object m_generator;
		pybind::object m_type;

		uint32_t m_count;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<EntityPrototypeGenerator> EntityPrototypeGeneratorPtr;
	
}