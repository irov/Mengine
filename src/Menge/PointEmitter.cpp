#	include "PointEmitter.h"
#	include "stdlib.h"
	//////////////////////////////////////////////////////////////////////////
	float rand2(float min, float max)
	{
		float r = (float)rand() / RAND_MAX;
		return min + r * (max-min);
	}
	//////////////////////////////////////////////////////////////////////////
	PointEmitter::PointEmitter()
		: m_emission(100)
		, m_emission_residue(0.0f)
		, m_life(0.5f,1.26f)
		, m_spread(0.96)
		, m_start_position(300,300)
		, m_dir(0)
		, m_velocity(300,300)
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
//		_num++;
//		_particle++;
	}
	//////////////////////////////////////////////////////////////////////////
	int PointEmitter::getEmissionCount(float _dt)
	{
		float	particles_needed = m_emission * _dt + m_emission_residue;
		int particles_created = (int)particles_needed;
		m_emission_residue = particles_needed - particles_created;
		return	particles_created;
	}
