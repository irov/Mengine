#	include "ParticleEmitter.h"

class	PointEmitter
		: public ParticleEmitter
{
public:
	PointEmitter();
	void emitt(float _dt, Particle * _particle);
	int getEmissionCount(float _dt);
private:
	mt::vec2f	m_start_position;
	mt::vec2f	m_life;
	mt::vec2f	m_velocity;
	float		m_emission;
	float		m_emission_residue;
	float		m_dir;	
	float		m_spread;	
};