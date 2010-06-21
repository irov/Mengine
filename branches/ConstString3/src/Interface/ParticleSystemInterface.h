#	pragma once

#	include "Config/Typedef.h"

#	include <vector>

namespace Menge
{
	struct RenderParticleRectangle
	{
		float x1,y1;
		float x2,y2;
		float x3,y3;
		float x4,y4;
	};

	struct RenderParticleTexture
	{
		float u0,v0;
		float u1,v1;
		int frame;
	};

	struct RenderParticleColor
	{
		unsigned long rgba;
	};

	struct RenderParticle
	{
		RenderParticleRectangle rectangle;
		RenderParticleTexture texture;
		RenderParticleColor color;
	};

	typedef std::vector<RenderParticle> TVectorRenderParticle;

	class ParticleEmitterListenerInterface
	{
	public:
		virtual void onStopped() = 0;
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
		virtual void setLooped( int _loop ) = 0;
		virtual int getLooped() const = 0;

	public:
		virtual void getBoundingBox( int & left, int & top, int & right, int & bottom ) const = 0;
		virtual void setLeftBorder( float _leftBorder ) = 0;
		virtual int getNumTypes() const = 0;
		virtual bool isIntensive() const = 0;

	public:
		virtual void setListener( ParticleEmitterListenerInterface* _listener ) = 0;
		virtual void setPosition( float _x, float _y ) = 0;
		virtual void setAngle( float _radians ) = 0;
	};

	class EmitterContainerInterface
	{
	//public:
		//virtual ~EmitterContainerInterface() {};
	};

	class ParticleSystemInterface
	{
	public:
		virtual EmitterContainerInterface * createEmitterContainerFromMemory( void * _buffer ) = 0;
		virtual void releaseEmitterContainer( EmitterContainerInterface* _containerInterface ) = 0;
		virtual EmitterInterface * createEmitterFromContainer( const String & _name, const EmitterContainerInterface * _container ) = 0;
		virtual void releaseEmitter( EmitterInterface * _emitter ) = 0;
		virtual bool lockEmitter( EmitterInterface * _emitter, int _typeParticle ) = 0;
		virtual int getTextureCount() const = 0;
		virtual const char * getTextureName( int _index ) const = 0;
		virtual int flushParticles( TVectorRenderParticle & _particles, int _particlesLimit ) = 0;
		virtual void unlockEmitter( EmitterInterface * _emitter ) = 0;
	};
}

bool	initInterfaceSystem( Menge::ParticleSystemInterface** );
void	releaseInterfaceSystem( Menge::ParticleSystemInterface* );
