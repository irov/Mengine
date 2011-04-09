#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#	include "magic.h"

#	include <map>

namespace Menge
{
	class AstralaxEmitterContainer
		: public EmitterContainerInterface
	{
	public:
		AstralaxEmitterContainer();
		~AstralaxEmitterContainer();

	public:
		void addEmitterId( const String & _name,  HM_EMITTER _id );
		HM_EMITTER getEmitter( const String & _name )  const;

	public:
		void addAtlas( const EmitterAtlas & _atlas );

	protected:
		const TVectorAtlas & getAtlas() const override;

	private:
		typedef std::map<String, HM_EMITTER> TMapEmitters;
		TMapEmitters m_emittersIds;

		TVectorAtlas m_atlas;
	};
}