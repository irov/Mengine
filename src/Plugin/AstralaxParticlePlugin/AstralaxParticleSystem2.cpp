#	include "AstralaxParticleSystem2.h"

#   include "Interface/StringizeInterface.h"

#   include "Core/String.h"
#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ParticleSystem, Menge::AstralaxParticleSystem2 );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxParticleSystem2::AstralaxParticleSystem2()
        : m_stageCount(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AstralaxParticleSystem2::~AstralaxParticleSystem2()
	{	
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticleSystem2::_initialize()
	{
		bool states[17];
		states[MAGIC_RENDER_STATE_BLENDING] = false;
		states[MAGIC_RENDER_STATE_TEXTURE_COUNT] = false;
		states[MAGIC_RENDER_STATE_TEXTURE] = true;
		states[MAGIC_RENDER_STATE_ADDRESS_U] = false;
		states[MAGIC_RENDER_STATE_ADDRESS_V] = false;
		states[MAGIC_RENDER_STATE_OPERATION_RGB] = false;
		states[MAGIC_RENDER_STATE_ARGUMENT1_RGB] = false;
		states[MAGIC_RENDER_STATE_ARGUMENT2_RGB] = false;
		states[MAGIC_RENDER_STATE_OPERATION_ALPHA] = false;
		states[MAGIC_RENDER_STATE_ARGUMENT1_ALPHA] = false;
		states[MAGIC_RENDER_STATE_ARGUMENT2_ALPHA] = false;
		states[MAGIC_RENDER_STATE_ZENABLE] = false;
		states[MAGIC_RENDER_STATE_ZWRITE] = false;
		states[MAGIC_RENDER_STATE_ALPHATEST_INIT] = false;
		states[MAGIC_RENDER_STATE_ALPHATEST] = false;
		states[MAGIC_RENDER_STATE_TECHNIQUE_ON] = false;
		states[MAGIC_RENDER_STATE_TECHNIQUE_OFF] = false;

		Magic_SetRenderStateFilter( states, false );

		Magic_EnableZBuffer( false );

		Magic_SetAxis( MAGIC_pXnYpZ );

		const char * version = Magic_GetVersion();

		if( strstr( version, MAGIC_API ) == nullptr )
		{
			LOGGER_CRITICAL( m_serviceProvider )("Astralax Magic Particles Version: %s but need %s"
				, version
				, MAGIC_API
				);

			return false;
		}

		LOGGER_WARNING( m_serviceProvider )("Astralax Magic Particles Version: %s"
			, version
			);

		m_factoryPoolAstralaxEmitterContainer.setMethodListener( this, &AstralaxParticleSystem2::onContainerRelease_ );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticleSystem2::_finalize()
	{
		uint32_t count = m_factoryPoolAstralaxEmitterContainer.countObject();

		if( count != 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("AstralaxParticleSystem2::finalize have %d container"
				, count
				);
		}

		m_atlases.clear();
		m_containers.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitterContainerInterface2Ptr AstralaxParticleSystem2::createEmitterContainerFromMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator )
	{
		AstralaxEmitterContainer2Ptr container = m_factoryPoolAstralaxEmitterContainer.createObject();

		if( container == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("AstralaxParticleSystem::createEmitterContainerFromMemory invalid create container"
				);

			return nullptr;
		}

		container->setServiceProvider( m_serviceProvider );
		
		if( container->initialize( this, _stream, _archivator ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::createEmitterContainerFromMemory invalid initialize container"
				);
			
			return nullptr;
		}

		uint32_t id = container->getId();

		m_containers[id] = container.get();

		return container;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticleSystem2::updateAtlas()
	{
		MAGIC_CHANGE_ATLAS c;
		while( Magic_GetNextAtlasChange( &c ) == MAGIC_SUCCESS )
		{
			switch( c.type )
			{
			case MAGIC_CHANGE_ATLAS_LOAD:
				{
				}break;
			case MAGIC_CHANGE_ATLAS_CLEAN:
				{
				}break;
			case MAGIC_CHANGE_ATLAS_CREATE:
				{
					AstralaxEmitterContainer2 * container = m_containers[c.ptc_id];

					const ResourceImagePtr & resourceImage = container->getAtlasResourceImage( c.file );

					m_atlases.push_back( resourceImage );
				}break;
			case MAGIC_CHANGE_ATLAS_DELETE:
				{
					TVectorAtlasDesc::iterator it_remove = m_atlases.begin();
					std::advance( it_remove, c.index );
					m_atlases.erase( it_remove );
				}break;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticleSystem2::updateMaterial()
	{
		int materialCount = Magic_GetMaterialCount();

		for( int i = m_stageCount; i != materialCount; ++i )
		{
			MAGIC_MATERIAL m;
			if( Magic_GetMaterial( i, &m ) != MAGIC_SUCCESS )
			{
				LOGGER_ERROR( m_serviceProvider )("AstralaxParticleSystem2::updateMaterial invalid get material %d"
					, i
					);

				return;
			}

			RenderMaterialStage rs;
						
			switch( m.blending )
			{
			case MAGIC_BLENDING_NORMAL:
				{
					rs.alphaBlendEnable = true;
					rs.blendSrc = BF_SOURCE_ALPHA;
					rs.blendDst = BF_ONE_MINUS_SOURCE_ALPHA;
				}break;
			case MAGIC_BLENDING_ADD:
				{
					rs.alphaBlendEnable = true;
					rs.blendSrc = BF_SOURCE_ALPHA;
					rs.blendDst = BF_ONE;
				}break;
			case MAGIC_BLENDING_OPACITY:
				{
					rs.alphaBlendEnable = false;
				}break;
			}

			for( int stage = 0; stage != m.textures; ++stage )
			{
				const MAGIC_TEXTURE_STATES & state = m.states[stage];

				const ETextureAddressMode dx_address[] = {TAM_WRAP, TAM_MIRROR, TAM_CLAMP, TAM_BORDER};

				RenderTextureStage & textureStage = rs.textureStage[stage];

				textureStage.addressU = dx_address[state.address_u];
				textureStage.addressV = dx_address[state.address_v];

				const ETextureOp dx_operation[] = {TOP_SELECTARG1, TOP_ADD, TOP_SUBTRACT, TOP_MODULATE, TOP_MODULATE2X, TOP_MODULATE4X};
				const ETextureArgument dx_arg[] = {TARG_CURRENT, TARG_DIFFUSE, TARG_TEXTURE};

				textureStage.colorOp = dx_operation[state.operation_rgb];
				textureStage.colorArg1 = dx_arg[state.argument_rgb1];
				textureStage.colorArg2 = dx_arg[state.argument_rgb2];

				textureStage.alphaOp = dx_operation[state.operation_alpha];
				textureStage.alphaArg1 = dx_arg[state.argument_alpha1];
				textureStage.alphaArg2 = dx_arg[state.argument_alpha2];
			}

			const RenderMaterialStage * cache_stage = RENDERMATERIAL_SERVICE( m_serviceProvider )
				->cacheStage( rs );

			m_stages[i] = cache_stage;
		}

		m_stageCount = materialCount;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderMaterialStage * AstralaxParticleSystem2::getMaterialStage( int _index ) const
	{
		if( _index >= m_stageCount )
		{
			return nullptr;
		}

		return m_stages[_index];
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceImagePtr & AstralaxParticleSystem2::getResourceImage( int _index ) const
	{
		size_t atlases_size = m_atlases.size();

		if( atlases_size <= (size_t)_index )
		{
			LOGGER_ERROR( m_serviceProvider )("AstralaxParticleSystem2::getResourceImage index %d but size is %d"
				, _index
				, atlases_size
				);

			return ResourceImagePtr::none();
		}

		const ResourceImagePtr & resourceImage = m_atlases[_index];

		return resourceImage;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticleSystem2::onContainerRelease_( AstralaxEmitterContainer2 * _container )
	{
		uint32_t id = _container->getId();

		m_containers.erase( id );

		_container->finalize();
	}
}