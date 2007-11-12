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

	public:
		EmitterContainerInterface * createEmitterContainerFromMemory( void * _buffer );
		EmitterInterface * createEmitterFromContainer( const char * _name, const EmitterContainerInterface * _container );
		void lockEmitter( EmitterInterface * _emitter, int _typeParticle );
		RenderParticle * nextParticle();
		void unlockEmitter( EmitterInterface * _emitter );

	protected:
		ParticleSystemInterface * m_interface;
	};
};