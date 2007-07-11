#	pragma once

#	include "SceneNode2D.h"

#	include "Particle.h"

#	include "math.h"

#	include "ParticleResource.h"

#	include "ParticleEmitter.h"
#	include "ParticleAffector.h"
#	include <list>

namespace Menge
{
	class ParticleSystem
		: public SceneNode2D
	{
		OBJECT_DECLARE(ParticleSystem);
	public:
		ParticleSystem();
	
		void	addAffector(ParticleAffector*);
		void	addEmitter(ParticleEmitter*);

	public:
		bool	isVisible(const Viewport & _viewPort);
		void	update(float _timing) override;
		void	loader(TiXmlElement *xml) override;

	protected:
		bool	_activate() override;
		void	_deactivate() override;
		void	_render( const mt::mat3f &rwm, const Viewport & _viewPort ) override;
		void	_debugRender() override;

	protected:
		ResourceParticle *	m_particleResource;

	private:
		std::list<ParticleAffector*>	m_affectors;
		std::list<ParticleEmitter*>		m_emitters;

		mt::vec2f	m_maxPoint;
		mt::vec2f	m_minPoint;

		RenderImageInterface * m_image;
		std::string			m_resourceName;

		typedef std::vector<Particle> TVectorPoolParticle;
		typedef std::list<Particle*> TListFreeParticle;
		typedef std::list<Particle*> TListActiveParticle;

		TVectorPoolParticle	m_particlePool;
		TListFreeParticle	m_freeParticleList;
		TListActiveParticle	m_activeParticleList;

		Particle*	createParticle();
		void	resizePool(size_t N);
	};
};