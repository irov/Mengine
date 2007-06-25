#	pragma once

#	include "Particle.h"

class	ParticleEmitter
{
public:
	ParticleEmitter(){};
	virtual ~ParticleEmitter(){};
	virtual void emitt(float _dt, Particle * _particle) = 0;
	virtual int getEmissionCount(float _dt) = 0;
};
