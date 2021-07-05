#pragma once

#include "AstralaxInterface.h"
#include "AstralaxIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AstralaxEmitter2
        : public AstralaxEmitterInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( AstralaxEmitter2 );

    public:
        AstralaxEmitter2();
        ~AstralaxEmitter2() override;

    public:
        bool initialize( const AstralaxEmitterContainerInterfacePtr & _container );
        void finalize();

    public:
        void play() override;
        void stop() override;
        void pause() override;
        void resume() override;
        void restart() override;
        bool update( float _time, bool * const _stop ) override;
        void seek( float _time ) override;
        void setLoop( bool _loop ) override;
        bool getLoop() const override;

        float getLeftBorder() const override;
        float getRightBorder() const override;
        float getDuration() const override;

        void setRandomMode( bool _randomMode ) override;
        bool getRandomMode() const override;

    public:
        void interrupt() override;

    public:
        bool is3d() const override;

        bool getCamera( AstralaxCamera * const _camera ) const override;

    public:
        bool prepareParticles( AstralaxEmitterRenderFlush * const _flush ) override;
        bool flushParticles( AstralaxMesh * const _meshes, uint32_t _meshLimit, RenderVertex2D * const _vertices, RenderIndex * const _indices, AstralaxEmitterRenderFlush * const _flush ) override;

    public:
        const mt::box2f & getBoundingBox() const override;
        void setEmitterTranslateWithParticle( bool _value ) override;

    public:
        bool changeEmitterImage( uint32_t _width, uint32_t _height, const void * _data, size_t _bytes ) override;
        bool changeEmitterModel( float * const _points, uint32_t _count ) override;

    public:
        bool setPositionProvider( const AstralaxPositionProviderInterfacePtr & _positionProvider ) override;
        bool setCameraProvider( const AstralaxCameraProviderInterfacePtr & _cameraProvider ) override;

        void setScale( float _scale ) override;

        void setAngle( float _radians ) override;

    public:
        HM_EMITTER getId() const;

        bool inInterval() const;

    public:
        float getUpdateTemp() const;

    protected:
        bool setupBasePosition_();

    protected:
        AstralaxEmitterContainerInterfacePtr m_container;

        HM_EMITTER m_emitterId;

        AstralaxPositionProviderInterfacePtr m_positionProvider;
        AstralaxCameraProviderInterfacePtr m_cameraProvider;

        float m_updateSpeed;
        double m_leftBorder;
        double m_rightBorder;
        double m_duration;

        double m_time;

        mt::vec4f m_rect;

        float m_angle;

        mt::box2f m_box;

        bool m_start;
        bool m_looped;

        bool m_is3d;
        bool m_relative;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AstralaxEmitter2> AstralaxEmitter2Ptr;
    //////////////////////////////////////////////////////////////////////////
}
