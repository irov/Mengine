#	pragma once

#	include "Particle.h"

#	include <list>

class	ParticleAffector
{
public:
	ParticleAffector(){};
	virtual ~ParticleAffector(){};
	virtual void	affectParticles(float _dt, std::list<Particle*>& _particles) = 0;
};