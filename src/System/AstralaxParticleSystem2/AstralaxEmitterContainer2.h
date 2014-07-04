#	pragma once

#	include "Interface/ParticleSystemInterface2.h"

#	include "AstralaxEmitter2.h"

#   include "Factory/FactoryStore.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(push, 0) 
#	endif 

#	include "magic.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(pop) 
#	endif

#	include <map>

namespace Menge
{
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
        bool initialize( const ConstString & _name, const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator ) override;
		void finalize() override;

    public:
        bool isValid() const override;

	public:
		ParticleEmitterInterfacePtr createEmitter() override;

	protected:
		void onEmitterRelease_( AstralaxEmitter2 * _emitter );

	protected:
		bool loadContainer_( unsigned char * _buffer, size_t _size, HM_FILE & _mf ) const;
		HM_EMITTER createEmitterId_() const;
			
	private:
        ServiceProviderInterface * m_serviceProvider;
        ConstString m_name;

		HM_FILE m_mf;
		uint32_t m_bufferId;

		typedef FactoryPoolStore<AstralaxEmitter2, 16> TFactoryPoolAstralaxEmitter;
		mutable TFactoryPoolAstralaxEmitter m_factoryPoolAstralaxEmitter;
	};

	typedef stdex::intrusive_ptr<AstralaxEmitterContainer2> AstralaxEmitterContainer2Ptr;
}