#	include "AstralaxParticleSystem2.h"

#   include "Interface/StringizeInterface.h"

#   include "Core/String.h"
#   include "Logger/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( ParticleSystem2, Menge::ParticleSystemInterface2, Menge::AstralaxParticleSystem2 );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AstralaxParticleSystem2::AstralaxParticleSystem2()
        : m_serviceProvider(nullptr)
		, m_stageCount(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AstralaxParticleSystem2::~AstralaxParticleSystem2()
	{	
	}
    //////////////////////////////////////////////////////////////////////////
    void AstralaxParticleSystem2::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * AstralaxParticleSystem2::getServiceProvider() const
    {
        return m_serviceProvider;
    }
	//////////////////////////////////////////////////////////////////////////
	bool AstralaxParticleSystem2::initialize()
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

		Magic_SetAxis( MAGIC_pXpYpZ );

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
	void AstralaxParticleSystem2::finalize()
	{
		uint32_t count = m_factoryPoolAstralaxEmitterContainer.countObject();

		if( count != 0 )
		{
			LOGGER_ERROR( m_serviceProvider )("AstralaxParticleSystem2::finalize have %d container"
				, count
				);
		}
		else
		{
			Magic_UnloadAllEmitters();
			Magic_DestroyAll();
		}				
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

		return container;
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

			RenderStage rs;
						
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

			const RenderStage * cache_stage = RENDERMATERIAL_SERVICE( m_serviceProvider )
				->cacheStage( rs );

			m_stages[i] = cache_stage;
		}

		m_stageCount = materialCount;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderStage * AstralaxParticleSystem2::getMaterialStage( int _index ) const
	{
		if( _index >= m_stageCount )
		{
			return nullptr;
		}

		return m_stages[_index];
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticleSystem2::onContainerRelease_( AstralaxEmitterContainer2 * _contanier )
	{
		_contanier->finalize();
	}
}