#	include "AstralaxParticleSystem2.h"

#   include "Interface/StringizeInterface.h"

#   include "Core/String.h"
#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ParticleSystem2, Menge::ParticleSystemInterface2, Menge::AstralaxParticleSystem2 );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxParticleSystem2::AstralaxParticleSystem2()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AstralaxParticleSystem2::~AstralaxParticleSystem2()
	{		
        Magic_DestroyAll();
	}
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticleSystem2::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * AstralaxParticleSystem2::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticleSystem2::initialize()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticleSystem2::finalize()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitterContainerInterface2Ptr AstralaxParticleSystem2::createEmitterContainerFromMemory( const ConstString & _name, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator )
	{
		AstralaxEmitterContainer2Ptr container = m_factoryPoolAstralaxEmitterContainer.createObjectT();

		container->setServiceProvider( m_serviceProvider );

		if( container->initialize( _name, _stream, _archivator ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::createEmitterContainerFromMemory invalid initialize container %s"
				, _name.c_str()
				);
			
			return nullptr;
		}

		return container;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticleSystem2::flushParticles( const mt::mat4f & _viewMatrix, ParticleEmitterInterface * _emitter, ParticleMesh * _meshes, size_t _meshLimit, ParticleVertices * _particles, size_t _particlesLimit, ParticleEmitterRenderFlush & _flush )
	{
        (void)_viewMatrix;

        _flush.particleCount = 0;
        _flush.meshCount = 0;

		if( _emitter == nullptr )
		{
			return false;
		}

		AstralaxEmitter2 * emitter = static_cast<AstralaxEmitter2*>( _emitter );

		if( emitter->inInterval() == false )
		{
			return false;
		}

		MAGIC_RENDERING rendering;
		if( emitter->createFirstRenderedParticlesList( &rendering ) == false )
		{
			return false;
		}

		while( rendering.count != 0 )
		{
            if( _particlesLimit <= _flush.particleCount + rendering.count || 
				_meshLimit <= _flush.meshCount )
            {
                return false;
            }
                       
            ParticleMesh & mesh = _meshes[_flush.meshCount];

			mesh.begin = _flush.particleCount;
			mesh.size = rendering.count;
			mesh.texture = rendering.texture_id;
			mesh.intense = rendering.intense;

			this->fillParticles_( _particles, _flush.particleCount, mesh.size );

			_flush.particleCount += rendering.count;
			++_flush.meshCount;
			
			Magic_CreateNextRenderedParticlesList( &rendering );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticleSystem2::fillParticles_( ParticleVertices * _particles, size_t _offset, size_t _count )
	{
		//_particles.resize(_offset + _count);
        for( size_t i = 0; i != _count; ++i )
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
}