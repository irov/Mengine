#	include "AstralaxEmitterContainer.h"

#	include "AstralaxEmitter.h"

#   include "Logger/Logger.h"

#	include "Core/String.h"

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
	void AstralaxEmitterContainer::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * AstralaxEmitterContainer::getServiceProvider() const
	{
		return m_serviceProvider;
	}
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitterContainer::initialize( const ConstString & _name )
    {
        m_name = _name;

		m_factoryPoolAstralaxEmitter.setMethodListener( this, &AstralaxEmitterContainer::onEmitterDestroy_ );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitterContainer::isValid() const
    {
        for( TMapEmitters::const_iterator
            it = m_emitters.begin(),
            it_end = m_emitters.end();
        it != it_end;
        ++it )
        {
            const ConstString & emitterName = it->first;

            const EmitterPool & emitter = it->second;

            HM_EMITTER id = Magic_DuplicateEmitter( emitter.id );

            MAGIC_RECT rect;
            float backgroundScale = Magic_GetBackgroundRect( id, &rect );

            if( fabsf( backgroundScale - 1.f ) > 0.0001f )
            {
                LOGGER_ERROR(m_serviceProvider)("AstralaxEmitterContainer::isValid %s emitter %s background scale is not 1.f (%f if is zero, add background!) Please remove scale from source and re-export!"
                    , m_name.c_str()
                    , emitterName.c_str()
                    , backgroundScale
                    );

                return false;
            }

            Magic_UnloadEmitter( id );
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitterContainer::addEmitterIds( const ConstString & _name, HM_EMITTER _id, MAGIC_POSITION _pos, const TVectorEmitters & _emitters )
	{
		TMapEmitters::iterator it_found = m_emitters.find( _name );

		if( it_found != m_emitters.end() )
		{
			return false;
		}

		EmitterPool pool;

        pool.id = _id;
        pool.pos = _pos;
		pool.emitters = _emitters;
		pool.dublicate = (_emitters.empty() == true);

		m_emitters.insert( std::make_pair( _name, pool ) );

		return true;			 
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
	ParticleEmitterInterfacePtr AstralaxEmitterContainer::createEmitter( const ConstString & _name )
	{
		HM_EMITTER id = this->getEmitterId( _name );

		if( id == 0 )
		{
			return nullptr;
		}

		AstralaxEmitter * emitter = m_factoryPoolAstralaxEmitter.createObjectT();

        if( emitter->initialize( m_serviceProvider, this, id, _name ) == false )
        {
            return nullptr;
        }

		return emitter;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer::onEmitterDestroy_( AstralaxEmitter * _emitter )
	{
		AstralaxEmitter * emitter = static_cast<AstralaxEmitter*>(_emitter);

		emitter->finalize();
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