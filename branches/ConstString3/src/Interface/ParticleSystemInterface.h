#	pragma once

#	include "Config/Typedef.h"

#	include "Utils/Math/vec2.h"

#	include <vector>

namespace Menge
{
	struct ParticleVertices
	{
		mt::vec2f v[4];
		mt::vec2f uv[4];

		unsigned long color;
	};

	typedef std::vector<ParticleVertices> TVectorParticleVerices;

	struct ParticleMesh
	{
		TVectorParticleVerices::size_type begin;
		TVectorParticleVerices::size_type size;

		int texture;
		bool intense;
	};

	typedef std::vector<ParticleMesh> TVectorParticleMeshes;

	class ParticleEmitterListenerInterface
	{
	public:
		virtual void onParticleEmitterStopped() = 0;
	};

	class EmitterInterface
	{
	public:
		virtual const std::string & getName() const = 0;

	public:
		virtual void play() = 0;
		virtual void stop() = 0;
		virtual void pause() = 0;
		virtual void restart() = 0;

	public:
		virtual void update( float _timing ) = 0;

	public:
		virtual void setLoop( int _loop ) = 0;
		virtual int getLoop() const = 0;

	public:
		virtual void getBoundingBox( int & left, int & top, int & right, int & bottom ) const = 0;
		virtual void setLeftBorder( float _leftBorder ) = 0;

		virtual bool isIntensive() const = 0;

	public:
		virtual void changeEmitterImage( int _width, int _height, unsigned char* _data, int _bytes ) = 0;
		//virtual void changeEmitterPolygon( int _width, int _height, unsigned char* _data, int _bytes ) = 0;

	public:
		virtual void setListener( ParticleEmitterListenerInterface* _listener ) = 0;
		virtual void getPosition( mt::vec2f & _pos ) = 0;
		virtual void setPosition( const mt::vec2f & _pos ) = 0;
		virtual void setAngle( float _radians ) = 0;
	};

	class EmitterContainerInterface
	{
	public:
		struct EmitterAtlas
		{
			String file;
			String path;
		};

		typedef std::vector<EmitterAtlas> TVectorAtlas;

	public:
		virtual const TVectorAtlas & getAtlas() const = 0;
	};

	class ParticleSystemInterface
	{
	public:
		virtual EmitterContainerInterface * createEmitterContainerFromMemory( const void * _buffer ) = 0;
		virtual void releaseEmitterContainer( EmitterContainerInterface* _containerInterface ) = 0;
		
		virtual EmitterInterface * createEmitterFromContainer( const String & _name, const EmitterContainerInterface * _container ) = 0;
		virtual void releaseEmitter( EmitterInterface * _emitter ) = 0;
		virtual void getEmitterPosition( EmitterInterface * _emitter, mt::vec2f & _pos ) = 0; 

		virtual bool flushParticles( EmitterInterface * _emitter, TVectorParticleMeshes & _meshes, TVectorParticleVerices & _particles, int _particlesLimit ) = 0;
	};
}

bool	initInterfaceSystem( Menge::ParticleSystemInterface** );
void	releaseInterfaceSystem( Menge::ParticleSystemInterface* );
