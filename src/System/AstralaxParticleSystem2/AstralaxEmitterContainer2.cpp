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
    bool AstralaxEmitterContainer2::initialize( const ConstString & _name, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator )
    {
		unsigned char * binary_memory;
		size_t binary_size;

		uint32_t bufferId = CACHE_SERVICE(m_serviceProvider)
			->getArchiveData( _stream, _archivator, GET_MAGIC_NUMBER(MAGIC_PTZ), GET_MAGIC_VERSION(MAGIC_PTZ), &binary_memory, binary_size );

		if( bufferId == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxEmitterContainer2::initialize: %s invalid get data"
				, _name.c_str()
				);

			return false;
		}

		m_mf = Magic_OpenFileInMemory( reinterpret_cast<const char *>(binary_memory) );

		CACHE_SERVICE(m_serviceProvider)
			->unlockBuffer( bufferId );

		if( m_mf == MAGIC_ERROR )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::createEmitterContainerFromMemory %s invalid open file in memory (alredy open)"
				, _name.c_str()
				);

			return nullptr;
		}

		if( m_mf == MAGIC_UNKNOWN )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::createEmitterContainerFromMemory %s invalid open file in memory (invalid format or version)"
				, _name.c_str()
				);

			return nullptr;
		}

		m_factoryPoolAstralaxEmitter.setMethodListener( this, &AstralaxEmitterContainer2::onEmitterRelease_ );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer2::finalize()
	{
		Magic_CloseFile( m_mf );
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
	HM_EMITTER AstralaxEmitterContainer2::createEmitterId_( const char * _name ) const
	{
		MAGIC_FIND_DATA find;
		const char* magicName = Magic_FindFirst( m_mf, &find, MAGIC_FOLDER | MAGIC_EMITTER );

		while( magicName != nullptr )
		{
			if( find.animate == 1 )
			{
				if( strcmp( magicName, _name ) == 0 )
				{
					HM_EMITTER id = Magic_LoadEmitter( m_mf, magicName );

					if( id == 0 )
					{
						LOGGER_ERROR(m_serviceProvider)("AstralaxEmitterContainer2::createEmitterId invalid load emitter %s"
							, _name
							);

						return 0;
					}

					return id;
				}
			}
			else
			{		
				const char * currentFolder = Magic_GetCurrentFolder( m_mf );
				Magic_SetCurrentFolder( m_mf, magicName );

				HM_EMITTER id = this->createEmitterId_( _name );
				
				Magic_SetCurrentFolder( m_mf, currentFolder );

				if( id != 0 )
				{
					return id;
				}
			}

			magicName = Magic_FindNext( m_mf, &find );
		}

		LOGGER_ERROR(m_serviceProvider)("AstralaxEmitterContainer2::createEmitterId not found emitter %s"
			, _name
			);

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitterInterfacePtr AstralaxEmitterContainer2::createEmitter( const ConstString & _name )
	{
		HM_EMITTER id = this->createEmitterId_( _name.c_str() );

		if( id == 0 )
		{
			return nullptr;
		}

		AstralaxEmitter2Ptr emitter = m_factoryPoolAstralaxEmitter.createObjectT();

        if( emitter->initialize( m_serviceProvider, id, _name ) == false )
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