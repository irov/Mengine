#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#   include "Factory/FactoryStore.h"

#   include "Core/IntrusiveSplayTree.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(push, 0) 
#	endif 

#	include "magic.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(pop) 
#	endif

#	ifndef MENGINE_ASTRALAX_EMITTER_NAME_MAX
#	define MENGINE_ASTRALAX_EMITTER_NAME_MAX 32
#	endif

#   include <stdex/stl_map.h>
#   include <stdex/stl_vector.h>

#	include <string.h>

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
		bool addEmitterIds( const char * _name, HM_EMITTER _id, MAGIC_POSITION _pos, const TVectorEmitters & _emitters );
		HM_EMITTER getEmitterId( const char * _name ) const;
		void releaseEmitterId( const char * _name, HM_EMITTER _id );

	public:
		void addAtlas( const ParticleEmitterAtlas & _atlas );

    public:
		void visitContainer( ParticleEmitterContainerVisitor * _visitor ) override;
		const TVectorParticleEmitterAtlas & getAtlas() const override;

    public:
		ParticleEmitterInterfacePtr createEmitter( const ConstString & _name ) override;

	public:
		void onEmitterDestroy_( AstralaxEmitter * _emitter );

	protected:
        ServiceProviderInterface * m_serviceProvider;

        struct EmitterDesc
        {
			char name[MENGINE_ASTRALAX_EMITTER_NAME_MAX];

            HM_EMITTER id;
            mutable MAGIC_POSITION pos;

            mutable TVectorEmitters emitters;
            bool dublicate;
        };
				
		typedef stdex::vector<EmitterDesc> TVectorEmitterDescs;
		TVectorEmitterDescs m_emitters;

        typedef FactoryPoolStore<AstralaxEmitter, 16> TFactoryPoolAstralaxEmitter;
        mutable TFactoryPoolAstralaxEmitter m_factoryPoolAstralaxEmitter;

		TVectorParticleEmitterAtlas m_atlas;

	protected:
		const EmitterDesc * findEmitterDesc_( const char * _name ) const;

	};

	typedef stdex::intrusive_ptr<AstralaxEmitterContainer> AstralaxEmitterContainerPtr;
}