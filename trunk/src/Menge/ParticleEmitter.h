#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Animatable.h"

#	include "../Interface/RenderSystemInterface.h"
#	include "../Interface/ParticleSystemInterface.h"

#	include "Math/mat3.h"

#	include "Core/Polygon.h"

#	include <vector>

namespace Menge
{
	class ResourceEmitterContainer;
	class EmitterInterface;
	class ResourceImage;
	class ResourceImageDefault;
	struct RenderMaterial;
	

	class ParticleEmitter
		: public Node
		, public ParticleEmitterListenerInterface
		, public Animatable
	{
	public:
		ParticleEmitter();
		~ParticleEmitter();

	public:
		bool _play( float _time ) override;
		bool _restart( float _time, size_t _enumerator ) override;
		void _stop( size_t _enumerator ) override;
		void _end( size_t _enumerator ) override;
		bool _interrupt( size_t _enumerator ) override;

	public:
		void pause();
		void restart();

		void setLeftBorder( float _leftBorder );
		void playFromPosition( float _pos );

		void setEmitterTranslateWithParticle( bool _with );
		void setEmitterRelative( bool _relative );
		void setStartPosition( float _pos );
        		
		//for astralax plugin
		mt::vec3f getEmitterPosition() const;
		mt::box2f getEmitterBoundingBox() const;
		
		void setEmitterImage( const ConstString & _emitterImageName );
        void removeEmitterImage();

		bool changeEmitterPolygon( const Polygon & _polygon );
        void removeEmitterPolygon();
	
		void onParticleEmitterStopped() override;
		void setResource( const ConstString& _resourceName );
		void setEmitter( const ConstString& _emitterName ); 

		float getDuration() const;
		float getLeftBorder() const;
		float getRightBorder() const;
		const ConstString& getEmitterName() const;
		
		void setRandomMode( bool _randomMode );
		bool getRandomMode() const;

    protected:
        void getRelativePosition( mt::vec3f & _pos );

    protected:
        void _setLoop( bool _value );

	protected:
		void _setEventListener( PyObject * _listener );

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _current, float _timing ) override;
		void _render( RenderCameraInterface * _camera ) override;

		void _updateBoundingBox( mt::box2f& _boundingBox ) override;
		void _invalidateWorldMatrix() override;
        void _invalidateColor() override;

	private:
		bool compileEmitterImage_();
        bool compilePolygon_();

    protected:
		bool updateParticleVertex_( RenderCameraInterface * _camera );
        void updateRelative_();

    protected:
        void updateVertexWM_();

	protected:
		ResourceEmitterContainer * m_resource;
		ConstString m_resourceName;

		ConstString m_emitterName;

		ConstString m_emitterImageName;

		EmitterInterface * m_interface;

        Polygon m_polygon;

        bool m_emitterRelative;

        mt::vec3f m_emitterPosition;

        bool m_randomMode;

		float m_startPosition;

		const RenderMaterial * m_materials[2]; //intensive and non intensive

		//TVectorVertex2D m_vertices;
		Vertex2D * m_vertices;
		size_t m_verticesCount;

		struct Batch
		{
			TVectorVertex2D::size_type begin;
			TVectorVertex2D::size_type size;
			const RenderTextureInterface * texture[1];
			const RenderMaterial * material;
		};

		typedef std::vector<Batch> TVectorBatchs;
		TVectorBatchs m_batchs;

		bool m_emitterTranslateWithParticle;
        bool m_emitterChangeRendering;
        bool m_emitterChangeRenderingExtra;
	};
}
