#	pragma once

#	include "Holder.h"
#	include	"math/vec3.h"
#	include "math/mat3.h"

#	include "Interface/ParticleSystemInterface.h"

#	include	<string>

namespace Menge
{
	class Viewport;

	struct EmitterRectangle
	{
		mt::vec2f v[4];
	};

	class ParticleEngine
		: public Holder<ParticleEngine>
	{
	public:
		ParticleEngine();
		~ParticleEngine();

		bool initialize();

	public:
		EmitterContainerInterface * createEmitterContainerFromFile( const String& _fileSystemName, const String & _filename );
		void releaseEmitterContainer( EmitterContainerInterface* _containerInterface );
		EmitterInterface * createEmitterFromContainer( const String & _name, const EmitterContainerInterface * _container );
		void releaseEmitter( EmitterInterface * _emitter );
		bool flushEmitter( EmitterInterface * _emitter, int _typeParticle, TVectorRenderParticle & _particles, int* _texturesNum );
		int getTextureCount() const;
		String getTextureName( int _index ) const;
		void lockEmitter( EmitterInterface * _emitter, int _typeParticle );
		void unlockEmitter( EmitterInterface * _emitter );
		std::size_t getParticlesCount( const TVectorRenderParticle & _particles, EmitterInterface* _emitter, int _typeParticle, Viewport* _viewport, const mt::mat3f* _transform );
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
