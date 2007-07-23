#	include "PointEmitter.h"
#	include "stdlib.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	float rand2(float min, float max)
	{
		float r = (float)rand() / RAND_MAX;
		return min + r * (max-min);
	}
	//////////////////////////////////////////////////////////////////////////
	PointEmitter::PointEmitter()
		: ParticleEmitter(1000)
		, m_life(0.5f,1.26f)
		, m_spread(0.96f)
		, m_start_position(300.f,300.f)
		, m_dir(0.f)
		, m_velocity(300.f,300.f)
	{};
	//////////////////////////////////////////////////////////////////////////
	void PointEmitter::emitt(float _dt, Particle * _particle)
	{
		_particle->m_timeToLive = 0.0f;
		_particle->m_totalTime = rand2(m_life.x, m_life.y);

		_particle->m_position = m_start_position;

		float ang = m_dir-1.57f+rand2(0,m_spread)-m_spread/2.0f;

		_particle->m_direction.x = cosf(ang);
		_particle->m_direction.y = sinf(ang);
		_particle->m_direction *= rand2(m_velocity.x, m_velocity.y);
	}
}