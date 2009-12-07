
#	include "AstralaxParticleSystem.h"
#	include "AstralaxEmitterContainer.h"

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::ParticleSystemInterface** _ptrParticleSystem )
{
	try
	{
		*_ptrParticleSystem = new AstralaxParticleSystem();
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
	delete static_cast<AstralaxParticleSystem*>(_ptrParticleSystem);
}
//////////////////////////////////////////////////////////////////////////
AstralaxParticleSystem::AstralaxParticleSystem()
: m_textureCount( 0 )
{
	for ( int i = 0; i < ASTRALAX_PARTICLE_MAX_TEXTURES; ++i )
	{
		m_texture[i] = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////
AstralaxParticleSystem::~AstralaxParticleSystem()
{
}
//////////////////////////////////////////////////////////////////////////
Menge::EmitterContainerInterface * AstralaxParticleSystem::createEmitterContainerFromMemory( void * _buffer )
{
	HM_FILE file;

	HM_EMITTER id = 0;

	AstralaxEmitterContainer * container = NULL;

	bool init = false;
	
	if ( Magic_OpenFileInMemory( static_cast<char*>(_buffer), &file ) != MAGIC_SUCCESS )
	{
		return 0;
	}

	MAGIC_FIND_DATA find;

	const char * name = Magic_FindFirst( file, &find, MAGIC_FOLDER | MAGIC_EMITTER );
	while ( name )
	{
		if ( find.type & MAGIC_EMITTER )
		{
			if ( Magic_LoadEmitter( file, name, &id ) == MAGIC_SUCCESS )
			{
				if( init == false )
				{
					container = new AstralaxEmitterContainer( file );
					init = true;
				}
				Magic_SetEmitterPositionMode( id, false );
				container->addEmitterId( name, id );
			}
		}

		name = Magic_FindNext( file, &find );
	}

	Magic_CloseFile( file );

	return container;
}
//////////////////////////////////////////////////////////////////////////
Menge::EmitterInterface * AstralaxParticleSystem::createEmitterFromContainer( const Menge::String & _name, const Menge::EmitterContainerInterface * _container )
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
void AstralaxParticleSystem::releaseEmitter( Menge::EmitterInterface * _emitter )
{
	delete static_cast<AstralaxEmitter*>(_emitter);
}
//////////////////////////////////////////////////////////////////////////
bool AstralaxParticleSystem::lockEmitter( Menge::EmitterInterface * _emitter, int _typeParticle )
{
	AstralaxEmitter * emitter = static_cast<AstralaxEmitter*>( _emitter );
	HM_EMITTER id = emitter->getId();

	Magic_LockParticlesType( id, _typeParticle );

	bool pos =  Magic_GetEmitterPositionMode( id );

	m_textureCount = Magic_GetTextureCount();

	if( m_textureCount == 0 )
	{
		//Magic_UnlockParticlesType();
		//return false;
	}

	if( m_textureCount > ASTRALAX_PARTICLE_MAX_TEXTURES )
	{
		m_textureCount = ASTRALAX_PARTICLE_MAX_TEXTURES;
	}

	for ( int i = 0; i < m_textureCount; i++ )
	{
		m_texture[i] = Magic_GetTexture( i );
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
int AstralaxParticleSystem::flushParticles( Menge::TVectorRenderParticle & _particles, int _particlesLimit )
{
	if( m_textureCount == 0 )
	{
		return 0;
	}

	int count = 0;

	while( MAGIC_PARTICLE * particle = Magic_GetNextParticle() )
	{
		MAGIC_TEXTURE * magic_texture = m_texture[particle->frame];

		MAGIC_VERTEX_RECTANGLE * vertex_rectangle = Magic_GetParticleRectangle( particle, magic_texture );

		Menge::RenderParticle & rp = _particles[count++];

		*(MAGIC_VERTEX_RECTANGLE *)&rp.rectangle = *vertex_rectangle;

		//rp.x1 = vertex_rectangle->x1;
		//rp.y1 = vertex_rectangle->y1;
		//rp.x2 = vertex_rectangle->x2;
		//rp.y2 = vertex_rectangle->y2;
		//rp.x3 = vertex_rectangle->x3;		
		//rp.y3 = vertex_rectangle->y3;
		//rp.x4 = vertex_rectangle->x4;
		//rp.y4 = vertex_rectangle->y4;

		//rp.texture.u0 = 0.0f;
		//rp.texture.v0 = 0.0f;
		//rp.texture.u1 = 1.0f;
		//rp.texture.v1 = 1.0f;
		rp.texture.frame = particle->frame;

		rp.color.rgba = particle->color;

		if( count == _particlesLimit )
		{
			break;
		}
	}

	return count;
}
//////////////////////////////////////////////////////////////////////////
Menge::String AstralaxParticleSystem::getTextureName( int _index ) const
{
	if( m_texture[_index] == NULL )
	{
		return Menge::String();
	}
	if( m_texture[_index]->file == NULL )
	{
		return Menge::String();
	}
	return m_texture[_index]->file;
}
//////////////////////////////////////////////////////////////////////////
void AstralaxParticleSystem::unlockEmitter( Menge::EmitterInterface * _emitter )
{
	Magic_UnlockParticlesType();
}
//////////////////////////////////////////////////////////////////////////
void AstralaxParticleSystem::releaseEmitterContainer( Menge::EmitterContainerInterface* _containerInterface )
{
	delete static_cast<AstralaxEmitterContainer*>( _containerInterface );
}
//////////////////////////////////////////////////////////////////////////
int AstralaxParticleSystem::getTextureCount() const
{
	return m_textureCount;
}
//////////////////////////////////////////////////////////////////////////
