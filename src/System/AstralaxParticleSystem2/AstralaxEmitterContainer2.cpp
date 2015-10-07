#	include "AstralaxEmitterContainer2.h"
#	include "AstralaxParticleSystem2.h"

#	include "Interface/MemoryInterface.h"

#   include "Logger/Logger.h"

#	include "Core/String.h"
#	include "Core/Stream.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitterContainer2::AstralaxEmitterContainer2()
        : m_serviceProvider(nullptr)
		, m_particleSystem(nullptr)
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
	bool AstralaxEmitterContainer2::initialize( AstralaxParticleSystem2 * _particleSystem, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator )
    {
		m_particleSystem = _particleSystem;

		MemoryInterfacePtr memory = Helper::loadStreamArchiveMemory( m_serviceProvider, _stream, _archivator, GET_MAGIC_NUMBER( MAGIC_PTZ ), GET_MAGIC_VERSION( MAGIC_PTZ ) );

		if( memory == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxEmitterContainer2::initialize: invalid get data"
				);

			return false;
		}

		size_t binary_size;
		unsigned char * binary_memory = memory->getMemoryT<unsigned char *>( binary_size );

		HM_FILE mf;
		if( this->loadContainer_( binary_memory, binary_size, mf ) == false )
		{
			return false;
		}

		m_mf = mf;

		m_memory = memory;

		m_factoryPoolAstralaxEmitter.setMethodListener( this, &AstralaxEmitterContainer2::onEmitterRelease_ );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer2::finalize()
	{
		Magic_CloseFile( m_mf );
		m_mf = 0;

		m_memory = nullptr;
	}
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitterContainer2::isValid() const
    {
		if( Magic_HasTextures( m_mf ) == true )
		{
			LOGGER_ERROR( m_serviceProvider )("AstralaxEmitterContainer2::isValid: particle textures are stored within the file"
				);

			return false;
		}

		int atlasCount = Magic_GetStaticAtlasCount( m_mf );

		if( atlasCount > 1 )
		{
			LOGGER_ERROR( m_serviceProvider )("AstralaxEmitterContainer2::isValid: particle has many static atlas, sorry need one :("
				);

			return false;
		}

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

		m_particleSystem->updateMaterial();

		return emitter;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer2::onEmitterRelease_( AstralaxEmitter2 * _emitter )
	{
		_emitter->finalize();
	}
}