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
		: public ServiceBase<ParticleSystemInterface2>
	{
	public:
		AstralaxParticleSystem2();
		~AstralaxParticleSystem2();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		ParticleEmitterContainerInterface2Ptr createEmitterContainerFromMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator ) override;

	public:
		const RenderStage * getMaterialStage( int _index ) const override;

	public:
		void updateMaterial();

	protected:
		void onContainerRelease_( AstralaxEmitterContainer2 * _contanier );

    protected:
        typedef FactoryPoolStore<AstralaxEmitterContainer2, 16> TFactoryPoolAstralaxEmitterContainer;
        TFactoryPoolAstralaxEmitterContainer m_factoryPoolAstralaxEmitterContainer;

		int m_stageCount;
		const RenderStage * m_stages[256];
	};
}