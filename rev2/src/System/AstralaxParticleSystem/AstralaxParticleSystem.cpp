#	pragma once

#	include "AstralaxParticleSystem.h"
#	include "AstralaxEmitterContainer.h"

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem(ParticleSystemInterface** _ptrParticleSystem)
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
void releaseInterfaceSystem(ParticleSystemInterface* _ptrParticleSystem)
{
	delete static_cast<AstralaxParticleSystem*>(_ptrParticleSystem);
}
//////////////////////////////////////////////////////////////////////////
AstralaxParticleSystem::AstralaxParticleSystem()
	: m_currentX(0)
	, m_currentY(0)
{}
//////////////////////////////////////////////////////////////////////////
AstralaxParticleSystem::~AstralaxParticleSystem()
{
}
//////////////////////////////////////////////////////////////////////////
EmitterContainerInterface * AstralaxParticleSystem::createEmitterContainerFromMemory( void * _buffer )
{
	HM_FILE file;

	HM_EMITTER id = 0;

	AstralaxEmitterContainer * container = NULL;

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
						container = new AstralaxEmitterContainer();
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
EmitterInterface * AstralaxParticleSystem::createEmitterFromContainer( const char * _name, const EmitterContainerInterface * _container )
{
	const AstralaxEmitterContainer * container = static_cast<const AstralaxEmitterContainer*>( _container );
	HM_EMITTER id = container->getEmitter( _name );

	if( id == 0 )
	{
		return NULL;
	}

	AstralaxEmitter * emitter = new AstralaxEmitter( id );

	return emitter;
}
//////////////////////////////////////////////////////////////////////////
void AstralaxParticleSystem::releaseEmitter( EmitterInterface * _emitter )
{
	delete static_cast<AstralaxEmitter*>(_emitter);
}
//////////////////////////////////////////////////////////////////////////
void AstralaxParticleSystem::lockEmitter( EmitterInterface * _emitter, int _typeParticle )
{
	AstralaxEmitter * emitter = static_cast<AstralaxEmitter*>( _emitter );
	HM_EMITTER id = emitter->getId();

	Magic_GetEmitterPosition( id, &m_currentX, &m_currentY );

	Magic_LockParticlesType( id, _typeParticle );

	Magic_GetTexture( 0, &m_texture );
}
//////////////////////////////////////////////////////////////////////////
RenderParticle * AstralaxParticleSystem::nextParticle()
{
	static RenderParticle rp;

	MAGIC_PARTICLE * particle = Magic_GetNextParticle();

	if( particle == NULL )
	{
		return NULL;
	}

	MAGIC_TEXTURE * magic_texture = & m_texture;	

	MAGIC_VERTEX_RECTANGLE * vertex_rectangle = Magic_GetParticleRectangle( particle, magic_texture, m_currentX, m_currentY );

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
const char * AstralaxParticleSystem::getTextureName() const
{
	return /*((m_texture.length == 0) || (m_texture.data == 0)) ? 0 : */m_texture.file;
}
//////////////////////////////////////////////////////////////////////////
void AstralaxParticleSystem::unlockEmitter( EmitterInterface * _emitter ) 
{
	Magic_UnlockParticlesType();
}
//////////////////////////////////////////////////////////////////////////