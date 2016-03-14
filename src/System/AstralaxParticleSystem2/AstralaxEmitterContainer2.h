#	pragma once

#	include "Interface/ParticleSystemInterface.h"

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
	class AstralaxParticleSystem2;

	class AstralaxEmitterContainer2
		: public ParticleEmitterContainerInterface2
	{
	public:
		AstralaxEmitterContainer2();
		~AstralaxEmitterContainer2();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

    public:
		bool initialize( AstralaxParticleSystem2 * _particleSystem, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator );
		void finalize();

    public:
        bool isValid() const override;

	public:
		uint32_t getId() const;

	public:
		void setAtlasResourceImage( size_t _index, const ResourceImagePtr & _resourceImage ) override;

	public:
		const ResourceImagePtr & getAtlasResourceImage( const char * _file ) const;

	public:
		ParticleEmitterInterfacePtr createEmitter() override;

	protected:
		void onEmitterRelease_( AstralaxEmitter2 * _emitter );

	protected:
		bool loadContainer_( const unsigned char * _buffer, size_t _size, HM_FILE & _mf ) const;
		HM_EMITTER createEmitterId_() const;
			
	private:
        ServiceProviderInterface * m_serviceProvider;

		AstralaxParticleSystem2 * m_particleSystem;

		HM_FILE m_mf;

		uint32_t m_id;
		
		MemoryInterfacePtr m_memory;

		typedef FactoryPoolStore<AstralaxEmitter2, 16> TFactoryPoolAstralaxEmitter;
		TFactoryPoolAstralaxEmitter m_factoryPoolAstralaxEmitter;

		typedef stdex::vector<ResourceImagePtr> TVectorResourceImage;
		TVectorResourceImage m_resourceImages;
	};

	typedef stdex::intrusive_ptr<AstralaxEmitterContainer2> AstralaxEmitterContainer2Ptr;
}