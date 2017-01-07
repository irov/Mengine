#	pragma once

#	include "Interface/PrototypeManagerInterface.h"

#   include "Core/ServantBase.h"

#	include "Kernel/Eventable.h"
#	include "Kernel/Entity.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class EntityPrototypeGenerator
		: public ServantBase<PrototypeGeneratorInterface>
		, public Eventable
	{
	public:
		EntityPrototypeGenerator();
		~EntityPrototypeGenerator();

	public:
		void setScriptGenerator( const pybind::object & _generator );

	public:
		bool initialize( const ConstString & _category, const ConstString & _prototype ) override;

	public:
		pybind::object preparePythonType();

	protected:
		Factorable * generate() override;

	public:
		PyObject * getGenerator() const;

	protected:
		uint32_t count() const override;

	protected:
		ConstString m_category;
		ConstString m_prototype;
		pybind::object m_generator;
		pybind::object m_type;

		uint32_t m_count;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<EntityPrototypeGenerator> EntityPrototypeGeneratorPtr;
	
}