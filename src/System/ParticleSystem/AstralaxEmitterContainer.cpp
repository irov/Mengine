#	include "AstralaxEmitterContainer.h"
#	include "AstralaxEmitter.h"

	EmitterContainer::EmitterContainer()
	{
	};

	EmitterContainer::~EmitterContainer()
	{
	}
	
	void EmitterContainer::addEmitterId( const char * _name,  HM_EMITTER _id )
	{
		m_emittersIds.insert( std::make_pair( _name, _id ) );
	}

	HM_EMITTER EmitterContainer::getEmitter( const char * _name ) const
	{
		std::map<std::string, HM_EMITTER>::const_iterator it = m_emittersIds.find( _name );

		if( it == m_emittersIds.end() )
		{
			return NULL;
		}

		HM_EMITTER id = it->second;

		if( id == 0 )
		{
			return NULL;
		}

		return id;
	}

/*	int EmitterContainer::sizeImages( const EmitterInterface * _emitter, int _particleType )
	{
		const Emitter * emitter = static_cast<const Emitter*>( _emitter );

		HM_EMITTER id = emitter->getId();
	
		Magic_LockParticlesType( id, _particleType );
		int count = Magic_GetTextureCount();
		Magic_UnlockParticlesType();

		return count;
	}

	const char * EmitterContainer::getImageName( const EmitterInterface * _emitter, int _particleType, int _index  )
	{
		const Emitter * emitter = static_cast<const Emitter*>( _emitter );

		HM_EMITTER id = emitter->getId();

		MAGIC_TEXTURE * texture = NULL;
	
		Magic_LockParticlesType( id, _particleType );
		Magic_GetTexture( _index, texture );
		Magic_UnlockParticlesType();

		return texture->file;
	}*/