#	include "AstralaxEmitterContainer.h"

#	include "Utils/Core/String.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitterContainer::AstralaxEmitterContainer()
        : m_serviceProvider(nullptr)
	{
	};
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitterContainer::~AstralaxEmitterContainer()
	{
		for( TMapEmitters::const_iterator 
			it = m_emitters.begin(), 
			it_end = m_emitters.end();
		it != it_end;
		++it )
		{
			const EmitterPool & pool = it->second;

            Magic_UnloadEmitter( pool.id );

			for( TVectorEmitters::const_iterator
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
    bool AstralaxEmitterContainer::initialize( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer::addEmitterIds( const ConstString & _name, HM_EMITTER _id, MAGIC_POSITION _pos, const TVectorEmitters & _emitters )
	{
		TMapEmitters::iterator it_found = m_emitters.find( _name );

		if( it_found != m_emitters.end() )
		{
			return;
		}

		EmitterPool pool;

        pool.id = _id;
        pool.pos = _pos;
		pool.emitters = _emitters;
		pool.dublicate = _emitters.size() == 0;

		m_emitters.insert( std::make_pair( _name, pool ) );
	}
	//////////////////////////////////////////////////////////////////////////
	HM_EMITTER AstralaxEmitterContainer::getEmitterId( const ConstString & _name ) const
	{
		TMapEmitters::const_iterator it = m_emitters.find( _name );

		if( it == m_emitters.end() )
		{
			return 0;
		}

		const EmitterPool & pool = it->second;

		HM_EMITTER result_id;

		if( pool.dublicate == true )
		{
			HM_EMITTER duplicated_id = Magic_DuplicateEmitter( pool.id );

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

        Magic_SetEmitterPosition( result_id, const_cast<MAGIC_POSITION *>(&pool.pos) );

		return result_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer::releaseEmitterId( const ConstString & _name, HM_EMITTER _id )
	{
		TMapEmitters::iterator it = m_emitters.find( _name );

		if( it == m_emitters.end() )
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
	void AstralaxEmitterContainer::addAtlas( const ParticleEmitterAtlas & _atlas )
	{
		m_atlas.push_back( _atlas );
	}
	//////////////////////////////////////////////////////////////////////////
	const TVectorParticleEmitterAtlas & AstralaxEmitterContainer::getAtlas() const
	{
		return m_atlas;
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitterInterface * AstralaxEmitterContainer::createEmitter( const ConstString & _name )
	{
		HM_EMITTER id = this->getEmitterId( _name );

		if( id == 0 )
		{
			return nullptr;
		}

		AstralaxEmitter * emitter = m_factoryPoolAstralaxEmitter.createObjectT();

        if( emitter->initialize( m_serviceProvider, this, id, _name ) == false )
        {
            emitter->destroy();

            return nullptr;
        }

		return emitter;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer::releaseEmitter( ParticleEmitterInterface * _emitter )
	{
		AstralaxEmitter * emitter = static_cast<AstralaxEmitter*>(_emitter);

		const ConstString & name = emitter->getName();
		HM_EMITTER id = emitter->getId();
		AstralaxEmitterContainer * container = emitter->getContainer();

		container->releaseEmitterId( name, id );

		emitter->destroy();
	}
	////////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer::visitContainer( ParticleEmitterContainerVisitor * visitor )
	{
		for( TMapEmitters::iterator 
			it = m_emitters.begin(), 
			it_end = m_emitters.end();
		it != it_end;
		++it )
		{
			visitor->visitEmitterName( it->first );
		}

		for (TVectorParticleEmitterAtlas::iterator
			it = m_atlas.begin(),
			it_end = m_atlas.end();
		it != it_end;
		++it)
		{
			visitor->visitAtlas( *(it) );
		}
	}
}