#	pragma once

#	include "ResourceImpl.h"

#	include "RenderSystemInterface.h"

#	include <vector>

#	include "math/vec2.h"
#	include "math/vec4.h"

namespace Menge
{
	struct ParticleDesc
	{
		float		emission; 
		float		lifetime;
		mt::vec2f	life;
		float		dir;
		float		spread;
		mt::vec2f	v;
		mt::vec2f	g;
		mt::vec2f	r;
		mt::vec2f	tan;
		mt::vec3f	size;
		mt::vec3f	spin;
		mt::vec4f	startColor; 
		mt::vec4f	endColor;
		mt::vec2f	varColor;
	};

	class ResourceParticle
		: public ResourceImpl
	{
		RESOURCE_DECLARE( ResourceParticle )

	public:
		ResourceParticle( const std::string & _name );

	public:
		void loader( TiXmlElement *xml );

	protected:
		bool _compile();
		void _release();

	public:
		const ParticleDesc&	getDesc(); 
		RenderImageInterface * getImage();
	private:
		std::string			m_filename;
		ParticleDesc		m_desc;
		RenderImageInterface*	m_image; 
	};
}