#	include "EntityManager.h"
#	include "Entity.h"

#	include "XmlEngine.h"
#	include "ScriptEngine.h"

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
		for( TMapEntities::iterator
			it = m_entities.begin(),
			it_end = m_entities.end();
		it != it_end;
		++it)
		{
			it->second->destroy();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void EntityManager::registerEntityType( const ConstString & _type, const EntityDesc & _desc )
	{
		m_descriptions.insert( std::make_pair(_type, _desc) );
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * EntityManager::createEntity( const ConstString & _type )
	{
		TMapEntities::iterator it_find = m_entities.find( _type );

		if( it_find == m_entities.end() )
		{
			Entity * entity = this->createEntity_( _type );

			it_find = m_entities.insert( std::make_pair( _type, entity ) ).first;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * EntityManager::createEntity_( const ConstString & _type )
	{
		TMapDescriptionEntities::iterator it_find = m_descriptions.find( _type );

		if( it_find == m_descriptions.end() )
		{
			MENGE_LOG_ERROR( "EntityManager: Entity '%s' declaration not found"
				, _type.c_str() 
				);

			return 0;
		}

		const EntityDesc & desc = it_find->second;

		Entity * entity = ScriptEngine::get()
			->createNodeT<Entity>( _type, _type );

		if( entity == 0 )
		{
			MENGE_LOG_ERROR( "EntityManager: Can't create entity '%s'"
				, _type.c_str() 
				); 

			return 0;
		}

		if( this->setupEntity_( desc, entity ) == false )
		{
			MENGE_LOG_ERROR( "EntityManager: Can't setup entity '%s'"
				, _type.c_str() 
				); 

			entity->destroy();

			return 0;
		}

		return entity;
	}
	//////////////////////////////////////////////////////////////////////////
	bool EntityManager::setupEntity_( const EntityDesc & _desc, Entity * _entity )
	{
		const ConstString & type = _entity->getType();

		TMapEntitiesData::iterator it_data = this->getEntityData_( type, _desc );

		if( it_data == m_mapEntitiesData.end() )
		{
			return false;
		}

		const TBlobject & buffer = it_data->second;

		if( XmlEngine::get()
			->parseXmlBufferM( &buffer.front(), buffer.size(), _entity, &Entity::loader ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	EntityManager::TMapEntitiesData::iterator EntityManager::getEntityData_( const ConstString & _type, const EntityDesc & _desc )
	{
		TMapEntitiesData::iterator it_found = m_mapEntitiesData.find( _type );

		if( it_found != m_mapEntitiesData.end() )
		{
			return it_found;
		}

		String xml_path = _desc.path;
		xml_path += "/";
		xml_path += _type.str();
		xml_path += ".xml";

		FileInputInterface* file = FileEngine::get()
			->openInputFile( _desc.pak, xml_path );

		if( file == 0 )
		{
			MENGE_LOG_INFO("EntityManager: failed open xml file %s"
				, xml_path.c_str()
				);

			return m_mapEntitiesData.end();
		}

		TMapEntitiesData::iterator it_insert =
			m_mapEntitiesData.insert( std::make_pair( _type, TBlobject() ) ).first;

		std::streamsize size = file->size();
		TBlobject & blob = it_insert->second;

		blob.resize( size );
		file->read( &blob[0], size );

		file->close();

		return it_insert;
	}
}