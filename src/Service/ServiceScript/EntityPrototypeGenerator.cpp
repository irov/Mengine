#	include "EntityPrototypeGenerator.h"

#   include "Interface/StringizeInterface.h"

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
	void EntityPrototypeGenerator::setScriptGenerator( const pybind::object & _generator )
	{
		m_generator = _generator;
	}
	//////////////////////////////////////////////////////////////////////////
	bool EntityPrototypeGenerator::initialize( const ConstString & _category, const ConstString & _prototype )
	{
		m_category = _category;
		m_prototype = _prototype;

		if( m_generator.is_callable() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	pybind::object EntityPrototypeGenerator::preparePythonType()
	{
		if( m_type.is_invalid() == false )
		{
			return m_type;
		}
				
		pybind::object py_type = m_generator.call( m_prototype );

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
	Factorable * EntityPrototypeGenerator::generate()
	{
		const pybind::object & py_type = this->preparePythonType();

		if( py_type.is_invalid() == true )
		{
			return nullptr;
		}

		pybind::object py_entity = py_type.call();

		if( py_entity.is_invalid() == true )
		{
			LOGGER_ERROR( m_serviceProvider )("EntityPrototypeGenerator.generate: can't create object '%s' '%s' (invalid create)"
				, m_category.c_str()
				, m_prototype.c_str()
				);

			return nullptr;
		}

		Entity * entity = nullptr;

		if( py_entity.is_class() == true )
		{
			entity = py_entity.extract();
		}
		else
		{
			entity = PROTOTYPE_SERVICE( m_serviceProvider )
				->generatePrototypeT<Entity *>( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Node" ), m_category );
		}

		if( entity == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("EntityPrototypeGenerator can't generate '%s' '%s'"
				, m_category.c_str()
				, m_prototype.c_str()
				);

			return nullptr;
		}

		entity->setServiceProvider( m_serviceProvider );
		entity->setType( m_category );
		entity->setPrototype( m_prototype );

		entity->setScriptEventable( this );
		entity->setScriptObject( py_entity );

		entity->onCreate();

		++m_count;

		return entity;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t EntityPrototypeGenerator::count() const
	{
		return m_count;
	}
}