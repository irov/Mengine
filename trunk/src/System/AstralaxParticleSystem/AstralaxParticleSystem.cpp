#	include "AstralaxParticleSystem.h"
#	include "AstralaxEmitterContainer.h"

#   include "Interface/StringizeInterface.h"

#   include "Core/String.h"
#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ParticleSystem, Menge::ParticleSystemInterface, Menge::AstralaxParticleSystem );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxParticleSystem::AstralaxParticleSystem()
        : m_serviceProvider(NULL)
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
	void AstralaxParticleSystem::loadEmittersFolder( const char * _path, HM_FILE _file, AstralaxEmitterContainer * _container )
	{
		Magic_SetCurrentFolder(_file, _path );
		
		MAGIC_FIND_DATA find;
		const char* magicName = Magic_FindFirst(_file, &find, MAGIC_FOLDER | MAGIC_EMITTER);
		
		while( magicName )
		{
			//if animated folder or emitter
			if ( find.animate == 1 )
			{
				if( this->loadEmitter( magicName, _file, _container ) == false )
                {
                    LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::loadEmittersFolder %s invalid load emitter %s"
                        , _path
                        , magicName
                        );
                }
			}
			//if folder
			else
			{		
				this->loadEmittersFolder( magicName, _file, _container );
			}
		
			magicName = Magic_FindNext( _file, &find );
		}
			
		Magic_SetCurrentFolder(_file, "..");
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticleSystem::loadEmitter( const char * _magicName, HM_FILE _file, AstralaxEmitterContainer * _container )
	{
		static String fullname;
        static String name;

        fullname = _magicName;
		
        HM_EMITTER id = Magic_LoadEmitter( _file, _magicName );

        if( id == 0 )
        {
            return false;
        }

        MAGIC_POSITION pos;
        Magic_GetEmitterPosition( id, &pos );

        size_t count = s_getCountTag( fullname, name );

		TVectorEmitters emitters;
		for( size_t i = 0; i != count; ++i )
		{
            HM_EMITTER duplicated_id = Magic_LoadEmitter( _file, _magicName );
			
			emitters.push_back( duplicated_id );
		}

		_container->addEmitterIds( Helper::stringizeString(m_serviceProvider, name), id, pos, emitters );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	EmitterContainerInterface * AstralaxParticleSystem::createEmitterContainerFromMemory( const void * _buffer )
	{
		HM_FILE file = Magic_OpenFileInMemory( static_cast<const char*>(_buffer) );

		if( file == 0 )
		{
            LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::createEmitterContainerFromMemory invalid open file in memory"
                );

			return NULL;
		}
		
		AstralaxEmitterContainer * container = new AstralaxEmitterContainer();
					
		//Load emitters from root folder
		this->loadEmittersFolder( "//", file, container );
		
		int atlasCount = Magic_GetStaticAtlasCount( file );

		for( int i = 0; i != atlasCount; ++i )
		{
			MAGIC_STATIC_ATLAS magicAtlas;
			if( Magic_GetStaticAtlas( file, i, &magicAtlas ) == MAGIC_ERROR )
			{
				continue;
			}

			EmitterContainerInterface::EmitterAtlas atlas;

            atlas.file = Helper::stringizeString( m_serviceProvider, magicAtlas.file );
			//atlas.path = magicAtlas.path;
	
			container->addAtlas( atlas );
		}

		Magic_CloseFile( file );

		return container;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticleSystem::flushParticles( const mt::mat4f & _viewMatrix, EmitterInterface * _emitter, ParticleMesh * _meshes, ParticleVertices * _particles, size_t _particlesLimit, EmitterRenderFlush & _flush )
	{
        (void)_viewMatrix;

		if( _emitter == 0 )
		{
			return false;
		}

		AstralaxEmitter * emitter = static_cast<AstralaxEmitter*>( _emitter );
		HM_EMITTER id = emitter->getId();

		if( Magic_InInterval( id ) == false )
		{
			return false;
		}

        MAGIC_POSITION pos;
        Magic_GetEmitterPosition( id, &pos );

#   ifdef MAGIC_3D
        Magic_SetViewMatrix( (MAGIC_MATRIX *)&_viewMatrix );

        MAGIC_ORIENTATION orientation;
        orientation.orientation = MAGIC_ORIENTATION_AXIS;
        orientation.x = 0.f;
        orientation.y = 0.f;
        orientation.z = 1.f;

        Magic_SetOrientation( &orientation );
#   endif

		MAGIC_RENDERING rendering;
		if( Magic_CreateFirstRenderedParticlesList(id, &rendering) == MAGIC_ERROR )
		{
			return false;
		}

		_flush.particleCount = 0;
		_flush.meshCount = 0;

		if( _particlesLimit <= (size_t)rendering.count )
		{
			return false;
		}

		while( rendering.count != 0 )
		{
			ParticleMesh & mesh = _meshes[_flush.meshCount];

			mesh.begin = _flush.particleCount;
			mesh.size = rendering.count;
			mesh.texture = rendering.texture_id;
			mesh.intense = rendering.intense;

			if( _particlesLimit <= _flush.particleCount )
			{
				return false;
			}
			
			this->fillParticles_( _particles, _flush.particleCount, rendering.count );

			_flush.particleCount += rendering.count;
			++_flush.meshCount;

			Magic_CreateNextRenderedParticlesList(&rendering);
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticleSystem::fillParticles_( ParticleVertices * _particles, size_t _offset, int _count )
	{
		//_particles.resize(_offset + _count);
        for( int i = 0; i != _count; ++i )
		{
            MAGIC_PARTICLE_VERTEXES vertexes;
			Magic_GetNextParticleVertexes( &vertexes );

			ParticleVertices & rp = _particles[_offset + i];

			rp.v[0].x = vertexes.vertex1.x;
			rp.v[0].y = vertexes.vertex1.y;

#ifdef MAGIC_3D
            rp.v[0].z = vertexes.vertex1.z; 
#else
            rp.v[0].z = 0.f;
#endif

			rp.v[1].x = vertexes.vertex2.x;
			rp.v[1].y = vertexes.vertex2.y;

#ifdef MAGIC_3D
            rp.v[1].z = vertexes.vertex2.z;
#else
            rp.v[1].z = 0.f;
#endif

			rp.v[2].x = vertexes.vertex3.x;
			rp.v[2].y = vertexes.vertex3.y;

#ifdef MAGIC_3D
            rp.v[2].z = vertexes.vertex3.z;
#else
            rp.v[2].z = 0.f;
#endif

			rp.v[3].x = vertexes.vertex4.x;
			rp.v[3].y = vertexes.vertex4.y;

#ifdef MAGIC_3D
            rp.v[3].z = vertexes.vertex4.z;
#else
            rp.v[3].z = 0.f;
#endif


			rp.uv[0].x = vertexes.u1;
			rp.uv[0].y = vertexes.v1;
			rp.uv[1].x = vertexes.u2;
			rp.uv[1].y = vertexes.v2;
			rp.uv[2].x = vertexes.u3;
			rp.uv[2].y = vertexes.v3;
			rp.uv[3].x = vertexes.u4;
			rp.uv[3].y = vertexes.v4;

			rp.color = vertexes.color;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticleSystem::releaseEmitterContainer( EmitterContainerInterface* _containerInterface )
	{
		delete static_cast<AstralaxEmitterContainer*>( _containerInterface );
	}
	//////////////////////////////////////////////////////////////////////////
}