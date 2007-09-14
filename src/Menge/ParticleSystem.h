#	pragma once


#	include "SceneNode2D.h"

#	include "Particle.h"

#	include "ParticleResource.h"

#	include "ParticleEmitter.h"
#	include "ParticleAffector.h"

#	include "math.h"

#	include "math/box2.h"

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
		void	loader(TiXmlElement * _xml) override;

	protected:
		bool	_activate() override;
		void	_deactivate() override;
		void	_render( const mt::mat3f &rwm, const Viewport & _viewPort ) override;
		void	_debugRender() override;

	protected:
		ResourceParticle *	m_particleResource;

	private:

		mt::box2f	m_bbox;

		RenderImageInterface * m_image;
		std::string			m_resourceName;

		typedef std::vector<Particle> TVectorPoolParticle;
		typedef std::list<Particle*> TListFreeParticle;
		typedef std::list<Particle*> TListActiveParticle;
		typedef std::list<ParticleAffector*> TListAffectors;
		typedef std::list<ParticleEmitter*> TListEmitters;

		TVectorPoolParticle	m_particlePool;
		TListFreeParticle	m_freeParticleList;
		TListActiveParticle	m_activeParticleList;
		TListAffectors		m_affectors;
		TListEmitters		m_emitters;

		Particle *	_createParticle();
		size_t		_getEmissionCount( float _timing );

		void		_resizePool(size_t N);
		void		_doLife( float _timing );
		void		_processEmitters( float _timing );
		void		_processAffectors( float _timing );
		void		_calculateBoundingBox();
	};
};