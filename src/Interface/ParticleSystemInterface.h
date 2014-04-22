#	pragma once

#   include "Interface/ServiceInterface.h"

#	include "Config/Typedef.h"

#   include "Core/ConstString.h"
#   include "Core/FilePath.h"

#   include "Factory/Factorable.h"
#   include "Factory/FactorablePtr.h"

#	include "Math/vec2.h"
#	include "Math/box2.h"

#   include "Math/mat4.h"

#	include <vector>

namespace Menge
{
	struct ParticleVertices
	{
		mt::vec3f v[4];
		mt::vec2f uv[4];

		unsigned long color;
	};

	typedef std::vector<ParticleVertices> TVectorParticleVerices;

	struct ParticleMesh
	{
		size_t begin;
		size_t size;

		int texture;
		bool intense;
	};

	class ParticleEmitterListenerInterface
	{
	public:
		virtual void onParticleEmitterStopped() = 0;
	};

	class ParticleEmitterInterface
        : public Factorable
	{
	public:
		virtual const ConstString & getName() const = 0;

	public:
		virtual void play() = 0;
		virtual void stop() = 0;
		virtual void pause() = 0;
		virtual void restart() = 0;
		virtual void seek( float _timming ) = 0;
	public:
		virtual bool update( float _timing ) = 0;

	public:
		virtual void setLoop( bool _loop ) = 0;
		virtual bool getLoop() const = 0;

	public:
		virtual void interrupt() = 0;

	public:
		virtual bool isBackground() const = 0;
		virtual void getBoundingBox( mt::box2f& _box ) const = 0;		
		virtual void setLeftBorder( float _leftBorder ) = 0;
		
		virtual float getLeftBorder() const = 0;
		virtual float getRightBorder() const = 0;
		virtual float getDuration() const = 0;
		
		virtual bool isIntensive() const = 0;
		virtual void setEmitterTranslateWithParticle( bool _value ) = 0;

	public:
		virtual bool changeEmitterImage( int _width, int _height, unsigned char* _data, int _bytes ) = 0;
		virtual bool changeEmitterModel( float * _points, int _count ) = 0;

	public:
		virtual void setListener( ParticleEmitterListenerInterface* _listener ) = 0;		

    public:
		virtual void setPosition( const mt::vec3f & _pos ) = 0;
		virtual void getPosition( mt::vec3f & _pos ) = 0;

        virtual void getBasePosition( mt::vec3f & _pos ) = 0;

		virtual void setScale( float _scale ) = 0;

		virtual void setAngle( float _radians ) = 0;

		virtual void setRandomMode( bool _randomMode ) = 0;
		virtual bool getRandomMode() const = 0;
       
    public:
        virtual bool getBackgroundBox( mt::box2f & _box ) = 0;
	};

    struct ParticleEmitterAtlas
    {
        FilePath file;
        //WString path;
    };

    typedef std::vector<ParticleEmitterAtlas> TVectorParticleEmitterAtlas;
    
    class ParticleEmitterContainerVisitor
    {
    public:
        virtual void visitEmitterName( const ConstString & _name ) = 0;
        virtual void visitAtlas( const ParticleEmitterAtlas & _atlas ) = 0;
    };
	
	class ParticleEmitterContainerInterface
        : public FactorablePtr
	{
    public:
        virtual bool initialize( const ConstString & _name, ServiceProviderInterface * _serviceProvider ) = 0;
		
    public:
        virtual bool isValid() const = 0;

	public:
		virtual const TVectorParticleEmitterAtlas & getAtlas() const = 0;
		virtual void visitContainer( ParticleEmitterContainerVisitor * visitor ) = 0;

    public:
		virtual ParticleEmitterInterface * createEmitter( const ConstString & _name ) = 0;
		virtual void releaseEmitter( ParticleEmitterInterface * _emitter ) = 0;
	};

    typedef stdex::intrusive_ptr<ParticleEmitterContainerInterface> ParticleEmitterContainerInterfacePtr;

	struct ParticleEmitterRenderFlush
	{
		size_t meshCount;
		size_t particleCount;
	};

	class ParticleSystemInterface
        : public ServiceInterface
	{
        SERVICE_DECLARE("ParticleSystem")

	public:
		virtual ParticleEmitterContainerInterfacePtr createEmitterContainerFromMemory( const ConstString & _name, const void * _buffer ) = 0;
		
	public:
        virtual bool flushParticles( const mt::mat4f & _viewMatrix, ParticleEmitterInterface * _emitter, ParticleMesh * _meshes, size_t _meshLimit, ParticleVertices * _particles, size_t _particlesLimit, ParticleEmitterRenderFlush & _flush ) = 0;
	};

#   define PARTICLE_SYSTEM( serviceProvider )\
    (Menge::Helper::getService<Menge::ParticleSystemInterface>(serviceProvider))

    class ParticleServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("ParticleService")

    public:
        virtual bool flushEmitter( const mt::mat4f & _viewMatrix, ParticleEmitterInterface * _emitter, ParticleMesh * _meshes, size_t _meshLimit, ParticleVertices * _particles, size_t _particlesLimit, ParticleEmitterRenderFlush & _flush ) = 0;
        virtual size_t renderParticlesCount( size_t _count ) = 0;

        virtual ParticleEmitterContainerInterfacePtr createEmitterContainerFromFile( const ConstString& _fileGroupName, const FilePath & _fileName ) = 0;

    public:
        virtual size_t getMaxParticlesCount() const = 0;

    public:
        virtual void update() = 0;
    };

#   define PARTICLE_SERVICE( serviceProvider )\
    (Menge::Helper::getService<Menge::ParticleServiceInterface>(serviceProvider))
}
