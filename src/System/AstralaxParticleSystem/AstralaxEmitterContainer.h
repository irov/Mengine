#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#	include "magic.h"

#	include <map>

class AstralaxEmitterContainer 
	: public Menge::EmitterContainerInterface
{
public:
	AstralaxEmitterContainer( HM_FILE _hFile );
	~AstralaxEmitterContainer();

public:
	void addEmitterId( const Menge::String & _name,  HM_EMITTER _id );
	HM_EMITTER getEmitter( const Menge::String & _name )  const;
	
private:
	typedef std::map<Menge::String, HM_EMITTER> TMapEmitters;
	TMapEmitters m_emittersIds;
	HM_FILE m_hfile;
};