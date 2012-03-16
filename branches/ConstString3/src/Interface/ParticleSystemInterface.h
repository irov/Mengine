#	pragma once

#	include "Config/Typedef.h"

#	include "Utils/Math/vec2.h"
#	include "Utils/Math/box2.h"
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
		virtual void seek( float _timming ) = 0;
	public:
		virtual void update( float _timing ) = 0;

	public:
		virtual void setLoop( bool _loop ) = 0;
		virtual bool getLoop() const = 0;

	public:
		virtual void interrupt() = 0;

	public:
		virtual void getBoundingBox( mt::box2f& _box ) = 0;
		virtual void setLeftBorder( float _leftBorder ) = 0;
		
		virtual float getLeftBorder() = 0;
		virtual float getRightBorder() = 0;
		virtual float getDuration() = 0;
		
		virtual bool isIntensive() const = 0;
		virtual void setEmitterTranslateWithParticle( bool _value ) = 0;
	public:
		virtual bool changeEmitterImage( int _width, int _height, unsigned char* _data, int _bytes ) = 0;
		virtual bool changeEmitterModel( float * _points, int _count ) = 0;
	public:
		virtual void setListener( ParticleEmitterListenerInterface* _listener ) = 0;
		virtual void getPosition( mt::vec2f & _pos ) = 0;
		virtual void setPosition( const mt::vec2f & _pos ) = 0;
		virtual void setAngle( float _radians ) = 0;
	};
	
	struct EmitterContainerMetaData
	{
		mt::vec2f size; 
	};

	class EmitterContainerInterface
	{
	public:
		struct EmitterAtlas
		{
			String file;
			String path;
		};
		
		class EmitterContainerVisitor
		{
		public:
			virtual void visitEmitterName( const String & _name ) = 0;
			virtual void visitAtlas( const EmitterAtlas & _atlas ) = 0;
		};

		typedef std::vector<EmitterAtlas> TVectorAtlas;
			
	public:
		virtual const EmitterContainerMetaData& getMetaData() const = 0;
		virtual const TVectorAtlas & getAtlas() const = 0;
		virtual void visitContainer( EmitterContainerVisitor * visitor ) = 0;
		virtual EmitterInterface * createEmitter( const String & _name ) = 0;
		virtual void releaseEmitter( EmitterInterface * _emitter ) = 0;
	};

	class ParticleSystemInterface
	{
	public:
		virtual EmitterContainerInterface * createEmitterContainerFromMemory( const void * _buffer ) = 0;
		virtual void releaseEmitterContainer( EmitterContainerInterface* _containerInterface ) = 0;
		
	public:
		virtual bool flushParticles( EmitterInterface * _emitter, TVectorParticleMeshes & _meshes, TVectorParticleVerices & _particles, int _particlesLimit ) = 0;
	};
}

bool	initInterfaceSystem( Menge::ParticleSystemInterface** );
void	releaseInterfaceSystem( Menge::ParticleSystemInterface* );
