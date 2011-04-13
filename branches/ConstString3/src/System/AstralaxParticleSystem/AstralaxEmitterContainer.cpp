#	include "AstralaxEmitterContainer.h"
#	include "AstralaxEmitter.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitterContainer::AstralaxEmitterContainer()
	{
	};
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitterContainer::~AstralaxEmitterContainer()
	{
		for( TMapEmitters::iterator 
			it = m_emittersIds.begin(), 
			it_end = m_emittersIds.end();
		it != it_end;
		++it )
		{
			EmitterPool & pool = it->second;

			for( TVectorEmitters::iterator 
				it_pool = pool.emitters.begin(),
				it_pool_end = pool.emitters.end();
			it_pool != it_pool_end;
			++it_pool )
			{
				Magic_UnloadEmitter( *it_pool );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer::addEmitterIds( const String & _name, TVectorEmitters _emitters )
	{
		if( _emitters.empty() == true )
		{
			return;
		}

		TMapEmitters::iterator it_found = m_emittersIds.find( _name );

		if( it_found != m_emittersIds.end() )
		{
			return;
		}

		EmitterPool pool;
		pool.emitters = _emitters;
		pool.dublicate = _emitters.size() == 1;

		m_emittersIds.insert( std::make_pair( _name, pool ) );
	}
	//////////////////////////////////////////////////////////////////////////
	HM_EMITTER AstralaxEmitterContainer::getEmitterId( const String & _name )
	{
		TMapEmitters::iterator it = m_emittersIds.find( _name );

		if( it == m_emittersIds.end() )
		{
			return 0;
		}

		EmitterPool & pool = it->second;

		HM_EMITTER result_id;

		if( pool.dublicate == true )
		{
			HM_EMITTER etalon_id = pool.emitters[0];

			if( etalon_id == 0 )
			{
				return 0;
			}

			HM_EMITTER duplicated_id = Magic_DuplicateEmitter( etalon_id );
		
			result_id = duplicated_id;
		}
		else
		{
			if( pool.emitters.empty() == true )
			{
				return 0;
			}

			HM_EMITTER pool_id = pool.emitters.back();
			pool.emitters.pop_back();

			result_id = pool_id;
		}

		return result_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer::releaseEmitterId( const String & _name, HM_EMITTER _id )
	{
		TMapEmitters::iterator it = m_emittersIds.find( _name );

		if( it == m_emittersIds.end() )
		{
			return;
		}

		if( it->second.dublicate == true )
		{
			Magic_UnloadEmitter( _id );
		}
		else
		{
			it->second.emitters.push_back( _id );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer::addAtlas( const EmitterAtlas & _atlas )
	{
		m_atlas.push_back( _atlas );
	}
	//////////////////////////////////////////////////////////////////////////
	const EmitterContainerInterface::TVectorAtlas & AstralaxEmitterContainer::getAtlas() const
	{
		return m_atlas;
	}
	//////////////////////////////////////////////////////////////////////////
	EmitterInterface * AstralaxEmitterContainer::createEmitter( const String & _name )
	{
		HM_EMITTER id = this->getEmitterId( _name );

		if( id == 0 )
		{
			return NULL;
		}

		AstralaxEmitter * emitter = new AstralaxEmitter( this, id, _name );

		return emitter;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer::releaseEmitter( EmitterInterface * _emitter )
	{
		AstralaxEmitter * emitter = static_cast<AstralaxEmitter*>(_emitter);

		const String & name = emitter->getName();
		HM_EMITTER id = emitter->getId();
		AstralaxEmitterContainer * container = emitter->getContainer();

		container->releaseEmitterId( name, id );

		delete emitter;
	}
}