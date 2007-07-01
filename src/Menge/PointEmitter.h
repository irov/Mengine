#	include "ParticleEmitter.h"

using namespace Menge;

class	PointEmitter
		: public ParticleEmitter
{
public:
	PointEmitter();
	void emitt(float _dt, Particle * _particle);
private:
	mt::vec2f	m_start_position;
	mt::vec2f	m_life;
	mt::vec2f	m_velocity;
	float		m_dir;	
	float		m_spread;	
};