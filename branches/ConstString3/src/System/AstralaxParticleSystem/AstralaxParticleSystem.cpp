#	include "AstralaxParticleSystem.h"
#	include "AstralaxEmitterContainer.h"

#	include "stdio.h"

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
	static size_t s_getCountTag( const String & _fullname, String & _name )
	{
		String::size_type st_begin = _fullname.find_first_of('[');

		if( st_begin == String::npos )
		{
			_name = _fullname;
			return 1;
		}

		int count;

		String::size_type st_end = _fullname.find_first_of(']');

		String num = _fullname.substr( st_begin + 1, st_end - st_begin - 1 );

		if( sscanf( num.c_str(), "%d", &count ) != 1 )
		{
			return 0;
		}

		_name = _fullname.substr( 0, st_begin );

		return count;
	}
	//////////////////////////////////////////////////////////////////////////
	void  AstralaxParticleSystem::loadEmittersFolder( const char * _path, HM_FILE _file, AstralaxEmitterContainer * _container )
	{
		Magic_SetCurrentFolder(_file, _path );
		
		MAGIC_FIND_DATA find;
		const char* magicName=Magic_FindFirst(_file, &find, MAGIC_FOLDER | MAGIC_EMITTER);
		
		while( magicName )
		{
			//if animated folder or emitter
			if ( find.animate == 1 )
			{
				loadEmitter( magicName, _file, _container );
			}
			//if folder
			else
			{
				if( _container->isMetaData(magicName) == true )
				{	
					_container->setMetaData( magicName );
				}
				
				loadEmittersFolder( magicName, _file, _container );
			}
		
			magicName = Magic_FindNext( _file, &find );
		}
			
		Magic_SetCurrentFolder(_file, "..");
	}
	//////////////////////////////////////////////////////////////////////////
	void  AstralaxParticleSystem::loadEmitter( const char* _magicName, HM_FILE _file, AstralaxEmitterContainer * _container )
	{
		String fullname = _magicName;
		String name;

		size_t count = s_getCountTag( fullname, name );

		TVectorEmittersId emitters;
		for( size_t i = 0; i != count; ++i )
		{
			HM_EMITTER id = Magic_LoadEmitter( _file, _magicName );

			if( id == 0 )
			{
				continue;
			}

			Magic_SetEmitterPositionMode( id, false );

			emitters.push_back( id );
		}

		_container->addEmitterIds( name, emitters );
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
					
		//Load emitters from root folder
		loadEmittersFolder( "//", file, container );
		
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
			//atlas.path = magicAtlas.path;
	
			container->addAtlas( atlas );
		}

		Magic_CloseFile( file );

		return container;
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

			rp.v[0].x = vertexes.vertex1.x;
			rp.v[0].y = vertexes.vertex1.y;
			rp.v[1].x = vertexes.vertex2.x;
			rp.v[1].y = vertexes.vertex2.y;
			rp.v[2].x = vertexes.vertex3.x;
			rp.v[2].y = vertexes.vertex3.y;
			rp.v[3].x = vertexes.vertex4.x;
			rp.v[3].y = vertexes.vertex4.y;

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