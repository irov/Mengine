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
		bool nextParticle( RenderParticle & _particle );
		void unlockEmitter( EmitterInterface * _emitter );
		std::size_t getParticlesCount( EmitterInterface* _emitter, int _typeParticle, Viewport* _viewport, const mt::mat3f* _transform );
		void beginFrame();
		size_t getFrameParticlesCount() const;
		size_t getMaxParticlesCount() const;
		void setMaxParticlesCount( size_t _count );

	protected:
		ParticleSystemInterface * m_interface;
		size_t m_maxParticlesNum;
		size_t m_frameParticlesNum;
	};
};
