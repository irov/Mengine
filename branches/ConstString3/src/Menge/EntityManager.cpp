#	include "EntityManager.h"
#	include "Entity.h"

#	include "XmlEngine.h"
#	include "ScriptEngine.h"

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
	void EntityManager::registerEntityType( const ConstString & _type, const EntityDesc & _desc )
	{
		m_descriptions.insert( std::make_pair(_type, _desc) );
	}
	//////////////////////////////////////////////////////////////////////////
	Entity * EntityManager::createEntity( const ConstString & _type )
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
			->createNodeT<Entity>( _type, Consts::get()->c_builtin_empty, _type );

		if( entity == 0 )
		{
			MENGE_LOG_ERROR( "EntityManager: Can't create entity '%s'"
				, _type.c_str() 
				); 

			return 0;
		}

		if( this->setupEntity_( entity, desc ) == false )
		{
			MENGE_LOG_ERROR( "EntityManager: Can't setup entity '%s'"
				, _type.c_str() 
				); 

			entity->destroy();

			return 0;
		}

		entity->callMethod( "onLoader", "()" );

		return entity;
	}
	//////////////////////////////////////////////////////////////////////////
	bool EntityManager::setupEntity_( Entity * _entity, const EntityDesc & _desc )
	{
		const ConstString & type = _entity->getType();

		const TBlobject * data = this->getEntityData_( type, _desc );

		if( data == 0 )
		{
			return false;
		}

		if( data->empty() == true )
		{
			return true;
		}

		return this->setupEntity_( _entity, &data->front(), data->size() );
	}
	//////////////////////////////////////////////////////////////////////////
	bool EntityManager::setupEntity_( Entity * _entity, const TBlobject::value_type * _buffer, TBlobject::size_type _size )
	{
		if( XmlEngine::get()
			->parseXmlBufferM( _buffer, _size, _entity, &Entity::loader ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const TBlobject * EntityManager::getEntityData_( const ConstString & _type, const EntityDesc & _desc )
	{
		TMapEntitiesData::iterator it_found = m_mapEntitiesData.find( _type );

		if( it_found != m_mapEntitiesData.end() )
		{
			return &it_found->second;
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

			return 0;
		}

		TMapEntitiesData::iterator it_insert =
			m_mapEntitiesData.insert( std::make_pair( _type, TBlobject() ) ).first;

		std::streamsize size = file->size();
		TBlobject & blob = it_insert->second;

		blob.resize( size );
		file->read( &blob[0], size );

		file->close();

		return &it_insert->second;
	}
}