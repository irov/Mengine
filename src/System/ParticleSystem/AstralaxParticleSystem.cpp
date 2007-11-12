#	pragma once

#	include "AstralaxParticleSystem.h"
#	include "AstralaxEmitterContainer.h"

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem(ParticleSystemInterface** _ptrParticleSystem)
{
	try
	{
		*_ptrParticleSystem = new ParticleSystem();
	}

	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem(ParticleSystemInterface* _ptrParticleSystem)
{
	delete static_cast<ParticleSystem*>(_ptrParticleSystem);
}
//////////////////////////////////////////////////////////////////////////
ParticleSystem::ParticleSystem()
{
}
//////////////////////////////////////////////////////////////////////////
ParticleSystem::~ParticleSystem()
{
}
//////////////////////////////////////////////////////////////////////////
EmitterContainerInterface * ParticleSystem::createEmitterContainerFromMemory( void * _buffer )
{
	HM_FILE file;

	HM_EMITTER id = 0;

	EmitterContainer * container = NULL;

	bool init = false;

	if ( Magic_OpenFileInMemory( static_cast<char*>(_buffer), &file ) == MAGIC_SUCCESS )
	{
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
						container = new EmitterContainer();
						init = true;
					}

					container->addEmitterId( name, id );
				}
			}

			name = Magic_FindNext( file, &find );
		}

		Magic_DetachFile( file );
	}

	return container;
}
//////////////////////////////////////////////////////////////////////////
EmitterInterface * ParticleSystem::createEmitterFromContainer( const char * _name, const EmitterContainerInterface * _container )
{
	const EmitterContainer * container = static_cast<const EmitterContainer*>( _container );
	HM_EMITTER id = container->getEmitter( _name );

	if( id == 0 )
	{
		return NULL;
	}

	Emitter * emitter = new Emitter( id );

	return emitter;
}
//////////////////////////////////////////////////////////////////////////
void ParticleSystem::lockEmitter( EmitterInterface * _emitter, int _typeParticle )
{
	Emitter * emitter = static_cast<Emitter*>( _emitter );
	HM_EMITTER id = emitter->getId();
	Magic_LockParticlesType( id, _typeParticle );
}
//////////////////////////////////////////////////////////////////////////
RenderParticle * ParticleSystem::nextParticle()
{
	static RenderParticle rp;

	MAGIC_PARTICLE * particle = Magic_GetNextParticle();

	if( particle == NULL )
	{
		return NULL;
	}

	static MAGIC_TEXTURE m_texture[1];

	Magic_GetTexture( 0, &(m_texture[0]) );

	int texture_frame = Magic_GetTextureFrame();

	MAGIC_TEXTURE * magic_texture=&(m_texture[texture_frame]);	

	MAGIC_VERTEX_RECTANGLE * vertex_rectangle = Magic_GetParticleRectangle( particle, magic_texture, 200, 200 );

	rp.x1 = vertex_rectangle->x1;
	rp.x2 = vertex_rectangle->x2;
	rp.x3 = vertex_rectangle->x3;
	rp.x4 = vertex_rectangle->x4;
	rp.y1 = vertex_rectangle->y1;
	rp.y2 = vertex_rectangle->y2;
	rp.y3 = vertex_rectangle->y3;
	rp.y4 = vertex_rectangle->y4;

	rp.u0 = magic_texture->left;
	rp.v0 = magic_texture->top;
	rp.u1 = magic_texture->right;
	rp.v1 = magic_texture->bottom;

	rp.color = particle->color;

	return &rp;
}
//////////////////////////////////////////////////////////////////////////
void ParticleSystem::unlockEmitter( EmitterInterface * _emitter ) 
{
	Magic_UnlockParticlesType();
}
//////////////////////////////////////////////////////////////////////////