#	pragma once
#	include "math/vec2.h"
#	include "math/vec4.h"

class Particle
{
public:
	mt::vec2f	x;
	mt::vec2f	v;

	float		g;
	float		r;
	float		tan;

	float		spin;
	float		spin_dt;

	float		size;
	float		size_dt;

	mt::vec4f	color;	
	mt::vec4f	color_dt;

	float		age;
	float		terminal_age;

	void		integrate(const mt::vec2f& offset, float dt);
};
