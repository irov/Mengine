#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#	include "Config/String.h"

#	include "AstralaxEmitterContainer2.h"
#	include "AstralaxEmitter2.h"

#   include "Factory/FactoryStore.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(push, 0) 
#	endif 

#	include "magic.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(pop) 
#	endif

namespace Menge
{
	class AstralaxParticleSystem2 
		: public ParticleSystemInterface2
	{
	public:
		AstralaxParticleSystem2();
		~AstralaxParticleSystem2();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

	public:
		bool initialize() override;
		void finalize() override;

	public:
		ParticleEmitterContainerInterface2Ptr createEmitterContainerFromMemory( const ConstString & _name, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator ) override;

    protected:
        ServiceProviderInterface * m_serviceProvider;

        typedef FactoryPoolStore<AstralaxEmitterContainer2, 16> TFactoryPoolAstralaxEmitterContainer;
        TFactoryPoolAstralaxEmitterContainer m_factoryPoolAstralaxEmitterContainer;
	};
}