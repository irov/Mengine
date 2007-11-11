#	pragma once

struct RenderParticle
{
	float x1,x2,x3,x4;
	float y1,y2,y3,y4;
	float u0,v0,u1,v1;
	unsigned long color;
};

struct Image
{
	unsigned int size;
	void * data;
};

class EmitterInterface
{
public:
	virtual ~EmitterInterface(){}; 
	virtual void update() = 0;
	virtual void setLoop( bool _loop ) = 0;
	virtual bool getLoop() const = 0;
	virtual float * getPosition() const = 0;
	virtual const Image & getImage( int _id ) =0;
	virtual int getImageCount() const = 0;
	virtual int getParticlesTypeCount() const = 0;
};


class ParticleSystemInterface
{
public:
	virtual ~ParticleSystemInterface(){};
	virtual EmitterInterface * createEmitterFromMemory( void * _buffer, int _size ) = 0;
	virtual void lockEmitter( EmitterInterface * _emitter, int _typeParticle ) = 0;
	virtual RenderParticle * nextParticle() = 0; 
	virtual void unlockEmitter( EmitterInterface * _emitter ) = 0;
};

bool	initInterfaceSystem( ParticleSystemInterface** );
void	releaseInterfaceSystem( ParticleSystemInterface* );