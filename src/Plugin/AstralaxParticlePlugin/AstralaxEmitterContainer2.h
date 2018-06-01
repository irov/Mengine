#pragma once

#include "Interface/ParticleSystemInterface.h"

#include "AstralaxEmitter2.h"

#include "Core/ServantBase.h"

#include "Config/Vector.h"

#include "Factory/Factory.h"

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(push, 0) 
#endif 

#include "magic.h"

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(pop) 
#endif

namespace Mengine
{
	class AstralaxParticleSystem2;

	class AstralaxEmitterContainer2
		: public ServantBase<ParticleEmitterContainerInterface2>
	{
	public:
		AstralaxEmitterContainer2();
		~AstralaxEmitterContainer2() override;

    public:
		bool initialize( AstralaxParticleSystem2 * _particleSystem, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator );
		void finalize();

    public:
        bool isValid() const override;

	public:
		uint32_t getId() const;

	public:
		void setAtlasResourceImage( uint32_t _index, const ResourceImagePtr & _resourceImage ) override;

	public:
		const ResourceImagePtr & getAtlasResourceImage( const char * _file ) const;

	public:
        HM_EMITTER createEmitterId() const;

	protected:
		bool loadContainer_( const unsigned char * _buffer, size_t _size, HM_FILE & _mf ) const;
					
    protected:
		AstralaxParticleSystem2 * m_particleSystem;

		HM_FILE m_mf;

		uint32_t m_id;
		
		MemoryInterfacePtr m_memory;

		typedef Vector<ResourceImagePtr> TVectorResourceImage;
		TVectorResourceImage m_resourceImages;
	};

	typedef IntrusivePtr<AstralaxEmitterContainer2> AstralaxEmitterContainer2Ptr;
}