#	pragma once

#	include "Holder.h"
#	include	"math/vec3.h"
#	include "math/mat3.h"

#	include "Interface/ParticleSystemInterface.h"

#	include	<string>

namespace Menge
{
	class Viewport;

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
		std::size_t getParticlesCount( EmitterInterface* _emitter, int _typeParticle, Viewport* _viewport, const mt::mat3f* _transform );

	protected:
		ParticleSystemInterface * m_interface;
	};
};
