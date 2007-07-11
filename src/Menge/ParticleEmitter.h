#	pragma once

#	include "Particle.h"

#	include "NodeCore.h"

namespace Menge
{
	class	ParticleEmitter
		: public NodeCore
	{
	public:
		ParticleEmitter(int _emission);
		virtual ~ParticleEmitter(){};
		virtual void	emitt(float _dt, Particle * _particle) = 0;
		virtual int		getEmissionCount(float _dt);
	protected:
		int	m_emission;				//num particles per second
		int	m_emissionResidue;		//ostatok
	};
};