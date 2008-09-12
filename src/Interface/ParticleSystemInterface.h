#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	struct RenderParticle
	{
		float x1,x2,x3,x4;
		float y1,y2,y3,y4;
		float u0,v0,u1,v1;
		unsigned long color;
	};

	class ParticleEmitterListenerInterface
	{
	public:
		virtual void onStopped() = 0;
	};

	class EmitterInterface
	{
	public:
		virtual void play() = 0;
		virtual void stop() = 0;
		virtual void pause() = 0;
		virtual void restart() = 0;
		virtual void update( float _timing ) = 0;
		virtual void setLooped( int _loop ) = 0;
		virtual int getLooped() const = 0;
		virtual void getBoundingBox( int & left, int & top, int & right, int & bottom ) const = 0;
		virtual void setLeftBorder( float _leftBorder ) = 0;
		virtual int getNumTypes() const = 0;
		virtual bool isIntensive() const = 0;
		virtual void setListener( ParticleEmitterListenerInterface* _listener ) = 0;
		virtual void setPosition( float _x, float _y ) = 0;
		virtual void setAngle( float _radians ) = 0;
	};

	class EmitterContainerInterface
	{
	public:
		virtual ~EmitterContainerInterface() {};
	};

	class ParticleSystemInterface
	{
	public:
		virtual EmitterContainerInterface * createEmitterContainerFromMemory( void * _buffer ) = 0;
		virtual void releaseEmitterContainer( EmitterContainerInterface* _containerInterface ) = 0;
		virtual EmitterInterface * createEmitterFromContainer( const String & _name, const EmitterContainerInterface * _container ) = 0;
		virtual void releaseEmitter( EmitterInterface * _emitter ) = 0;
		virtual void lockEmitter( EmitterInterface * _emitter, int _typeParticle ) = 0;
		virtual String getTextureName() const = 0;
		virtual RenderParticle * nextParticle() = 0; 
		virtual void unlockEmitter( EmitterInterface * _emitter ) = 0;
	};
}

bool	initInterfaceSystem( Menge::ParticleSystemInterface** );
void	releaseInterfaceSystem( Menge::ParticleSystemInterface* );