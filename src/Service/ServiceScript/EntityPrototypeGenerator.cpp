#	include "EntityPrototypeGenerator.h"

#	include "pybind/pybind.hpp"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	EntityPrototypeGenerator::EntityPrototypeGenerator()
		: m_serviceProvider(nullptr)
		, m_generator(nullptr)
		, m_type(nullptr)
		, m_count(0)
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	EntityPrototypeGenerator::~EntityPrototypeGenerator()
	{
		pybind::decref( m_generator );
		pybind::decref( m_type );
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
	bool EntityPrototypeGenerator::initialize( const ConstString & _category, const ConstString & _prototype, PyObject * _generator )
	{
		if( pybind::is_callable(_generator) == false )
		{
			return false;
		}

		m_category = _category;
		m_prototype = _prototype;

		m_generator = _generator;
		pybind::incref( m_generator );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject * EntityPrototypeGenerator::preparePythonType()
	{
		if( m_type != nullptr )
		{
			return m_type;
		}

		PyObject * p = pybind::ptr(m_prototype);
		
		PyObject * py_type = pybind::ask( m_generator, "(O)"
			, p
			);

		//pybind::decref( p );

		if( py_type == nullptr || pybind::is_none( py_type ) == true )
		{
			LOGGER_ERROR(m_serviceProvider)("PythonPrototypeGenerator type %s prototype %s invalid type create"
				, m_category.c_str()
				, m_prototype.c_str()
				);

			return nullptr;
		}

		if( pybind::type_initialize( py_type ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("PythonPrototypeGenerator prototype %s invalid type initialize"
				, m_category.c_str()
				, m_prototype.c_str()
				);

			return nullptr;
		}

		this->registerEventMethod( EVENT_CREATE, "onCreate", py_type );
		this->registerEventMethod( EVENT_DESTROY, "onDestroy", py_type );

		this->registerEventMethod( EVENT_PREPARATION, "onPreparation", py_type );
		this->registerEventMethod( EVENT_ACTIVATE, "onActivate", py_type );
		this->registerEventMethod( EVENT_PREPARATION_DEACTIVATE, "onPreparationDeactivate", py_type );
		this->registerEventMethod( EVENT_DEACTIVATE, "onDeactivate", py_type );
		this->registerEventMethod( EVENT_COMPILE, "onCompile", py_type );
		this->registerEventMethod( EVENT_RELEASE, "onRelease", py_type );

		m_type = py_type;
		pybind::incref( m_type );

		return py_type;
	}
	//////////////////////////////////////////////////////////////////////////
	Factorable * EntityPrototypeGenerator::generate( const ConstString & _category, const ConstString & _prototype )
	{
		(void)_category;
		(void)_prototype;

		PyObject * py_type = this->preparePythonType();

		if( py_type == nullptr )
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