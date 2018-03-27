#include "AstralaxEmitterContainer2.h"
#include "AstralaxParticleSystem2.h"

#include "Interface/MemoryInterface.h"

#include "Logger/Logger.h"

#include "Core/String.h"
#include "Core/Stream.h"

#include "Kernel/ResourceImage.h"

#include "Factory/FactoryPool.h"
#include "Factory/FactoryPoolWithListener.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitterContainer2::AstralaxEmitterContainer2()
        : m_particleSystem(nullptr)
		, m_mf(0)
		, m_id(0)
	{
	};
	//////////////////////////////////////////////////////////////////////////
	AstralaxEmitterContainer2::~AstralaxEmitterContainer2()
	{
	}
    //////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitterContainer2::initialize( AstralaxParticleSystem2 * _particleSystem, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator )
    {
		m_particleSystem = _particleSystem;

		MemoryInterfacePtr memory = Helper::loadStreamArchiveMagicMemory( _stream, _archivator, GET_MAGIC_NUMBER( MAGIC_PTZ ), GET_MAGIC_VERSION( MAGIC_PTZ ), __FILE__, __LINE__ );

		if( memory == nullptr )
		{
			LOGGER_ERROR("AstralaxEmitterContainer2::initialize: invalid get data"
				);

			return false;
		}
				
		const unsigned char * binary_memory = memory->getMemory();
		size_t binary_size = memory->getSize();

		HM_FILE mf;
		if( this->loadContainer_( binary_memory, binary_size, mf ) == false )
		{
			return false;
		}

		m_mf = mf;

		m_memory = memory;

		int atlasCount = Magic_GetStaticAtlasCount( m_mf );

		if( atlasCount > 0 )
		{
			MAGIC_STATIC_ATLAS atlas;
			Magic_GetStaticAtlas( m_mf, 0, &atlas );

			m_id = atlas.ptc_id;
		}

		m_resourceImages.resize( atlasCount );
                
        m_factoryPoolAstralaxEmitter = Helper::makeFactoryPoolWithListener<AstralaxEmitter2, 16>( this, &AstralaxEmitterContainer2::onEmitterRelease_ );
        		
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer2::finalize()
	{
		Magic_CloseFile( m_mf );
		m_mf = 0;
		
		m_memory = nullptr;

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryPoolAstralaxEmitter );

        m_factoryPoolAstralaxEmitter = nullptr;
	}
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitterContainer2::isValid() const
    {
		if( Magic_HasTextures( m_mf ) == true )
		{
			LOGGER_ERROR("AstralaxEmitterContainer2::isValid: particle textures are stored within the file"
				);

			return false;
		}

		return true;
    }
	//////////////////////////////////////////////////////////////////////////
	uint32_t AstralaxEmitterContainer2::getId() const
	{ 
		return m_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxEmitterContainer2::setAtlasResourceImage( size_t _index, const ResourceImagePtr & _resourceImage )
	{
		m_resourceImages[_index] = _resourceImage;
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceImagePtr & AstralaxEmitterContainer2::getAtlasResourceImage( const char * _file ) const
	{
		int atlasCount = Magic_GetStaticAtlasCount( m_mf );
	
		for( int i = 0; i != atlasCount; ++i )
		{
			MAGIC_STATIC_ATLAS atlas;
			Magic_GetStaticAtlas( m_mf, i, &atlas );

			if( strcmp( atlas.file, _file ) != 0 )
			{
				continue;
			}
		
            const ResourceImagePtr & image = m_resourceImages[i];

            return image;
		}

		LOGGER_ERROR("AstralaxEmitterContainer2::getAtlasResourceImage: not found atlas %s"
			, _file
			);

		return ResourceImagePtr::none();
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxEmitterContainer2::loadContainer_( const unsigned char * _buffer, size_t _size, HM_FILE & _mf ) const
	{
		(void)_size;

		HM_FILE mf = Magic_OpenFileInMemory( reinterpret_cast<const char *>(_buffer) );

		if( mf == MAGIC_ERROR )
		{
			LOGGER_ERROR("AstralaxParticleSystem::createEmitterContainerFromMemory invalid open file in memory (alredy open)"
				);

			return false;
		}

		if( mf == MAGIC_UNKNOWN )
		{
			LOGGER_ERROR("AstralaxParticleSystem::createEmitterContainerFromMemory invalid open file in memory (invalid format or version)"
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
					LOGGER_ERROR("AstralaxEmitterContainer2::createEmitterId invalid load emitter %s"
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

		LOGGER_ERROR("AstralaxEmitterContainer2::createEmitterId not found emitter"
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

		AstralaxEmitter2Ptr emitter = m_factoryPoolAstralaxEmitter->createObject();
        
		if( emitter->initialize( m_particleSystem, this, id ) == false )
		{
			return nullptr;
		}

        if( m_particleSystem->updateMaterial() == false )
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