#	include "EntityManager.h"
#	include "Entity.h"

#	include <memory>

#	include "LoaderEngine.h"
#	include "ScriptEngine.h"

#	include "BinParser.h"

#	include "Consts.h"
#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	EntityManager::EntityManager()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	EntityManager::~EntityManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void EntityManager::addPrototype( const ConstString & _type, const PrototypeDesc & _desc )
	{
		m_descriptions.insert( std::make_pair(_type, _desc) );
	}
	//////////////////////////////////////////////////////////////////////////
	bool EntityManager::getPrototypeDesc( const ConstString & _type, PrototypeDesc & _desc )
	{
		TMapDescriptionEntities::const_iterator it_found = m_descriptions.find( _type );

		if( it_found == m_descriptions.end() )
		{
			return false;
		}

		_desc = it_found->second;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * EntityManager::createEntity( const ConstString & _name, const ConstString & _prototype )
	{
		TMapDescriptionEntities::iterator it_find = m_descriptions.find( _prototype );

		if( it_find == m_descriptions.end() )
		{
			MENGE_LOG_ERROR( "EntityManager: Entity '%s''%s' declaration not found"
				, _name.c_str()
				, _prototype.c_str() 
				);

			return 0;
		}

		const PrototypeDesc & desc = it_find->second;
		const ConstString & type = Consts::get()->c_Entity;

		Entity * entity = ScriptEngine::get()
			->createEntityT<Entity>( _prototype, type, desc.pak, desc.path );

		if( entity == 0 )
		{
			MENGE_LOG_ERROR( "EntityManager: Can't create entity '%s''%s'"
				, _name.c_str()
				, _prototype.c_str() 
				); 

			return 0;
		}

		entity->setName( _name );
		entity->setType( type );
		entity->setPrototype( _prototype );

		if( this->setupEntityDesk_( entity, desc ) == false )
		{
			MENGE_LOG_ERROR( "EntityManager: Can't setup entity '%s''%s'"
				, _name.c_str()
				, _prototype.c_str() 
				); 

			entity->destroy();

			return 0;
		}

		//entity->callMethod( "onLoader", "()" );

		return entity;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class EntityLoadable
			: public Loadable
		{
		public:
			EntityLoadable( Entity * _entity )
				: m_entity(_entity)
			{

			}

		protected:
			void loader( BinParser * _parser ) override
			{
				BIN_SWITCH_ID( _parser )
				{
					BIN_CASE_NODE_PARSE( Protocol::Entity, m_entity );
				}
			}

		protected:
			void loaded() override
			{
				m_entity->loaded();
			}

		protected:
			Entity * m_entity;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	bool EntityManager::setupEntityDesk_( Entity * _entity, const PrototypeDesc & _desc )
	{
		const ConstString & prototype = _entity->getPrototype();

		TMapEntitiesData::iterator it_found = m_entitiesData.find( prototype );

		if( it_found == m_entitiesData.end() )
		{
			const ConstString & prototype = _entity->getPrototype();

			TBlobject buff;

			if( this->loadEntityData_( prototype, _desc, buff ) == false )
			{
				return false;
			}

			it_found = m_entitiesData.insert( std::make_pair( prototype, buff ) ).first;
		}

		const TBlobject & data = it_found->second;

		std::auto_ptr<EntityLoadable> loadable( new EntityLoadable(_entity) );

		if( LoaderEngine::get()
			->loadBinary( data, loadable.get() ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool EntityManager::loadEntityData_( const ConstString & _prototype, const PrototypeDesc & _desc, TBlobject & _data )
	{
		String data_path = _desc.path.str();
		data_path += "/";
		data_path += _prototype.str();
		data_path += "/";
		data_path += _prototype.str();

		if( LoaderEngine::get()
			->import( _desc.pak, data_path, _data ) == false )
		{
			return false;
		}

		return true;
	}
}