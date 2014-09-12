#	include "AstralaxEmitterContainer2.h"

#	include "Interface/CacheInterface.h"

#   include "Logger/Logger.h"

#	include "Core/String.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitterContainer2::AstralaxEmitterContainer2()
        : m_serviceProvider(nullptr)
		, m_mf(0)
		, m_bufferId(0)
	{
	};
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitterContainer2::~AstralaxEmitterContainer2()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer2::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * AstralaxEmitterContainer2::getServiceProvider() const 
	{
		return m_serviceProvider;
	}
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitterContainer2::initialize( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator )
    {
		unsigned char * binary_memory;
		size_t binary_size;

		m_bufferId = CACHE_SERVICE(m_serviceProvider)
			->getArchiveData( _stream, _archivator, GET_MAGIC_NUMBER(MAGIC_PTZ), GET_MAGIC_VERSION(MAGIC_PTZ), &binary_memory, binary_size );

		if( m_bufferId == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxEmitterContainer2::initialize: invalid get data"
				);

			return false;
		}

		HM_FILE mf;
		if( this->loadContainer_( binary_memory, binary_size, mf ) == false )
		{
			CACHE_SERVICE(m_serviceProvider)
				->unlockBuffer( m_bufferId );
			m_bufferId = 0;

			return false;
		}

		m_mf = mf;

		m_factoryPoolAstralaxEmitter.setMethodListener( this, &AstralaxEmitterContainer2::onEmitterRelease_ );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer2::finalize()
	{
		CACHE_SERVICE(m_serviceProvider)
			->unlockBuffer( m_bufferId );
		m_bufferId = 0;

		Magic_CloseFile( m_mf );
		m_mf = 0;
	}
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitterContainer2::isValid() const
    {
        //for( TMapEmitters::const_iterator
        //    it = m_emitters.begin(),
        //    it_end = m_emitters.end();
        //it != it_end;
        //++it )
        //{
        //    const ConstString & emitterName = it->first;

        //    const EmitterPool & emitter = it->second;

        //    HM_EMITTER id = Magic_DuplicateEmitter( emitter.id );

        //    MAGIC_RECT rect;
        //    float backgroundScale = Magic_GetBackgroundRect( id, &rect );

        //    if( fabsf( backgroundScale - 1.f ) > 0.0001f )
        //    {
        //        LOGGER_ERROR(m_serviceProvider)("AstralaxEmitterContainer::isValid %s emitter %s background scale is not 1.f (%f if is zero, add background!) Please remove scale from source and re-export!"
        //            , m_name.c_str()
        //            , emitterName.c_str()
        //            , backgroundScale
        //            );

        //        return false;
        //    }

        //    Magic_UnloadEmitter( id );
        //}

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitterContainer2::loadContainer_( unsigned char * _buffer, size_t _size, HM_FILE & _mf ) const
	{
		(void)_size;

		HM_FILE mf = Magic_OpenFileInMemory( reinterpret_cast<const char *>(_buffer) );

		if( mf == MAGIC_ERROR )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::createEmitterContainerFromMemory invalid open file in memory (alredy open)"
				);

			return false;
		}

		if( mf == MAGIC_UNKNOWN )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::createEmitterContainerFromMemory invalid open file in memory (invalid format or version)"
				);

			return false;
		}

		_mf = mf;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	HM_EMITTER AstralaxEmitterContainer2::createEmitterId_() const
	{
		MAGIC_FIND_DATA find;
		const char * magicName = Magic_FindFirst( m_mf, &find, MAGIC_FOLDER | MAGIC_EMITTER );

		while( magicName != nullptr )
		{
			if( find.animate == 1 )
			{
				HM_EMITTER id = Magic_LoadEmitter( m_mf, magicName );

				if( id == 0 )
				{
					LOGGER_ERROR(m_serviceProvider)("AstralaxEmitterContainer2::createEmitterId invalid load emitter %s"
						, magicName
						);

					return 0;
				}

				return id;
			}
			else
			{		
				const char * currentFolder = Magic_GetCurrentFolder( m_mf );
				Magic_SetCurrentFolder( m_mf, magicName );

				HM_EMITTER id = this->createEmitterId_();
				
				Magic_SetCurrentFolder( m_mf, currentFolder );

				if( id != 0 )
				{
					return id;
				}
			}

			magicName = Magic_FindNext( m_mf, &find );
		}

		LOGGER_ERROR(m_serviceProvider)("AstralaxEmitterContainer2::createEmitterId not found emitter"
			);

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitterInterfacePtr AstralaxEmitterContainer2::createEmitter()
	{
		HM_EMITTER id = this->createEmitterId_();

		if( id == 0 )
		{
			return nullptr;
		}

		AstralaxEmitter2Ptr emitter = m_factoryPoolAstralaxEmitter.createObjectT();

        if( emitter->initialize( m_serviceProvider, id ) == false )
        {
            return nullptr;
        }

		return emitter;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer2::onEmitterRelease_( AstralaxEmitter2 * _emitter )
	{
		_emitter->finalize();
	}
}