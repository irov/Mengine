#pragma once

#include "Interface/ParticleSystemInterface.h"

#include "Kernel/ServantBase.h"

#  ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(push, 0) 
#endif 

#include "magic.h"

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(pop) 
#endif

namespace Mengine
{
    class AstralaxParticleSystem2;

    class AstralaxEmitter2
        : public ServantBase<ParticleEmitterInterface>
    {
    public:
        AstralaxEmitter2();
        ~AstralaxEmitter2() override;

    public:
        bool initialize( AstralaxParticleSystem2 * _particleSystem, const ParticleEmitterContainerInterface2Ptr & _container );
        void finalize();

    public:
        void play() override;
        void stop() override;
        void pause() override;
        void resume() override;
        void restart() override;
        bool update( float _time, bool * _stop ) override;
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

        bool getCamera( ParticleCamera & _camera ) const override;

    public:
        bool prepareParticles( ParticleEmitterRenderFlush & _flush ) override;
        bool flushParticles( ParticleMesh * _meshes, uint32_t _meshLimit, RenderVertex2D * _vertices, RenderIndex * _indices, ParticleEmitterRenderFlush & _flush ) override;

    public:
        //void getBoundingBox( int & left, int & top, int & right, int & bottom )  const override;
        const mt::box2f & getBoundingBox() const override;
        //void getBoundingBox( Rect & _rect )  const override;
        void setEmitterTranslateWithParticle( bool _value ) override;

    public:
        bool changeEmitterImage( uint32_t _width, uint32_t _height, const void * _data, size_t _bytes ) override;
        bool changeEmitterModel( float * _points, uint32_t _count ) override;

    public:
        bool setPositionProvider( ParticlePositionProviderInterface * _positionProvider ) override;
        bool setCameraProvider( ParticleCameraProviderInterface * _cameraProvider ) override;

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
        AstralaxParticleSystem2 * m_particleSystem;

        ParticleEmitterContainerInterface2Ptr m_container;

        HM_EMITTER m_emitterId;

        ParticlePositionProviderInterface * m_positionProvider;
        ParticleCameraProviderInterface * m_cameraProvider;

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

    typedef IntrusivePtr<AstralaxEmitter2> AstralaxEmitter2Ptr;
}
