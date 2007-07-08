#	include "SimplePhysicAffector.h"
#	include "stdlib.h"
//////////////////////////////////////////////////////////////////////////
static float rand2(float min, float max)
{
	float r = (float)rand() / RAND_MAX;
	return min + r * (max-min);
}
SimplePhysicAffector::SimplePhysicAffector()
	: m_gravity(355.f)
	, m_radial(-10.f,-14.f)
	, m_tangetial(14.f,14.f)
	, m_size(1.94f,0.22f,0.54f)
	, m_spin(-50.f,11.f,0.126f)
	, m_startColor(0.68f,0.319f,0.27f,0.3f)
	, m_endColor(0.97f,0.73f,0.389f,0.6f)
	, m_varColor(0.38f,0.44f)
{}

void	SimplePhysicAffector::affectParticles(float _dt, std::list<Particle*>& _particles)
{
	for(std::list<Particle*>::iterator it = _particles.begin(); it != _particles.end(); ++it)
	{
		Particle * particle = static_cast<Particle*>(*it);

		particle->m_size = (m_size.x, m_size.x + (m_size.y - m_size.x) * m_size.z);

		particle->m_rotation = rand2(m_spin.x, m_spin.x+(m_spin.y-m_spin.x)*m_spin.z);

		particle->m_color.r = rand2(m_startColor.r, m_startColor.r+(m_endColor.r-m_startColor.r)*m_varColor.x);
		particle->m_color.g = rand2(m_startColor.g, m_startColor.g+(m_endColor.g-m_startColor.g)*m_varColor.x);
		particle->m_color.b = rand2(m_startColor.b, m_startColor.b+(m_endColor.b-m_startColor.b)*m_varColor.x);
		particle->m_color.a = rand2(m_startColor.a, m_startColor.a+(m_endColor.a-m_startColor.a)*m_varColor.y);

		mt::vec2f a = mt::norm_safe_v2(particle->m_position);

		mt::vec2f a2 = mt::perp(a) * rand2(m_tangetial.x, m_tangetial.y);

		a *= rand2(m_radial.x, m_radial.y);

		particle->m_direction += (a+a2) * _dt;
		particle->m_direction.y += m_gravity * _dt;

		particle->m_position += particle->m_direction * _dt;

		particle->m_rotation += (m_spin.y-particle->m_rotation) / particle->m_totalTime * _dt;
		particle->m_size += (m_size.y-particle->m_size) / particle->m_totalTime * _dt;
		particle->m_color += (m_endColor-particle->m_color) / particle->m_totalTime * _dt;
	}
}