#	pragma once

#	include "ResourceImpl.h"

#	include "Interface/RenderSystemInterface.h"

#	include <vector>

#	include "math/vec2.h"
#	include "math/vec4.h"

namespace Menge
{
	struct BoxEmitterDesc
	{
		mt::vec2f	position;
		mt::vec2f	life;
		mt::vec2f	velocity;
		float		dir;	
		float		spread;
	};

	class ResourceBoxEmitter
		: public ResourceImpl
	{
		RESOURCE_DECLARE( ResourceBoxEmitter )

	public:
		ResourceBoxEmitter( const std::string & _name );

	public:
		void loader( TiXmlElement *xml );

	protected:
		bool _compile();
		void _release();

	public:
		BoxEmitterDesc& getDesc();

	private:
		BoxEmitterDesc m_desc;
	};
}