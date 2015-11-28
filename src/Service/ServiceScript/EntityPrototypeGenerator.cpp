#	include "EntityPrototypeGenerator.h"

#	include "pybind/pybind.hpp"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	EntityPrototypeGenerator::EntityPrototypeGenerator()
		: m_serviceProvider(nullptr)
		, m_count(0)
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	EntityPrototypeGenerator::~EntityPrototypeGenerator()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void EntityPrototypeGenerator::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * EntityPrototypeGenerator::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool EntityPrototypeGenerator::initialize( const ConstString & _category, const ConstString & _prototype, const pybind::object & _generator )
	{
		if( _generator.is_callable() == false )
		{
			return false;
		}

		m_category = _category;
		m_prototype = _prototype;

		m_generator = _generator;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object EntityPrototypeGenerator::preparePythonType()
	{
		if( m_type.is_invalid() == false )
		{
			return m_type;
		}
				
		pybind::object py_type = m_generator( m_prototype );

		if( py_type.is_invalid() == true || py_type.is_none() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("PythonPrototypeGenerator type %s prototype %s invalid type create"
				, m_category.c_str()
				, m_prototype.c_str()
				);

			return pybind::make_invalid_object_t();
		}

		if( py_type.is_type_class() == true )
		{
			PyObject * py_type_ptr = py_type.ptr();

			if( pybind::type_initialize( py_type_ptr ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("PythonPrototypeGenerator prototype %s invalid type initialize"
					, m_category.c_str()
					, m_prototype.c_str()
					);

				return pybind::make_invalid_object_t();
			}
		}

		this->registerEventMethod( EVENT_ENTITY_CREATE, "onCreate", py_type );
		this->registerEventMethod( EVENT_ENTITY_DESTROY, "onDestroy", py_type );

		this->registerEventMethod( EVENT_ENTITY_PREPARATION, "onPreparation", py_type );
		this->registerEventMethod( EVENT_ENTITY_ACTIVATE, "onActivate", py_type );
		this->registerEventMethod( EVENT_ENTITY_PREPARATION_DEACTIVATE, "onPreparationDeactivate", py_type );
		this->registerEventMethod( EVENT_ENTITY_DEACTIVATE, "onDeactivate", py_type );
		this->registerEventMethod( EVENT_ENTITY_COMPILE, "onCompile", py_type );
		this->registerEventMethod( EVENT_ENTITY_RELEASE, "onRelease", py_type );

		m_type = py_type;

		return m_type;
	}
	//////////////////////////////////////////////////////////////////////////
	Factorable * EntityPrototypeGenerator::generate( const ConstString & _category, const ConstString & _prototype )
	{
		(void)_category;
		(void)_prototype;

		const pybind::object & py_type = this->preparePythonType();

		if( py_type.is_invalid() == true )
		{
			return nullptr;
		}

		Entity * entity = SCRIPT_SERVICE(m_serviceProvider)
			->createEntityT<Entity>( _category, m_prototype, py_type, this );

		if( entity == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("EntityPrototypeGenerator can't generate %s %s"
				, m_category.c_str()
				, m_prototype.c_str()
				);

			return nullptr;
		}

		++m_count;

		return entity;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t EntityPrototypeGenerator::count() const
	{
		return m_count;
	}
}