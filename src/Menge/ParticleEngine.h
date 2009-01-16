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
		EmitterContainerInterface * createEmitterContainerFromFile( const String & _filename );
		void releaseEmitterContainer( EmitterContainerInterface* _containerInterface );
		EmitterInterface * createEmitterFromContainer( const String & _name, const EmitterContainerInterface * _container );
		void releaseEmitter( EmitterInterface * _emitter );
		void lockEmitter( EmitterInterface * _emitter, int _typeParticle );
		String getTextureName() const;
		RenderParticle * nextParticle();
		void unlockEmitter( EmitterInterface * _emitter );

	protected:
		ParticleSystemInterface * m_interface;
	};
};
