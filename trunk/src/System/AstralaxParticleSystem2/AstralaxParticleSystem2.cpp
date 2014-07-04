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
	ParticleEmitterContainerInterface2Ptr AstralaxParticleSystem2::createEmitterContainerFromMemory( const ConstString & _name, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator )
	{
		AstralaxEmitterContainer2Ptr container = m_factoryPoolAstralaxEmitterContainer.createObjectT();

		container->setServiceProvider( m_serviceProvider );

		if( container->initialize( _name, _stream, _archivator ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AstralaxParticleSystem::createEmitterContainerFromMemory invalid initialize container %s"
				, _name.c_str()
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