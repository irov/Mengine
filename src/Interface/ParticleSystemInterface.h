#pragma once

#include "Config/Typedef.h"
#include "Config/String.h"

#include "Interface/ServiceInterface.h"
#include "Interface/StreamInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/ArchiveInterface.h"

#include "Core/ConstString.h"
#include "Core/FilePath.h"
#include "Core/ColourValue.h"
#include "Core/Magic.h"
#include "Core/RenderVertex2D.h"
#include "Core/RenderIndex.h"

#include "Factory/Factorable.h"
#include "Factory/Factorable.h"

#include "math/vec2.h"
#include "math/box2.h"

#include "math/mat4.h"
#include "math/uv4.h"

#ifndef MENGINE_PARTICLE_MAX_MESH
#	define MENGINE_PARTICLE_MAX_MESH 1000
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class ResourceImage> ResourceImagePtr;
    //////////////////////////////////////////////////////////////////////////
	struct ParticleMesh
	{
		uint32_t vertexOffset;
		uint32_t vertexCount;
		uint32_t indexOffset;
		uint32_t indexCount;		

		int texture[8];
		uint32_t textures;

		int material;
	};
    //////////////////////////////////////////////////////////////////////////
	struct ParticleEmitterRenderFlush
	{		
		uint32_t meshCount;
		uint32_t vertexCount;
		uint32_t indexCount;

		uint32_t arrays;
		void * context;
	};
    //////////////////////////////////////////////////////////////////////////
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
    //////////////////////////////////////////////////////////////////////////
	class ParticlePositionProviderInterface
	{
	public:
		virtual void onProviderEmitterPosition( mt::vec3f & _position ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
	class ParticleCameraProviderInterface
	{
	public:
		virtual void onProviderEmitterCamera( bool & _orthogonality, mt::vec3f & _position, mt::vec3f & _direction ) = 0;
	};
    //////////////////////////////////////////////////////////////////////////
	class ParticleEmitterInterface
        : public ServantInterface
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
		virtual bool flushParticles( ParticleMesh * _meshes, uint32_t _meshLimit, RenderVertex2D * _vertices, RenderIndex * _indices, ParticleEmitterRenderFlush & _flush ) = 0;
        
	public:
		virtual const mt::box2f & getBoundingBox() const = 0;

	public:						
		virtual float getLeftBorder() const = 0;
		virtual float getRightBorder() const = 0;
		virtual float getDuration() const = 0;
		
		virtual void setEmitterTranslateWithParticle( bool _value ) = 0;

	public:
		virtual bool changeEmitterImage( uint32_t _width, uint32_t _height, unsigned char * _data, size_t _bytes ) = 0;
		virtual bool changeEmitterModel( float * _points, uint32_t _count ) = 0;

    public:
		virtual bool setCameraProvider( ParticleCameraProviderInterface * _cameraProvider ) = 0;
		virtual bool setPositionProvider( ParticlePositionProviderInterface * _positionProvider ) = 0;

		virtual void setScale( float _scale ) = 0;

		virtual void setAngle( float _radians ) = 0;

		virtual void setRandomMode( bool _randomMode ) = 0;
		virtual bool getRandomMode() const = 0;
	};
    //////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<ParticleEmitterInterface> ParticleEmitterInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
	DECLARE_MAGIC_NUMBER( MAGIC_PTZ, 'P', 'T', 'Z', '2', 2 );
    //////////////////////////////////////////////////////////////////////////
	class ParticleEmitterContainerInterface2
		: public ServantInterface
	{
	public:
		virtual bool isValid() const = 0;

	public:
		virtual void setAtlasResourceImage( uint32_t _index, const ResourceImagePtr & _resourceImage ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<ParticleEmitterContainerInterface2> ParticleEmitterContainerInterface2Ptr;
    //////////////////////////////////////////////////////////////////////////
    struct RenderMaterialStage;
    //////////////////////////////////////////////////////////////////////////
	class ParticleSystemInterface2
		: public ServiceInterface
	{
		SERVICE_DECLARE( "ParticleSystem2" )

	public:
		virtual ParticleEmitterContainerInterface2Ptr createEmitterContainerFromMemory( const InputStreamInterfacePtr & _stream, const ArchivatorInterfacePtr & _archivator, const ConstString & _whoName ) = 0;
        virtual ParticleEmitterInterfacePtr createEmitter( const ParticleEmitterContainerInterface2Ptr & _container ) = 0;

	public:
		virtual const RenderMaterialStage * getMaterialStage( int _index ) const = 0;
		virtual const ResourceImagePtr & getResourceImage( int _index ) const = 0;

    public:
        virtual uint32_t getEmitterCount() const = 0;
	};
    //////////////////////////////////////////////////////////////////////////
#   define PARTICLE_SYSTEM2()\
	((ParticleSystemInterface2 *)SERVICE_GET(Mengine::ParticleSystemInterface2))
    //////////////////////////////////////////////////////////////////////////
	class ParticleServiceInterface2
		: public ServiceInterface
	{
		SERVICE_DECLARE( "ParticleService2" )

	public:
		virtual bool isAvailable() const = 0;

	public:
		virtual ParticleEmitterContainerInterface2Ptr createEmitterContainerFromFile( const FileGroupInterfacePtr& _fileGroupName, const FilePath & _fileName, const ConstString & _whoName ) = 0;

	public:
		virtual uint32_t getMaxParticlesCount() const = 0;
	};
    //////////////////////////////////////////////////////////////////////////
#   define PARTICLE_SERVICE2()\
	((ParticleServiceInterface2 *)SERVICE_GET(Mengine::ParticleServiceInterface2))
}
