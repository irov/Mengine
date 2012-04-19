#	pragma once
#	include "Utils/Core/Holder.h"
#	include "Utils/Core/String.h"
#	include <vector>

namespace Menge
{
	class EmitterDescription;
	struct ParticlesAtlas;

	typedef std::vector<EmitterDescription *> TVectorEmitterDescription;
	typedef std::vector<ParticlesAtlas *> TVectorParticlesAtlas;
	class EmitterDescriptionManager
		: public Holder<EmitterDescriptionManager>
	{
	public:
		EmitterDescription * createEmitterDescription();
		void destroy();
		ParticlesAtlas * createAtlas( const String & _file/*, const String & _path*/ );
	protected:
		TVectorEmitterDescription m_emitters;
		TVectorParticlesAtlas m_atlasses;
	};

}
 