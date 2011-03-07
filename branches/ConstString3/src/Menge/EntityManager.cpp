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
			->createEntityT<Entity>( _name, type, type, _prototype, desc.pak, desc.path );

		if( entity == 0 )
		{
			MENGE_LOG_ERROR( "EntityManager: Can't create entity '%s''%s'"
				, _name.c_str()
				, _prototype.c_str() 
				); 

			return 0;
		}

		if( this->setupEntityDesk_( entity, desc ) == false )
		{
			MENGE_LOG_ERROR( "EntityManager: Can't setup entity '%s''%s'"
				, _name.c_str()
				, _prototype.c_str() 
				); 

			entity->destroy();

			return 0;
		}

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
			void _loaded() override
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
			TBlobject buff;

			if( this->loadEntityData_( prototype, _desc, buff ) == false )
			{
				return false;
			}

			it_found = m_entitiesData.insert( std::make_pair( prototype, buff ) ).first;
		}

		const TBlobject & data = it_found->second;

		if( data.empty() == true )
		{
			return true;
		}

		EntityLoadable loadable(_entity);

		if( LoaderEngine::get()
			->loadBinary( data, &loadable ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool EntityManager::loadEntityData_( const ConstString & _prototype, const PrototypeDesc & _desc, TBlobject & _data )
	{
		String data_path = Helper::to_str(_desc.path);
		data_path += "/";
		data_path += Helper::to_str(_prototype);
		data_path += "/";
		data_path += Helper::to_str(_prototype);

		if( LoaderEngine::get()
			->import( _desc.pak, data_path, _data ) == false )
		{
			return false;
		}

		return true;
	}
}