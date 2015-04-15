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
		for( TVectorEmitterDescs::iterator
			it = m_emitters.begin(),
			it_end = m_emitters.end();
		it != it_end;
		++it )
		{
			EmitterDesc & desc = *it;

			Magic_UnloadEmitter( desc.id );

			for( TVectorEmitters::const_iterator
				it_pool = desc.emitters.begin(),
				it_pool_end = desc.emitters.end();
			it_pool != it_pool_end;
			++it_pool )
			{
				HM_EMITTER emitter_id = *it_pool;

				Magic_UnloadEmitter( emitter_id );
			}
		};
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
    bool AstralaxEmitterContainer::initialize()
    {
		m_factoryPoolAstralaxEmitter.setMethodListener( this, &AstralaxEmitterContainer::onEmitterDestroy_ );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitterContainer::isValid() const
    {
		for( TVectorEmitterDescs::const_iterator
			it = m_emitters.begin(),
			it_end = m_emitters.end();
		it != it_end;
		++it )
		{
			const EmitterDesc & desc = *it;
			
			HM_EMITTER id = Magic_DuplicateEmitter( desc.id );

			MAGIC_RECT rect;
			float backgroundScale = Magic_GetBackgroundRect( id, &rect );

			if( mt::equal_f_f( backgroundScale, 1.f ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("AstralaxEmitterContainer::isValid emitter %s background scale is not 1.f (%f if is zero, add background!) Please remove scale from source and re-export!"
					, desc.name
					, backgroundScale
					);

				return false;
			}

			Magic_UnloadEmitter( id );
		}
	
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	const AstralaxEmitterContainer::EmitterDesc * AstralaxEmitterContainer::findEmitterDesc_( const char * _name ) const
	{
		for( TVectorEmitterDescs::const_iterator
			it = m_emitters.begin(),
			it_end = m_emitters.end();
		it != it_end;
		++it )
		{
			const EmitterDesc & desc = *it;

			if( strcmp( desc.name, _name) != 0 )
			{
				continue;
			}

			return &desc;			
		}

		return nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitterContainer::addEmitterIds( const char * _name, HM_EMITTER _id, MAGIC_POSITION _pos, const TVectorEmitters & _emitters )
	{
		const EmitterDesc * find_desc = this->findEmitterDesc_( _name );

		if( find_desc != nullptr )
		{
			return false;
		}

		if( strlen(_name) >= MENGINE_ASTRALAX_EMITTER_NAME_MAX )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxEmitterContainer::addEmitterIds emitters name '%s' more %d len"
				, _name
				, MENGINE_ASTRALAX_EMITTER_NAME_MAX
				);

			return false;
		}

		EmitterDesc new_desc;
		
		strcpy( new_desc.name, _name );
        new_desc.id = _id;
        new_desc.pos = _pos;
		new_desc.emitters = _emitters;
		new_desc.dublicate = (_emitters.empty() == true);

		m_emitters.push_back( new_desc );

		return true;			 
	}
	//////////////////////////////////////////////////////////////////////////
	HM_EMITTER AstralaxEmitterContainer::getEmitterId( const char * _name ) const
	{
		const EmitterDesc * desc = this->findEmitterDesc_( _name );

		if( desc == nullptr )
		{
			return 0;
		}

		HM_EMITTER result_id;

		if( desc->dublicate == true )
		{
			HM_EMITTER duplicated_id = Magic_DuplicateEmitter( desc->id );

			result_id = duplicated_id;
		}
		else
		{
			if( desc->emitters.empty() == true )
			{
				return 0;
			}

			HM_EMITTER pool_id = desc->emitters.back();
			desc->emitters.pop_back();

			result_id = pool_id;
		}

        Magic_SetEmitterPosition( result_id, &desc->pos );

		return result_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer::releaseEmitterId( const char * _name, HM_EMITTER _id )
	{
		const EmitterDesc * desc = this->findEmitterDesc_( _name );

		if( desc == nullptr )
		{
			return;
		}

		if( desc->dublicate == true )
		{
			Magic_UnloadEmitter( _id );
		}
		else
		{
			desc->emitters.push_back( _id );
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
	ParticleEmitterInterfacePtr AstralaxEmitterContainer::createEmitter( const ConstString & _emitterName )
	{
		HM_EMITTER id = this->getEmitterId( _emitterName.c_str() );

		if( id == 0 )
		{
			return nullptr;
		}

		AstralaxEmitter * emitter = m_factoryPoolAstralaxEmitter.createObjectT();

        if( emitter->initialize( m_serviceProvider, this, id, _emitterName.c_str() ) == false )
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
	void AstralaxEmitterContainer::visitContainer( ParticleEmitterContainerVisitor * _visitor )
	{
		for( TVectorEmitterDescs::const_iterator
			it = m_emitters.begin(),
			it_end = m_emitters.end();
		it != it_end;
		++it)
		{
			const EmitterDesc & desc = *it;

			_visitor->visitEmitterName( desc.name );
		}
				
		for( TVectorParticleEmitterAtlas::const_iterator
			it = m_atlas.begin(),
			it_end = m_atlas.end();
		it != it_end;
		++it)
		{
			const ParticleEmitterAtlas & atlas = *it;

			_visitor->visitAtlas( atlas );
		}
	}
}