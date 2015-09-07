#	pragma once

#   include "Interface/ServiceInterface.h"
#	include "Interface/StreamInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ArchiveInterface.h"

#	include "Config/Typedef.h"
#	include "Config/String.h"

#   include "Core/ConstString.h"
#   include "Core/FilePath.h"
#   include "Core/ColourValue.h"
#	include "Core/Magic.h"

#   include "Factory/Factorable.h"
#   include "Factory/FactorablePtr.h"

#	include "Math/vec2.h"
#	include "Math/box2.h"

#   include "Math/mat4.h"
#   include "Math/uv4.h"

#	ifndef MENGINE_PARTICLE_MAX_MESH
#	define MENGINE_PARTICLE_MAX_MESH 1000
#	endif

namespace Menge
{
	struct ParticleMesh
	{
		uint32_t vertexOffset;
		uint32_t vertexCount;
		uint32_t indexOffset;
		uint32_t indexCount;
		

		int texture[8];
		uint32_t textures;

		RenderStage stage;
	};

	struct ParticleEmitterRenderFlush
	{		
		uint32_t meshCount;
		uint32_t vertexCount;
		uint32_t indexCount;

		uint32_t arrays;
		void * context;
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
		virtual void resume() = 0;
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
		virtual bool prepareParticles( ParticleEmitterRenderFlush & _flush ) = 0;
		virtual bool flushParticles( ParticleMesh * _meshes, uint32_t _meshLimit, RenderVertex2D * _vertices, RenderIndices * _indices, ParticleEmitterRenderFlush & _flush ) = 0;

	public:
		virtual bool isBackground() const = 0;
		virtual void getBoundingBox( mt::box2f& _box ) const = 0;		
		virtual void setLeftBorder( float _leftBorder ) = 0;
		
		virtual float getLeftBorder() const = 0;
		virtual float getRightBorder() const = 0;
		virtual float getDuration() const = 0;
		
		virtual void setEmitterTranslateWithParticle( bool _value ) = 0;

	public:
		virtual bool changeEmitterImage( uint32_t _width, uint32_t _height, unsigned char * _data, size_t _bytes ) = 0;
		virtual bool changeEmitterModel( float * _points, uint32_t _count ) = 0;

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
	
	DECLARE_MAGIC_NUMBER( MAGIC_PTZ, 'P', 'T', 'Z', '2', 2 );

	class ParticleEmitterContainerInterface2
		: public FactorablePtr
	{
	public:
		virtual void setServiceProvider( ServiceProviderInterface * _serviceProvider ) = 0;
		virtual ServiceProviderInterface * getServiceProvider() const = 0;

	public:
		virtual bool initialize( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator ) = 0;
		virtual void finalize() = 0;

	public:
		virtual bool isValid() const = 0;

	public:
		virtual ParticleEmitterInterfacePtr createEmitter() = 0;
	};

	typedef stdex::intrusive_ptr<ParticleEmitterContainerInterface2> ParticleEmitterContainerInterface2Ptr;

	class ParticleSystemInterface2
		: public ServiceInterface
	{
		SERVICE_DECLARE( "ParticleSystem2" )

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual ParticleEmitterContainerInterface2Ptr createEmitterContainerFromMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator ) = 0;
	};

#   define PARTICLE_SYSTEM2( serviceProvider )\
	SERVICE_GET(serviceProvider, Menge::ParticleSystemInterface2)

	class ParticleServiceInterface2
		: public ServiceInterface
	{
		SERVICE_DECLARE( "ParticleService2" )

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual bool isAvailable() const = 0;

	public:
		virtual ParticleEmitterContainerInterface2Ptr createEmitterContainerFromFile( const ConstString& _fileGroupName, const FilePath & _fileName ) = 0;

	public:
		virtual uint32_t getMaxParticlesCount() const = 0;
	};

#   define PARTICLE_SERVICE2( serviceProvider )\
	SERVICE_GET(serviceProvider, Menge::ParticleServiceInterface2)
}
