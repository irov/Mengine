#	pragma once

#	include "Interface/ParticleSystemInterface.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(push, 0) 
#	endif 

#	include "magic.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(pop) 
#	endif

#	include <stdex/stl_string.h>
#	include <stdex/stl_vector.h>

namespace Menge
{
	class AstralaxEmitter2 
		: public ParticleEmitterInterface
	{
	public:
		AstralaxEmitter2();
		~AstralaxEmitter2();

    public:
        bool initialize( ServiceProviderInterface * _serviceProvider, HM_EMITTER _id, const ConstString & _containerName );
		void finalize();

	public:
		const ConstString & getContainerName() const override;
		
	public:
		void play() override;
		void stop() override;
		void pause() override;
		void restart() override;
		bool update( float _timing, bool & _stop ) override;
		void setLeftBorder( float _leftBorder ) override;
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
		bool flushParticles( ParticleMesh * _meshes, size_t _meshLimit, ParticleVertices * _particles, size_t _particlesLimit, ParticleEmitterRenderFlush & _flush ) override;

	public:
		//void getBoundingBox( int & left, int & top, int & right, int & bottom )  const override;
		void getBoundingBox( mt::box2f& _box ) const override;
		//void getBoundingBox( Rect & _rect )  const override;
		void setEmitterTranslateWithParticle( bool _value ) override;
		bool isIntensive() const override;

	public:
		bool changeEmitterImage( int _width, int _height, unsigned char* _data, int _bytes ) override;
		bool changeEmitterModel( float * _points, int _count ) override;
		
	public:
		void setPosition( const mt::vec3f & _pos ) override;
		void getPosition( mt::vec3f & _pos ) override;

        void getBasePosition( mt::vec3f & _pos ) override;

		void setScale( float _scale ) override;

		void setAngle( float _radians ) override;

        bool getBackgroundBox( mt::box2f & _box ) override;

	public:
		HM_EMITTER getId() const;

		bool inInterval() const;
		bool createFirstRenderedParticlesList( MAGIC_RENDERING * _rendering );

    public:
		float getUpdateTemp() const;

    protected:
        bool setupBasePosition_();
		
	protected:
        ServiceProviderInterface * m_serviceProvider; 
		
		HM_EMITTER m_emitterId;

        mt::vec3f m_basePosition;		

		ConstString m_containerName;

		double m_tempScale;
		double m_leftBorder;
		double m_rightBorder;
		
        double m_rate;
		double m_total_rate;

        double m_time;

		float m_width;
		float m_height;
		
		float m_angle;

		bool m_start;
		bool m_looped;

		bool m_background;
        bool m_firstUpdate;
	};

	typedef stdex::intrusive_ptr<AstralaxEmitter2> AstralaxEmitter2Ptr;
}