#	pragma once

#	include "Renderable.h"

#	include "Particle.h"

#	include "math.h"

#	include "windows.h"
#	include "ParticleResource.h"


namespace Menge
{
	class ParticleSystem
		: public Renderable
	{
		OBJECT_DECLARE(ParticleSystem);
	public:
		ParticleSystem();
		void	move(float x, float y);
	public:
		bool	isVisible(const Viewport & _viewPort);
		void	update(float _timing) override;
		void	loader(TiXmlElement *xml) override;
	protected:
		bool _activate() override;
		void _deactivate() override;
		void _render( const mt::mat3f &rwm, const Viewport & _viewPort ) override;
		void _debugRender() override;

	protected:
		ResourceParticle *	m_particleResource;
	private:
		static const int MAX_PARTICLES = 500;
		Particle			particles[MAX_PARTICLES];
		float				fAge;
		float				emission_residue;
		mt::vec2f			prev_position;
		mt::vec2f			position;
		ParticleDesc		info;
		RenderImageInterface * m_image;
		int					alive_num;
	private:
		std::string m_resourceName;
	};
};