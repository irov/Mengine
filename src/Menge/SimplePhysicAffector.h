#	pragma once
#	include "ParticleAffector.h"

using namespace Menge;

class	SimplePhysicAffector
	: public ParticleAffector
{
public:
	SimplePhysicAffector();
	void	affectParticles(float _dt, std::list<Particle*>& _particles);
private:
	float		m_gravity;		
	mt::vec2f	m_radial;		
	mt::vec2f	m_tangetial;	
	mt::vec3f	m_size;	
	mt::vec3f	m_spin;	
	mt::vec4f	m_startColor;
	mt::vec4f	m_endColor;
	mt::vec2f	m_varColor;
};