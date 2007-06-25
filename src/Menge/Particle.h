#	pragma once
#	include "math/vec2.h"
#	include "math/vec4.h"

class Particle
{
public:
	Particle()
		: m_position(0,0)
		, m_direction(0,0)
		, m_rotation(0)
		, m_size()
		, m_color(0,0,0,0)
		, m_timeToLive()
		, m_totalTime()
	{};

	mt::vec2f	m_position;	
	mt::vec2f	m_direction;	
	float		m_rotation;	
	float		m_size;	
	mt::vec4f	m_color;	
	float		m_timeToLive;	
	float		m_totalTime;	
};