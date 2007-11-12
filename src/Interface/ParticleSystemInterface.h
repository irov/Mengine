#	pragma once

struct RenderParticle
{
	float x1,x2,x3,x4;
	float y1,y2,y3,y4;
	float u0,v0,u1,v1;
	unsigned long color;
};

class EmitterInterface
{
public:
	virtual ~EmitterInterface(){};
	virtual void play( bool _leftVisible ) = 0;
	virtual void stop() = 0;
	virtual void update() = 0;
	virtual bool isPlaying() const = 0;
	virtual int getParticlesTypeCount() const = 0;
};

class EmitterContainerInterface
{
public:
	virtual ~EmitterContainerInterface(){};
};

class ParticleSystemInterface
{
public:
	virtual ~ParticleSystemInterface(){};
	virtual EmitterContainerInterface * createEmitterContainerFromMemory( void * _buffer ) = 0;
	virtual EmitterInterface * createEmitterFromContainer( const char * _name, const EmitterContainerInterface * _container ) = 0;
	virtual void lockEmitter( EmitterInterface * _emitter, int _typeParticle ) = 0;
	virtual RenderParticle * nextParticle() = 0; 
	virtual void unlockEmitter( EmitterInterface * _emitter ) = 0;
};

bool	initInterfaceSystem( ParticleSystemInterface** );
void	releaseInterfaceSystem( ParticleSystemInterface* );