#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#	include "magic.h"

#	include <map>

namespace Menge
{
	typedef std::vector<HM_EMITTER> TVectorEmittersId;

	class AstralaxEmitterContainer
		: public EmitterContainerInterface
	{
	public:
		AstralaxEmitterContainer();
		~AstralaxEmitterContainer();

	public:
		void addEmitterIds( const String & _name, TVectorEmittersId _emitters );
		HM_EMITTER getEmitterId( const String & _name );
		void releaseEmitterId( const String & _name, HM_EMITTER _id );

	public:
		void addAtlas( const EmitterAtlas & _atlas );
		const EmitterContainerMetaData& getMetaData() const override;
		void visitContainer( EmitterContainerVisitor * visitor ) override;
		const TVectorAtlas & getAtlas() const override;
		EmitterInterface * createEmitter( const String & _name ) override;
		bool isMetaData( const char * _data );
		void setMetaData( const char * _data );
		void releaseEmitter( EmitterInterface * _emitter );
		
	private:
		struct EmitterPool
		{
			TVectorEmittersId emitters;
			bool dublicate;
		};

		typedef std::vector<EmitterInterface *> TVectorEmitters;
		EmitterContainerMetaData m_metaData;
		TVectorEmitters m_emitters;
		typedef std::map<String, EmitterPool> TMapEmitters;
		
		TMapEmitters m_emittersIds;
		TVectorAtlas m_atlas;
	};
}