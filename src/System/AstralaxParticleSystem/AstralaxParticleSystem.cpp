#	include "AstralaxParticleSystem.h"

#   include "Interface/StringizeInterface.h"

#	include "Core/MemoryCacheBuffer.h"
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
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticleSystem::finalize()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	static uint32_t s_getCountTag( const char * _fullname, char * _name )
	{
		if( strlen(_fullname) >= MENGINE_ASTRALAX_EMITTER_NAME_MAX )
		{
			return (uint32_t)-1;
		}

		_name[0] = '\0';

		const char * st_begin = strchr( _fullname, '[' );

		if( st_begin == nullptr )
		{
			strcat( _name, _fullname );

			return 0;
		}

		const char * st_end = strchr( _fullname, ']' );

		char num[MENGINE_ASTRALAX_EMITTER_NAME_MAX];
		num[0] = '\0';

		strncat( num, st_begin + 1, st_end - st_begin - 1 );

		uint32_t str_count;
		if( sscanf( num, "%u", &str_count ) != 1 )
		{
			return (uint32_t)-1;
		}

		strncat( _name, _fullname, st_begin - _fullname );

		return str_count;
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
        //m_loadEmitterCacheFullname = _magicName;
		
        HM_EMITTER id = Magic_LoadEmitter( _file, _magicName );

        if( id == 0 )
        {
            return false;
        }
		
        MAGIC_POSITION pos;
        Magic_GetEmitterPosition( id, &pos );

		char emitterName[MENGINE_ASTRALAX_EMITTER_NAME_MAX];		
        uint32_t count = s_getCountTag( _magicName, emitterName );

		if( count == (uint32_t)-1 )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::loadEmitter invalid emitterName %s (maybe len > %d)"
				, _magicName
				, MENGINE_ASTRALAX_EMITTER_NAME_MAX
				);

			return false;
		}

		TVectorEmitters emitters;
		for( uint32_t i = 0; i != count; ++i )
		{
			HM_EMITTER duplicated_id = Magic_LoadEmitter( _file, _magicName );

			emitters.push_back( duplicated_id );
		}

		if( _container->addEmitterIds( emitterName, id, pos, emitters ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::loadEmitter alredy add %s"
				, emitterName
				);

			Magic_UnloadEmitter( id );

			return false;
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitterContainerInterfacePtr AstralaxParticleSystem::createParticleEmitterContainer()
	{
		AstralaxEmitterContainerPtr container = m_factoryPoolAstralaxEmitterContainer.createObjectT();

		container->setServiceProvider( m_serviceProvider );

		if( container->initialize() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::createEmitterContainerFromMemory invalid initialize container"
				);

			return nullptr;
		}

		return container;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticleSystem::loadParticleEmitterContainerFromMemory( const ParticleEmitterContainerInterfacePtr & _container, const InputStreamInterfacePtr & _stream )
	{
		AstralaxEmitterContainerPtr astralax_container = stdex::intrusive_static_cast<AstralaxEmitterContainerPtr>(_container);

		MemoryCacheBufferPtr data_cache = Helper::createMemoryStream( m_serviceProvider, _stream, "AstralaxEmitterContainer2" );
		
		if( data_cache == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxEmitterContainer2::initialize invalid get memory"				
				);

			return false;
		}
				
		const void * container_memory = data_cache->getMemory();
		
		HM_FILE file = Magic_OpenFileInMemory( static_cast<const char*>(container_memory) );

		if( file == MAGIC_ERROR )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::createEmitterContainerFromMemory invalid open file in memory (alredy open)"
				);

			return false;
		}

		if( file == MAGIC_UNKNOWN )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::createEmitterContainerFromMemory invalid open file in memory (invalid format or version)"
				);

			return false;
		}
		
		//Load emitters from root folder
		if( this->loadEmittersFolder( "//", file, astralax_container ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::createEmitterContainerFromMemory invalid load emitters"
				);

			Magic_CloseFile( file );

			return false;
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

			atlas.filename = Helper::stringizeString( m_serviceProvider, magicAtlas.file );

			astralax_container->addAtlas( atlas );
		}

		Magic_CloseFile( file );

		return true;
	}
}