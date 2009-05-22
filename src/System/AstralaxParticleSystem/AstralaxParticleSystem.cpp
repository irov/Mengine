
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
	: m_currentX(0.0f)
	, m_currentY(0.0f)
{}
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

	AstralaxEmitter * emitter = new AstralaxEmitter( id );

	return emitter;
}
//////////////////////////////////////////////////////////////////////////
void AstralaxParticleSystem::releaseEmitter( Menge::EmitterInterface * _emitter )
{
	delete static_cast<AstralaxEmitter*>(_emitter);
}
//////////////////////////////////////////////////////////////////////////
void AstralaxParticleSystem::lockEmitter( Menge::EmitterInterface * _emitter, int _typeParticle )
{
	AstralaxEmitter * emitter = static_cast<AstralaxEmitter*>( _emitter );
	HM_EMITTER id = emitter->getId();

	Magic_LockParticlesType( id, _typeParticle );

	//Magic_GetEmitterPosition( id, &m_currentX, &m_currentY );

	bool pos =  Magic_GetEmitterPositionMode( id );

	int texCount = Magic_GetTextureCount();

	if( texCount > ASTRALAX_PARTICLE_MAX_TEXTURES )
	{
		texCount = ASTRALAX_PARTICLE_MAX_TEXTURES;
	}

	for ( int i = 0; i < texCount; i++ )
	{
		m_texture[i] = Magic_GetTexture( i );
	}
}
//////////////////////////////////////////////////////////////////////////
bool AstralaxParticleSystem::nextParticle( Menge::RenderParticle & _particle )
{
	MAGIC_PARTICLE * particle = Magic_GetNextParticle();

	if( particle == NULL )
	{
		return false;
	}

	//int texture_frame = Magic_GetTextureFrame();

	/*MAGIC_PARTICLE* parent = particle->owner;
	if( parent != NULL )
	{
		while( parent != NULL )
		{
			particle->x += parent->x;
			particle->y += parent->y;

			parent = parent->owner;
		}
		particle->owner = NULL;
	}*/

	MAGIC_TEXTURE * magic_texture = m_texture[particle->frame];
	
	MAGIC_VERTEX_RECTANGLE * vertex_rectangle = Magic_GetParticleRectangle( particle, magic_texture );

	_particle.x2 = vertex_rectangle->x1 + m_currentX;
	_particle.x1 = vertex_rectangle->x2 + m_currentX;
	_particle.x4 = vertex_rectangle->x3 + m_currentX;
	_particle.x3 = vertex_rectangle->x4 + m_currentX;
	_particle.y2 = vertex_rectangle->y1 + m_currentY;
	_particle.y1 = vertex_rectangle->y2 + m_currentY;
	_particle.y4 = vertex_rectangle->y3 + m_currentY;
	_particle.y3 = vertex_rectangle->y4 + m_currentY;

	/*rp.u0 = magic_texture->left;
	rp.v0 = magic_texture->top;
	rp.u1 = magic_texture->right;
	rp.v1 = magic_texture->bottom;*/

	_particle.u0 = 0.0f;
	_particle.v0 = 0.0f;
	_particle.u1 = 1.0f;
	_particle.v1 = 1.0f;

	_particle.color = particle->color;

	return true;
}
//////////////////////////////////////////////////////////////////////////
Menge::String AstralaxParticleSystem::getTextureName() const
{
	return m_texture[0]->file;
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
