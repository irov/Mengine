#	include "ParticleEmitter.h"

#	include "SceneNode2D.h"

namespace Menge
{
	class	ResourceBoxEmitter;

	class	BoxEmitter
			: public ParticleEmitter
	{
		OBJECT_DECLARE(BoxEmitter);
	public:
		BoxEmitter();
		void emitt(float _dt, Particle * _particle);
		void loader( TiXmlElement * _xml ) override;
	protected:
		ResourceBoxEmitter * m_resource;
		std::string m_resourceName;

		bool _activate() override;
		void _deactivate() override;
	private:
		mt::vec2f	m_position;
		mt::vec2f	m_life;
		mt::vec2f	m_velocity;
		float		m_dir;	
		float		m_spread;	
	};
}