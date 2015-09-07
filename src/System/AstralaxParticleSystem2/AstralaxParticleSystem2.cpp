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

		m_factoryPoolAstralaxEmitterContainer.setMethodListener( this, &AstralaxParticleSystem2::onContainerRelease_ );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticleSystem2::finalize()
	{
		Magic_DestroyAll();
	}
	//////////////////////////////////////////////////////////////////////////
	ParticleEmitterContainerInterface2Ptr AstralaxParticleSystem2::createEmitterContainerFromMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator )
	{
		AstralaxEmitterContainer2Ptr container = m_factoryPoolAstralaxEmitterContainer.createObjectT();

		if( container == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("AstralaxParticleSystem::createEmitterContainerFromMemory invalid create container"
				);

			return nullptr;
		}

		container->setServiceProvider( m_serviceProvider );

		if( container->initialize( _stream, _archivator ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::createEmitterContainerFromMemory invalid initialize container"
				);
			
			return nullptr;
		}

		return container;
	}
	//////////////////////////////////////////////////////////////////////////
	void AstralaxParticleSystem2::onContainerRelease_( AstralaxEmitterContainer2 * _contanier )
	{
		_contanier->finalize();
	}
}