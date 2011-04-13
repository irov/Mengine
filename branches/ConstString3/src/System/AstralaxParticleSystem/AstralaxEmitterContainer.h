#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#	include "magic.h"

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
		void addEmitterIds( const String & _name, TVectorEmitters _emitters );
		HM_EMITTER getEmitterId( const String & _name );
		void releaseEmitterId( const String & _name, HM_EMITTER _id );

	public:
		void addAtlas( const EmitterAtlas & _atlas );

	protected:
		const TVectorAtlas & getAtlas() const override;
		EmitterInterface * createEmitter( const String & _name ) override;
		void releaseEmitter( EmitterInterface * _emitter ) override;

	private:
		struct EmitterPool
		{
			TVectorEmitters emitters;

			bool dublicate;
		};

		typedef std::map<String, EmitterPool> TMapEmitters;
		TMapEmitters m_emittersIds;

		TVectorAtlas m_atlas;
	};
}