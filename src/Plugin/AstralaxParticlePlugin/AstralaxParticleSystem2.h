#pragma once

#include "Interface/ParticleSystemInterface.h"

#include "Core/ServiceBase.h"

#include "Config/String.h"

#include "AstralaxEmitterContainer2.h"
#include "AstralaxEmitter2.h"

#include "Factory/Factory.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(push, 0) 
#	endif 

#include "magic.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(pop) 
#	endif

#include "stdex/stl_map.h"

namespace Mengine
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
		const RenderMaterialStage * getMaterialStage( int _index ) const override;
		const ResourceImagePtr & getResourceImage( int _index ) const override;

	public:
		bool updateAtlas();
		void updateMaterial();

	protected:
		void onContainerRelease_( AstralaxEmitterContainer2 * _container );

    protected:        
        FactoryPtr m_factoryPoolAstralaxEmitterContainer;

		typedef stdex::map<uint32_t, AstralaxEmitterContainer2 *> TMapHashEmitterContainers;
		TMapHashEmitterContainers m_containers;

		int m_stageCount;
		const RenderMaterialStage * m_stages[256];

		typedef stdex::vector<ResourceImagePtr> TVectorAtlasDesc;
		TVectorAtlasDesc m_atlases;
	};
}