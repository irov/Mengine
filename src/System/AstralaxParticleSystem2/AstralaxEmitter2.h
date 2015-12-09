#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(push, 0) 
#	endif 

#	include "magic.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(pop) 
#	endif

namespace Menge
{
	class AstralaxEmitter2 
		: public ParticleEmitterInterface
	{
	public:
		AstralaxEmitter2();
		~AstralaxEmitter2();

    public:
		bool initialize( ServiceProviderInterface * _serviceProvider, const ParticleEmitterContainerInterface2Ptr & _container, HM_EMITTER _id );
		void finalize();
		
	public:
		void play() override;
		void stop() override;
		void pause() override;
		void resume() override;
		void restart() override;
		bool update( float _timing, bool & _stop ) override;
		void seek( float _timming ) override;
		void setLoop( bool _loop ) override;
		bool getLoop() const override;
		
		float getLeftBorder() const override;
		float getRightBorder() const override;
		float getDuration() const override;
		
		void setRandomMode( bool _randomMode ) override;
		bool getRandomMode() const override;

		bool isBackground() const override;

	public:
		void interrupt() override;

	public:
		bool is3d() const override;

		bool getCamera( ParticleCamera & _camera ) const override;

	public:		
		bool prepareParticles( ParticleEmitterRenderFlush & _flush ) override;
		bool flushParticles( ParticleMesh * _meshes, uint32_t _meshLimit, RenderVertex2D * _vertices, RenderIndices * _indices, ParticleEmitterRenderFlush & _flush );

	public:
		//void getBoundingBox( int & left, int & top, int & right, int & bottom )  const override;
		const mt::box2f & getBoundingBox() const override;
		//void getBoundingBox( Rect & _rect )  const override;
		void setEmitterTranslateWithParticle( bool _value ) override;

	public:
		bool changeEmitterImage( uint32_t _width, uint32_t _height, unsigned char* _data, size_t _bytes ) override;
		bool changeEmitterModel( float * _points, uint32_t _count ) override;
		
	public:
		bool setPositionProvider( ParticlePositionProviderInterface * _positionProvider ) override;

		void setScale( float _scale ) override;

		void setAngle( float _radians ) override;

        bool getBackgroundBox( mt::box2f & _box ) override;

	public:
		HM_EMITTER getId() const;

		bool inInterval() const;

	public:
		float getUpdateTemp() const;

    protected:
        bool setupBasePosition_();
		
	protected:
        ServiceProviderInterface * m_serviceProvider;

		ParticleEmitterContainerInterface2Ptr m_container;
		
		HM_EMITTER m_emitterId;

		ParticlePositionProviderInterface * m_positionProvider;

		float m_updateSpeed;
		double m_leftBorder;
		double m_rightBorder;
		
        double m_time;

		mt::vec4f m_rect;
		
		float m_angle;

		mt::box2f m_box;

		bool m_start;
		bool m_looped;

		bool m_background;
		bool m_relative;
	};

	typedef stdex::intrusive_ptr<AstralaxEmitter2> AstralaxEmitter2Ptr;
}