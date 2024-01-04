#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/RenderMaterialInterface.h"

#include "Kernel/String.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Color.h"
#include "Kernel/Magic.h"
#include "Kernel/RenderVertex2D.h"
#include "Kernel/RenderIndex.h"
#include "Kernel/ResourceImage.h"

#include "Kernel/Factorable.h"
#include "Kernel/Factorable.h"

#include "math/vec2.h"
#include "math/box2.h"

#include "math/mat4.h"
#include "math/uv4.h"

#ifndef MENGINE_PARTICLE_MAX_MESH
#define MENGINE_PARTICLE_MAX_MESH 1000
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct AstralaxMesh
    {
        uint32_t vertexOffset;
        uint32_t vertexCount;
        uint32_t indexOffset;
        uint32_t indexCount;

        int32_t texture[8];
        uint32_t textures;

        int32_t material;
    };
    //////////////////////////////////////////////////////////////////////////
    struct AstralaxEmitterRenderFlush
    {
        uint32_t meshCount;
        uint32_t vertexCount;
        uint32_t indexCount;

        uint32_t arrays;
        void * context;
    };
    //////////////////////////////////////////////////////////////////////////
    struct AstralaxCamera
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
    class AstralaxPositionProviderInterface
        : public Mixin
    {
    public:
        virtual void onProviderEmitterPosition( mt::vec3f * _position ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AstralaxPositionProviderInterface> AstralaxPositionProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AstralaxCameraProviderInterface
        : public Mixin
    {
    public:
        virtual void onProviderEmitterCamera( bool * _orthogonality, mt::vec3f * _position, mt::vec3f * _direction ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AstralaxCameraProviderInterface> AstralaxCameraProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AstralaxEmitterInterface
        : public Mixin
    {
    public:
        virtual void play() = 0;
        virtual void stop() = 0;
        virtual void pause() = 0;
        virtual void resume() = 0;
        virtual void restart() = 0;
        virtual void seek( float _time ) = 0;
    public:
        virtual bool update( float _time, bool * _stop ) = 0;

    public:
        virtual void setLoop( bool _loop ) = 0;
        virtual bool getLoop() const = 0;

    public:
        virtual void interrupt() = 0;

    public:
        virtual bool is3d() const = 0;

        virtual bool getCamera( AstralaxCamera * _camera ) const = 0;

    public:
        virtual bool prepareParticles( AstralaxEmitterRenderFlush * _flush ) = 0;
        virtual bool flushParticles( AstralaxMesh * _meshes, uint32_t _meshLimit, RenderVertex2D * _vertices, RenderIndex * _indices, AstralaxEmitterRenderFlush * _flush ) = 0;

    public:
        virtual const mt::box2f & getBoundingBox() const = 0;

    public:
        virtual float getLeftBorder() const = 0;
        virtual float getRightBorder() const = 0;
        virtual float getDuration() const = 0;

        virtual void setEmitterTranslateWithParticle( bool _value ) = 0;

    public:
        virtual bool changeEmitterImage( uint32_t _width, uint32_t _height, const void * _data, size_t _bytes ) = 0;
        virtual bool changeEmitterModel( float * _points, uint32_t _count ) = 0;

    public:
        virtual bool setCameraProvider( const AstralaxCameraProviderInterfacePtr & _cameraProvider ) = 0;
        virtual bool setPositionProvider( const AstralaxPositionProviderInterfacePtr & _positionProvider ) = 0;

        virtual void setScale( float _scale ) = 0;

        virtual void setAngle( float _radians ) = 0;

        virtual void setRandomMode( bool _randomMode ) = 0;
        virtual bool getRandomMode() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AstralaxEmitterInterface> AstralaxEmitterInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    DECLARE_MAGIC_NUMBER( MAGIC_PTZ, 'P', 'T', 'Z', '2', 2 );
    //////////////////////////////////////////////////////////////////////////
    class AstralaxEmitterContainerInterface
        : public Mixin
    {
    public:
        virtual bool isValid() const = 0;

    public:
        virtual bool setAtlasResourceImage( uint32_t _index, const ResourceImagePtr & _resourceImage ) = 0;
        virtual const ResourceImagePtr & getAtlasResourceImage( uint32_t _index ) const = 0;
        virtual uint32_t getAtlasResourceImageCount() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AstralaxEmitterContainerInterface> AstralaxEmitterContainerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AstralaxServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AstralaxService" )

    public:
        virtual AstralaxEmitterContainerInterfacePtr createEmitterContainerFromFile( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual AstralaxEmitterInterfacePtr createEmitter( const AstralaxEmitterContainerInterfacePtr & _container, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual const RenderMaterialInterfacePtr & getMaterial( const AstralaxMesh & _mesh ) const = 0;

    public:
        virtual uint32_t getEmitterCount() const = 0;
        virtual uint32_t getMaxParticlesCount() const = 0;

    public:
        virtual bool updateAtlas() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class UnknownAstralaxEmitterInterface
        : public UnknownInterface
    {
    public:
        virtual void setResourceAstralax( const ResourcePtr & _resource ) = 0;
        virtual const ResourcePtr & getResourceAstralax() const = 0;

        virtual void setEmitterPositionProviderOriginOffset( const mt::vec3f & _originOffset ) = 0;

        virtual void setEmitterTranslateWithParticle( bool _translateWithParticle ) = 0;
        virtual void setEmitterPositionRelative( bool _positionRelative ) = 0;
        virtual void setEmitterCameraRelative( bool _cameraRelative ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define ASTRALAX_SERVICE()\
    ((Mengine::AstralaxServiceInterface *)SERVICE_GET(Mengine::AstralaxServiceInterface))
//////////////////////////////////////////////////////////////////////////
