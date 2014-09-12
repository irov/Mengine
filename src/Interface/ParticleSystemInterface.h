#	pragma once

#   include "Interface/ServiceInterface.h"
#	include "Interface/StreamInterface.h"

#	include "Config/Typedef.h"

#   include "Core/ConstString.h"
#   include "Core/FilePath.h"

#   include "Factory/Factorable.h"
#   include "Factory/FactorablePtr.h"

#	include "Math/vec2.h"
#	include "Math/box2.h"

#   include "Math/mat4.h"

#	include <stdex/stl_vector.h>

namespace Menge
{
	struct ParticleVertices
	{
		mt::vec3f v[4];
		mt::vec2f uv[4];

		uint32_t color;
	};

	typedef stdex::vector<ParticleVertices> TVectorParticleVerices;

	struct ParticleMesh
	{
		size_t begin;
		size_t size;

		int texture;
		bool intense;
	};

	struct ParticleEmitterRenderFlush
	{
		size_t meshCount;
		size_t particleCount;
	};

	struct ParticleCamera
	{
		mt::vec3f pos;
		mt::vec3f dir;
		mt::vec3f up;

		float fov;
		float aspect;
		float znear;
		float zfar;

		float width;
		float height;
	};

	class ParticleEmitterInterface
        : public FactorablePtr
	{
	public:
		virtual void play() = 0;
		virtual void stop() = 0;
		virtual void pause() = 0;
		virtual void restart() = 0;
		virtual void seek( float _timming ) = 0;
	public:
		virtual bool update( float _timing, bool & _stop ) = 0;

	public:
		virtual void setLoop( bool _loop ) = 0;
		virtual bool getLoop() const = 0;

	public:
		virtual void interrupt() = 0;

	public:
		virtual bool is3d() const = 0;

		virtual bool getCamera( ParticleCamera & _camera ) const = 0;

	public:
		virtual bool flushParticles( ParticleMesh * _meshes, size_t _meshLimit, ParticleVertices * _particles, size_t _particlesLimit, ParticleEmitterRenderFlush & _flush ) = 0;

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

	typedef stdex::intrusive_ptr<ParticleEmitterInterface> ParticleEmitterInterfacePtr;

    struct ParticleEmitterAtlas
    {
        FilePath file;
        //WString path;
    };

    typedef stdex::vector<ParticleEmitterAtlas> TVectorParticleEmitterAtlas;
    
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
		virtual void setServiceProvider( ServiceProviderInterface * _serviceProvider ) = 0;
		virtual ServiceProviderInterface * getServiceProvider() const = 0;

    public:
        virtual bool initialize() = 0;
		
    public:
        virtual bool isValid() const = 0;

	public:
		virtual const TVectorParticleEmitterAtlas & getAtlas() const = 0;
		virtual void visitContainer( ParticleEmitterContainerVisitor * visitor ) = 0;

    public:
		virtual ParticleEmitterInterfacePtr createEmitter( const ConstString & _name ) = 0;
	};

    typedef stdex::intrusive_ptr<ParticleEmitterContainerInterface> ParticleEmitterContainerInterfacePtr;

	class ParticleSystemInterface
        : public ServiceInterface
	{
        SERVICE_DECLARE("ParticleSystem")

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual ParticleEmitterContainerInterfacePtr createEmitterContainerFromMemory( const InputStreamInterfacePtr & _stream ) = 0;
	};

#   define PARTICLE_SYSTEM( serviceProvider )\
    SERVICE_GET(serviceProvider, Menge::ParticleSystemInterface)

    class ParticleServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE("ParticleService")

    public:
        virtual size_t renderParticlesCount( size_t _count ) = 0;

	public:
        virtual ParticleEmitterContainerInterfacePtr createEmitterContainerFromFile( const ConstString& _fileGroupName, const FilePath & _fileName ) = 0;

    public:
        virtual size_t getMaxParticlesCount() const = 0;

    public:
        virtual void update() = 0;
    };

#   define PARTICLE_SERVICE( serviceProvider )\
    SERVICE_GET(serviceProvider, Menge::ParticleServiceInterface)
}
