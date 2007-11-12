#	pragma once

#	include "Interface\ParticleSystemInterface.h"

#	include "magic.h"

#	include <map>

#	include <string>

class EmitterContainer : public EmitterContainerInterface
{
public:
	EmitterContainer();
	~EmitterContainer();

public:
	void addEmitterId( const char * _name,  HM_EMITTER _id );
	HM_EMITTER getEmitter( const char * _name )  const;
	
private:
	std::map<std::string, HM_EMITTER> m_emittersIds;
};