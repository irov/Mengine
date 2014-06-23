#	include "AstralaxParticleSystem.h"

#   include "Interface/StringizeInterface.h"

#	include "Core/CacheMemoryBuffer.h"
#   include "Core/String.h"

#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ParticleSystem, Menge::ParticleSystemInterface, Menge::AstralaxParticleSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxParticleSystem::AstralaxParticleSystem()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AstralaxParticleSystem::~AstralaxParticleSystem()
	{		
        Magic_DestroyAll();
	}
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticleSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * AstralaxParticleSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticleSystem::initialize()
	{
		//Magic_SetAxis( MAGIC_pXpYpZ );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticleSystem::finalize()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	static size_t s_getCountTag( const String & _fullname, String & _name )
	{
		String::size_type st_begin = _fullname.find_first_of('[');

		if( st_begin == String::npos )
		{
			_name = _fullname;

			return 0;
		}

		int count;

		String::size_type st_end = _fullname.find_first_of(']');

		String num = _fullname.substr( st_begin + 1, st_end - st_begin - 1 );

        if( Utils::stringToInt( num, count ) == false )
        {
            return 0;
        }

		_name = _fullname.substr( 0, st_begin );

		return count;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticleSystem::loadEmitters_( HM_FILE _file, const AstralaxEmitterContainerPtr & _container )
	{
		MAGIC_FIND_DATA find;
		const char* magicName = Magic_FindFirst( _file, &find, MAGIC_FOLDER | MAGIC_EMITTER );

		while( magicName != nullptr )
		{
			if( find.animate == 1 )
			{
				if( this->loadEmitter( magicName, _file, _container ) == false )
				{
					LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::loadEmitters_ invalid load emitter %s"
						, magicName
						);

					return false;
				}
			}
			else
			{		
				if( this->loadEmittersFolder( magicName, _file, _container ) == false )
				{
					LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::loadEmitters_ invalid load animate emitter %s"
						, magicName
						);

					return false;
				}
			}

			magicName = Magic_FindNext( _file, &find );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticleSystem::loadEmittersFolder( const char * _path, HM_FILE _file, const AstralaxEmitterContainerPtr & _container )
	{
		Magic_SetCurrentFolder( _file, _path );
		
		bool result = this->loadEmitters_( _file, _container );
			
		Magic_SetCurrentFolder( _file, ".." );

		if( result == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::loadEmittersFolder invalid load emitters %s"
				, _path
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticleSystem::loadEmitter( const char * _magicName, HM_FILE _file, const AstralaxEmitterContainerPtr & _container )
	{		
        m_loadEmitterCacheFullname = _magicName;
		
        HM_EMITTER id = Magic_LoadEmitter( _file, _magicName );

        if( id == 0 )
        {
            return false;
        }
		
        MAGIC_POSITION pos;
        Magic_GetEmitterPosition( id, &pos );

        size_t count = s_getCountTag( m_loadEmitterCacheFullname, m_loadEmitterCacheName );

		TVectorEmitters emitters;
		for( size_t i = 0; i != count; ++i )
		{
			HM_EMITTER duplicated_id = Magic_LoadEmitter( _file, _magicName );

			emitters.push_back( duplicated_id );
		}

		if( _container->addEmitterIds( Helper::stringizeString(m_serviceProvider, m_loadEmitterCacheName), id, pos, emitters ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::loadEmitter alredy add %s"
				, m_loadEmitterCacheName.c_str()
				);

			Magic_UnloadEmitter( id );

			return false;
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitterContainerInterfacePtr AstralaxParticleSystem::createEmitterContainerFromMemory( const ConstString & _name, const InputStreamInterfacePtr & _stream )
	{
		size_t fileSize = _stream->size();

		CacheMemoryBuffer container_buffer(m_serviceProvider, fileSize, "AstralaxEmitterContainer2");
		void * container_memory = container_buffer.getMemory();

		if( container_memory == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxEmitterContainer2::initialize %s invalid get memory %d"				
				, _name.c_str()
				, fileSize
				);

			return nullptr;
		}

		_stream->read( container_memory, fileSize );

		HM_FILE file = Magic_OpenFileInMemory( static_cast<const char*>(container_memory) );

		if( file == MAGIC_ERROR )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::createEmitterContainerFromMemory invalid open file in memory (alredy open)"
				);

			return nullptr;
		}

		if( file == MAGIC_UNKNOWN )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::createEmitterContainerFromMemory invalid open file in memory (invalid format or version)"
				);

			return nullptr;
		}

		AstralaxEmitterContainerPtr container = m_factoryPoolAstralaxEmitterContainer.createObjectT();

		container->setServiceProvider( m_serviceProvider );

		if( container->initialize( _name ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::createEmitterContainerFromMemory invalid initialize container %s"
				, _name.c_str()
				);

			Magic_CloseFile( file );

			return nullptr;
		}

		//Load emitters from root folder
		if( this->loadEmittersFolder( "//", file, container ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::createEmitterContainerFromMemory invalid load emitters %s"
				, _name.c_str()
				);

			return nullptr;
		}

		int atlasCount = Magic_GetStaticAtlasCount( file );

		for( int i = 0; i != atlasCount; ++i )
		{
			MAGIC_STATIC_ATLAS magicAtlas;
			if( Magic_GetStaticAtlas( file, i, &magicAtlas ) == MAGIC_ERROR )
			{
				continue;
			}

			ParticleEmitterAtlas atlas;

			atlas.file = Helper::stringizeString( m_serviceProvider, magicAtlas.file );
			//atlas.path = magicAtlas.path;

			container->addAtlas( atlas );
		}

		Magic_CloseFile( file );

		return container;
	}
}