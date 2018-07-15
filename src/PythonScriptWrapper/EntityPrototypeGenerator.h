#pragma once

#include "Interface/PrototypeManagerInterface.h"

#include "Kernel/ServantBase.h"
#include "Kernel/Eventable.h"
#include "Kernel/BaseEvent.h"
#include "Kernel/Entity.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class EntityPrototypeGenerator
		: public ServantBase<PrototypeGeneratorInterface>
        , public Eventable
        , public BaseEvent
	{
        DECLARE_EVENTABLE( EntityEventReceiver );

	public:
		EntityPrototypeGenerator();
		~EntityPrototypeGenerator() override;

	public:
		void setScriptGenerator( const pybind::object & _generator );

	public:
		bool initialize( const ConstString & _category, const ConstString & _prototype ) override;

	public:
		pybind::object preparePythonType();

	protected:
		PointerFactorable generate() override;

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
	typedef IntrusivePtr<EntityPrototypeGenerator> EntityPrototypeGeneratorPtr;
	
}