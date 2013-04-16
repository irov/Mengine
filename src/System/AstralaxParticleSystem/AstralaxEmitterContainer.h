#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#pragma warning(push, 0) 
#	include "magic.h"
#pragma warning(pop) 

#	include <map>

namespace Menge
{
	typedef std::vector<HM_EMITTER> TVectorEmitters;

	class AstralaxEmitterContainer
		: public EmitterContainerInterface
	{
	public:
		AstralaxEmitterContainer();
		~AstralaxEmitterContainer();

	public:
		void addEmitterIds( const ConstString & _name, HM_EMITTER _id, MAGIC_POSITION _pos, TVectorEmitters _emitters );
		HM_EMITTER getEmitterId( const ConstString & _name ) const;
		void releaseEmitterId( const ConstString & _name, HM_EMITTER _id );

	public:
		void addAtlas( const EmitterAtlas & _atlas );
		void visitContainer( EmitterContainerVisitor * visitor ) override;
		const TVectorAtlas & getAtlas() const override;
		EmitterInterface * createEmitter( const ConstString & _name ) override;
		void releaseEmitter( EmitterInterface * _emitter );
		
	private:
        struct EmitterPool
        {
            HM_EMITTER id;
            MAGIC_POSITION pos;

            mutable TVectorEmitters emitters;
            bool dublicate;
        };

		typedef std::map<ConstString, EmitterPool> TMapEmitters;		
		TMapEmitters m_emitters;

		TVectorAtlas m_atlas;
	};
}