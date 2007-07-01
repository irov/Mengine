#	pragma once

#	include "Particle.h"

#	include <vector>

class	ParticleAffector
{
public:
	ParticleAffector(){};
	virtual ~ParticleAffector(){};
	virtual void	affectParticles(float _dt, std::vector<Particle*>& _particles) = 0;
};