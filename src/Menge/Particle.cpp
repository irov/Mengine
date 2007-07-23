#	include "Particle.h"

namespace Menge
{
	Particle::Particle()
		: m_position(0,0)
		, m_direction(0,0)
		, m_rotation(0)
		, m_size()
		, m_color(0,0,0,0)
		, m_timeToLive()
		, m_totalTime()
	{}
}