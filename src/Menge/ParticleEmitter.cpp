#include "ParticleEmitter.h"
using namespace	Menge;
//////////////////////////////////////////////////////////////////////////
ParticleEmitter::ParticleEmitter(int _emission)
	: m_emission(_emission)
	, m_emissionResidue(0)
{
};
//////////////////////////////////////////////////////////////////////////
int ParticleEmitter::getEmissionCount(float _dt)
{
	float	particles_needed = m_emission * _dt + m_emissionResidue;
	int particles_created = (int)particles_needed;
	m_emissionResidue = particles_needed - particles_created;
	return	particles_created;
}