#	include "Particle.h"

void	Particle::integrate(const mt::vec2f& offset, float dt)
{
	mt::vec2f a = mt::norm_safe_v2(x - offset);

	mt::vec2f a2 = mt::perp(a) * tan;

	a *= r;
	v += (a+a2) * dt;
	v.y += g * dt;

	x += v * dt;

	spin += spin_dt * dt;
	size += size_dt * dt;
	color += color_dt * dt;
}