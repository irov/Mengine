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
	virtual void play( bool _leftVisible ) = 0;
	virtual void stop() = 0;
	virtual void pause() = 0;
	virtual void update( float _timing ) = 0;
	virtual void setLooped( bool _loop ) = 0;
	virtual bool getLooped() const = 0;
	virtual void getBoundingBox( int & left, int & top, int & right, int & bottom ) const = 0;
	virtual int getNumTypes() const = 0;
};

class EmitterContainerInterface
{
};

class ParticleSystemInterface
{
public:
	virtual EmitterContainerInterface * createEmitterContainerFromMemory( void * _buffer ) = 0;
	virtual EmitterInterface * createEmitterFromContainer( const char * _name, const EmitterContainerInterface * _container ) = 0;
	virtual void releaseEmitter( EmitterInterface * _emitter ) = 0;
	virtual void lockEmitter( EmitterInterface * _emitter, int _typeParticle ) = 0;
	virtual const char * getTextureName() const = 0;
	virtual RenderParticle * nextParticle() = 0; 
	virtual void unlockEmitter( EmitterInterface * _emitter ) = 0;
};

bool	initInterfaceSystem( ParticleSystemInterface** );
void	releaseInterfaceSystem( ParticleSystemInterface* );