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
	void EntityManager::registerEntityType( const ConstString & _name, const EntityDesc & _desc )
	{
		m_descriptions.insert( std::make_pair(_name, _desc) );
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * EntityManager::createEntity( const ConstString & _name )
	{
		TMapEntities::iterator it_find = m_entities.find( _name );

		if( it_find == m_entities.end() )
		{
			Entity * entity = this->createEntity_( _name );

			it_find = m_entities.insert( std::make_pair( _name, entity ) ).first;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	void EntityManager::removeEntity( const ConstString& _name )
	{
		//TMapArrows::iterator it_find = m_arrows.find( _name );

		//if( it_find == m_arrows.end() )
		//{
		//	MENGE_LOG_ERROR( "Can't find arrow '%s' to remove"
		//		, _name.c_str() 
		//		); 

		//	return;
		//}

		//it_find->second->destroy();

		//m_arrows.erase( it_find );
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * EntityManager::createEntity_( const ConstString & _name )
	{
		TMapDescriptionEntities::iterator it_find = m_descriptions.find( _name );

		if( it_find == m_descriptions.end() )
		{
			MENGE_LOG_ERROR( "EntityManager: Entity '%s' declaration not found"
				, _name.c_str() 
				);

			return 0;
		}

		const EntityDesc & desc = it_find->second;

		Entity * entity = ScriptEngine::get()
			->createEntity( _name );

		if( entity == 0 )
		{
			MENGE_LOG_ERROR( "EntityManager: Can't create entity '%s'"
				, _name.c_str() 
				); 

			return 0;
		}

		entity->setName( _name );

		if( this->setupEntity_( desc, entity ) == false )
		{
			MENGE_LOG_ERROR( "EntityManager: Can't setup entity '%s'"
				, _name.c_str() 
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

		const Blobject & buffer = it_data->second;

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
			m_mapEntitiesData.insert( std::make_pair( _type, Blobject() ) ).first;

		std::streamsize size = file->size();
		Blobject & blob = it_insert->second;

		blob.resize( size );
		file->read( &blob[0], size );

		file->close();

		return it_insert;
	}
}