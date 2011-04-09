
#	include "AstralaxParticleSystem.h"
#	include "AstralaxEmitterContainer.h"

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::ParticleSystemInterface** _ptrParticleSystem )
{
	try
	{
		*_ptrParticleSystem = new Menge::AstralaxParticleSystem();
	}

	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::ParticleSystemInterface* _ptrParticleSystem )
{
	delete static_cast<Menge::AstralaxParticleSystem*>(_ptrParticleSystem);
}

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxParticleSystem::AstralaxParticleSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AstralaxParticleSystem::~AstralaxParticleSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	EmitterContainerInterface * AstralaxParticleSystem::createEmitterContainerFromMemory( const void * _buffer )
	{
		HM_FILE file = Magic_OpenFileInMemory( static_cast<const char*>(_buffer) );

		if( file == 0 )
		{
			return 0;
		}

		AstralaxEmitterContainer * container = new AstralaxEmitterContainer();

		MAGIC_FIND_DATA find;
		const char * name = Magic_FindFirst( file, &find, MAGIC_FOLDER | MAGIC_EMITTER );

		while( name )
		{
			if ( find.type & MAGIC_EMITTER )
			{
				HM_EMITTER id = Magic_LoadEmitter( file, name );

				if( id == 0 )
				{
					continue;
				}

				Magic_SetEmitterPositionMode( id, false );
				container->addEmitterId( name, id );
			}

			name = Magic_FindNext( file, &find );
		}

		int atlasCount = Magic_GetStaticAtlasCount( file );

		for( int i = 0; i != atlasCount; ++i )
		{
			MAGIC_STATIC_ATLAS magicAtlas;
			if( Magic_GetStaticAtlas( file, i, &magicAtlas ) == MAGIC_ERROR )
			{
				continue;
			}

			EmitterContainerInterface::EmitterAtlas atlas;

			atlas.file = magicAtlas.file;
			atlas.path = magicAtlas.path;
	
			container->addAtlas( atlas );
		}

		Magic_CloseFile( file );

		return container;
	}
	//////////////////////////////////////////////////////////////////////////
	EmitterInterface * AstralaxParticleSystem::createEmitterFromContainer( const String & _name, const EmitterContainerInterface * _container )
	{
		const AstralaxEmitterContainer * container =
			static_cast<const AstralaxEmitterContainer*>( _container );

		HM_EMITTER id = container->getEmitter( _name );

		if( id == 0 )
		{
			return NULL;
		}

		AstralaxEmitter * emitter = new AstralaxEmitter( id, _name );

		return emitter;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticleSystem::releaseEmitter( EmitterInterface * _emitter )
	{
		delete static_cast<AstralaxEmitter*>(_emitter);
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticleSystem::getEmitterPosition( EmitterInterface * _emitter, mt::vec2f & _pos )
	{
		AstralaxEmitter * emitter = static_cast<AstralaxEmitter*>( _emitter );
		HM_EMITTER id = emitter->getId();

		MAGIC_POSITION pos;
		Magic_GetEmitterPosition( id, &pos );
		_pos.x = pos.x;
		_pos.y = pos.y;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticleSystem::flushParticles( EmitterInterface * _emitter, TVectorParticleMeshes & _meshes, TVectorParticleVerices & _particles, int _particlesLimit )
	{
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

		MAGIC_RENDERING rendering;
		if( Magic_CreateFirstRenderedParticlesList(id, &rendering) == MAGIC_ERROR )
		{
			return false;
		}

		while( rendering.count )
		{
			TVectorParticleVerices::size_type offset = _particles.size();

			ParticleMesh mesh;
			mesh.begin = offset;
			mesh.size = rendering.count;
			mesh.texture = rendering.texture_id;
			mesh.intense = rendering.intense;

			_meshes.push_back( mesh );

			this->fillParticles_( _particles, offset, rendering.count );

			Magic_CreateNextRenderedParticlesList(&rendering);
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticleSystem::fillParticles_( TVectorParticleVerices & _particles, TVectorParticleVerices::size_type _offset, int _count )
	{
		_particles.resize(_offset + _count);

		MAGIC_PARTICLE_VERTEXES vertexes;
		for( int i = 0; i != _count; ++i )
		{
			Magic_GetNextParticleVertexes(&vertexes);

			ParticleVertices & rp = _particles[_offset + i];

			rp.v[0].x = vertexes.x1;
			rp.v[0].y = vertexes.y1;
			rp.v[1].x = vertexes.x2;
			rp.v[1].y = vertexes.y2;
			rp.v[2].x = vertexes.x3;
			rp.v[2].y = vertexes.y3;
			rp.v[3].x = vertexes.x4;
			rp.v[3].y = vertexes.y4;

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