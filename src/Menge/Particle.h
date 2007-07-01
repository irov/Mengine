#	pragma once
#	include "math/vec2.h"
#	include "math/vec4.h"

namespace Menge
{
	class Particle
	{
	public:
		Particle();
		mt::vec2f	m_position;	
		mt::vec2f	m_direction;	
		float		m_rotation;	
		float		m_size;	
		mt::vec4f	m_color;	
		float		m_timeToLive;	
		float		m_totalTime;	
	};
};