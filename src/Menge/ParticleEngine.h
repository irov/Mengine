#	pragma once

#	include "Holder.h"
#	include	"math/vec3.h"

#	include "Interface/ParticleSystemInterface.h"

#	include	<string>

namespace Menge
{
	class ParticleEngine
	{
	public:
		ParticleEngine( ParticleSystemInterface * _interface );
		~ParticleEngine();

	public:
		EmitterContainerInterface * createEmitterContainerFromFile( const std::string & _filename );
		EmitterInterface * createEmitterFromContainer( const char * _name, const EmitterContainerInterface * _container );
		void releaseEmitter( EmitterInterface * _emitter );
		void lockEmitter( EmitterInterface * _emitter, int _typeParticle );
		const std::string& getTextureName() const;
		RenderParticle * nextParticle();
		void unlockEmitter( EmitterInterface * _emitter );

	protected:
		ParticleSystemInterface * m_interface;
	};
};