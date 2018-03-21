#pragma once

#include "Kernel/Node.h"
#include "Kernel/Animatable.h"

#include "ResourceParticle.h"

#include "Interface/ParticleSystemInterface.h"

#include "math/mat3.h"

#include "Core/Polygon.h"

#include "stdex/stl_vector.h"

#	define MENGINE_PARTICLE_MAX_ATLAS_TEXTURE 8

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ParticleEmitter2EventFlag
    {
    };
    //////////////////////////////////////////////////////////////////////////
    class ParticleEmitter2EventReceiver
        : public AnimatableEventReceiver
    {
    public:
    };
    //////////////////////////////////////////////////////////////////////////
	class ParticleEmitter2
		: public Node
		, public Eventable
		, public Animatable
		, public ParticlePositionProviderInterface
		, public ParticleCameraProviderInterface
	{
        EVENT_RECEIVER( ParticleEmitter2EventReceiver );

	public:
		ParticleEmitter2();
		~ParticleEmitter2() override;

	public:
		bool _play( float _time ) override;
		bool _restart( uint32_t _enumerator, float _time ) override;
		void _pause( uint32_t _enumerator ) override;
		void _resume( uint32_t _enumerator, float _time ) override;
		void _stop( uint32_t _enumerator ) override;
		void _end( uint32_t _enumerator ) override;
		bool _interrupt( uint32_t _enumerator ) override;

	public:
		void setEmitterTranslateWithParticle( bool _translateWithParticle );
		void setEmitterPositionRelative( bool _positionRelative );
		void setEmitterCameraRelative( bool _cameraRelative );

		void setEmitterPositionProviderOriginOffset( const mt::vec3f & _originOffset );

		void changeEmitterImage( const ConstString & _emitterImageName );
        void removeEmitterImage();

		bool changeEmitterPolygon( const Polygon & _polygon );
        void removeEmitterPolygon();
	
		void setResourceParticle( const ResourceParticlePtr & _resourceParticle );
		const ResourceParticlePtr & getResourceParticle() const;

		float getDuration() const;
		
		void setEmitterRandomMode( bool _randomMode );
		bool getEmitterRandomMode() const;

    protected:
        void _setLoop( bool _value ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _current, float _timing ) override;
		void _render( RenderServiceInterface * _renderService, const RenderObjectState * _state ) override;

		void _updateBoundingBox( mt::box2f& _boundingBox ) const override;

	private:
		bool compileEmitterImage_( const ParticleEmitterInterfacePtr & _emitter );
		bool compilePolygon_( const ParticleEmitterInterfacePtr & _emitter );

    protected:
		void updateVertexColor_( RenderVertex2D * _vertices, uint32_t _verticesCount );
		void updateVertexWM_( RenderVertex2D * _vertices, uint32_t _verticesCount );

	protected:
		void onProviderEmitterPosition( mt::vec3f & _position ) override;
		void onProviderEmitterCamera( bool & _orthogonality, mt::vec3f & _position, mt::vec3f & _direction ) override;

	protected:
		ResourceHolder<ResourceParticle> m_resourceParticle;

		ConstString m_emitterImageName;

		ParticleEmitterInterfacePtr m_emitter;

        Polygon m_polygon;

		mt::vec3f m_positionProviderOriginOffset;

        bool m_emitterPositionRelative;
		bool m_emitterCameraRelative;

        bool m_randomMode;
				
		RenderVertex2D * m_vertices;
		uint32_t m_vertexCount;

		RenderIndex * m_indicies;
		uint32_t m_indexCount;		

		bool m_emitterTranslateWithParticle;
	};
}
