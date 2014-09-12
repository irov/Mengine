#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#   include "Factory/FactoryStore.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(push, 0) 
#	endif 

#	include "magic.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(pop) 
#	endif

#   include <stdex/stl_map.h>
#   include <stdex/stl_vector.h>

namespace Menge
{
	class AstralaxEmitter;

	typedef stdex::vector<HM_EMITTER> TVectorEmitters;

	class AstralaxEmitterContainer
		: public ParticleEmitterContainerInterface
	{
	public:
		AstralaxEmitterContainer();
		~AstralaxEmitterContainer();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize() override;

    public:
        bool isValid() const override;

	public:
		bool addEmitterIds( const ConstString & _name, HM_EMITTER _id, MAGIC_POSITION _pos, const TVectorEmitters & _emitters );
		HM_EMITTER getEmitterId( const ConstString & _name ) const;
		void releaseEmitterId( const ConstString & _name, HM_EMITTER _id );

	public:
		void addAtlas( const ParticleEmitterAtlas & _atlas );

    public:
		void visitContainer( ParticleEmitterContainerVisitor * visitor ) override;
		const TVectorParticleEmitterAtlas & getAtlas() const override;

    public:
		ParticleEmitterInterfacePtr createEmitter( const ConstString & _name ) override;

	public:
		void onEmitterDestroy_( AstralaxEmitter * _emitter );
		
	private:
        ServiceProviderInterface * m_serviceProvider;

        struct EmitterPool
        {
            HM_EMITTER id;
            MAGIC_POSITION pos;

            mutable TVectorEmitters emitters;
            bool dublicate;
        };

        typedef FactoryPoolStore<AstralaxEmitter, 16> TFactoryPoolAstralaxEmitter;
        mutable TFactoryPoolAstralaxEmitter m_factoryPoolAstralaxEmitter;

		typedef stdex::map<ConstString, EmitterPool> TMapEmitters;		
		TMapEmitters m_emitters;

		TVectorParticleEmitterAtlas m_atlas;
	};

	typedef stdex::intrusive_ptr<AstralaxEmitterContainer> AstralaxEmitterContainerPtr;
}