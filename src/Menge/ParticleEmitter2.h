#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Animatable.h"

#   include "ResourceParticle.h"

#	include "../Interface/RenderSystemInterface.h"
#	include "../Interface/ParticleSystemInterface.h"

#	include "Math/mat3.h"

#	include "Core/Polygon.h"

#	include <stdex/stl_vector.h>

#	define MENGINE_PARTICLE_MAX_ATLAS_TEXTURE 8

namespace Menge
{
	class ParticleEmitterInterface;
	class ResourceImage;
	class ResourceImageDefault;
	struct RenderMaterial;
	
	class ParticleEmitter2
		: public Node
		, public Animatable
		, public ParticlePositionProviderInterface
	{
	public:
		ParticleEmitter2();
		~ParticleEmitter2();

	public:
		bool _play( float _time ) override;
		bool _restart( float _time, uint32_t _enumerator ) override;
		void _pause( uint32_t _enumerator ) override;
		void _resume( uint32_t _enumerator ) override;
		void _stop( uint32_t _enumerator ) override;
		void _end( uint32_t _enumerator ) override;
		bool _interrupt( uint32_t _enumerator ) override;

	public:
		void setEmitterTranslateWithParticle( bool _with );
		void setEmitterRelative( bool _relative );

		void changeEmitterImage( const ConstString & _emitterImageName );
        void removeEmitterImage();

		bool changeEmitterPolygon( const Polygon & _polygon );
        void removeEmitterPolygon();
	
        void setResourceParticle( ResourceParticle * _resourceParticle );
        ResourceParticle * getResourceParticle() const;

		float getDuration() const;
		
		void setEmitterRandomMode( bool _randomMode );
		bool getEmitterRandomMode() const;

    protected:
        void _setLoop( bool _value ) override;

	protected:
		void _setEventListener( const pybind::dict & _listener ) override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _current, float _timing ) override;
		void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderClipplaneInterface * _clipplane ) override;

		void _updateBoundingBox( mt::box2f& _boundingBox ) const override;

	private:
		bool compileEmitterImage_( const ParticleEmitterInterfacePtr & _emitter );
		bool compilePolygon_( const ParticleEmitterInterfacePtr & _emitter );

    protected:
		void updateVertexColor_( RenderVertex2D * _vertices, uint32_t _verticesCount );
		void updateVertexWM_( RenderVertex2D * _vertices, uint32_t _verticesCount );

	protected:
		void onProviderEmitterPosition( mt::vec3f & _position ) override;

	protected:
		ResourceHolder<ResourceParticle> m_resourceParticle;

		ConstString m_emitterImageName;

		ParticleEmitterInterfacePtr m_emitter;

        Polygon m_polygon;

        bool m_emitterRelative;

        bool m_randomMode;
				
		RenderVertex2D * m_vertices;
		uint32_t m_vertexCount;

		RenderIndices * m_indicies;
		uint32_t m_indexCount;		

		bool m_emitterTranslateWithParticle;
	};
}
