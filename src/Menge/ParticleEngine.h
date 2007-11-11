#	pragma once

#	include "Holder.h"
#	include	"math/vec3.h"

#	include "Interface/ParticleSystemInterface.h"

#	include	<set>
#	include	<string>

namespace Menge
{
	class ParticleEngine
	{
	public:
		ParticleEngine( ParticleSystemInterface * _interface );
		~ParticleEngine();
	protected:
		ParticleSystemInterface * m_interface;
	};
};