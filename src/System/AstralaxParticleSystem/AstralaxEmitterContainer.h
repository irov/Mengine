#	pragma once

#	include "Interface\ParticleSystemInterface.h"

#	include "magic.h"

#	include <map>

#	include <string>

class AstralaxEmitterContainer 
	: public Menge::EmitterContainerInterface
{
public:
	AstralaxEmitterContainer();
	~AstralaxEmitterContainer();

public:
	void addEmitterId( const Menge::String & _name,  HM_EMITTER _id );
	HM_EMITTER getEmitter( const Menge::String & _name )  const;
	
private:
	std::map<std::string, HM_EMITTER> m_emittersIds;
};