#	pragma once

#	include "Core/Holder.h"
#	include	"math/vec3.h"
#	include "math/mat3.h"

#	include "Interface/ParticleSystemInterface.h"

#	include "Core/ConstString.h"

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

	public:
		bool initialize();

	public:
		EmitterContainerInterface * createEmitterContainerFromFile( const ConstString& _fileSystemName, const ConstString & _filename );
		void releaseEmitterContainer( EmitterContainerInterface* _containerInterface );

		EmitterInterface * createEmitterFromContainer( const ConstString & _name, const EmitterContainerInterface * _container );
		void releaseEmitter( EmitterInterface * _emitter );

		bool flushEmitter( EmitterInterface * _emitter, TVectorParticleMeshes & _meshes, TVectorParticleVerices & _particles, int _particlesLimit );

		void getEmitterPosition( EmitterInterface * _emitter, mt::vec2f & _pos );
				
		size_t getMaxParticlesCount() const;
		void setMaxParticlesCount( size_t _count );

	public:
		void beginParticlesCount();
		size_t renderParticlesCount( size_t _count );

	protected:
		ParticleSystemInterface * m_interface;
		size_t m_maxParticlesNum;
		size_t m_renderParticleNum;
	};
};
