#	pragma once

#	include "SceneNode2D.h"

#	include "Particle.h"

#	include "math.h"

#	include "windows.h"
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

		ParticleDesc		info;
		RenderImageInterface * m_image;
		int					alive_num;
		std::string			m_resourceName;
//
		//std::vector<Particle*>	m_particlePool;
		//std::list<Particle*>	m_freeParticleList;
		//std::list<Particle*>	m_activeParticleList;

		std::vector<Particle*>	m_particles;

		Particle*	createParticle()
		{
			//m_particles.push_back()
			//Particle* p = m_freeParticleList.front();
			//m_activeParticleList.splice(m_activeParticleList.end(), m_freeParticleList, m_freeParticleList.begin());
			//return p;
			return NULL;
		}
	};
};