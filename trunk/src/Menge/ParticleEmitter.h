#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Animatable.h"

#   include "ResourceEmitterContainer.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ParticleSystemInterface.h"

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
	
	class ParticleEmitter
		: public Node
		, public Animatable
	{
	public:
		ParticleEmitter();
		~ParticleEmitter();

	public:
		bool _play( float _time ) override;
		bool _restart( float _time, uint32_t _enumerator ) override;
		void _stop( uint32_t _enumerator ) override;
		void _end( uint32_t _enumerator ) override;
		bool _interrupt( uint32_t _enumerator ) override;

	public:
		void pause();
		void restart();

		void setLeftBorder( float _leftBorder );
		void playFromPosition( float _pos );

		void setEmitterTranslateWithParticle( bool _with );
		void setEmitterPosition( const mt::vec3f & _position );
		void setEmitterRelative( bool _relative );
		void setStartPosition( float _pos );
        		
		//for astralax plugin
		mt::vec3f getEmitterPosition() const;
		mt::box2f getEmitterBoundingBox() const;
		
		void setEmitterImage( const ConstString & _emitterImageName );
        void removeEmitterImage();

		bool changeEmitterPolygon( const Polygon & _polygon );
        void removeEmitterPolygon();

        void setResourceEmitterContainer( ResourceEmitterContainer * _resourceEmitterContainer );
        ResourceEmitterContainer * getResourceEmitterContainer() const;

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
		void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera ) override;

		void _updateBoundingBox( mt::box2f& _boundingBox ) const override;
		void _invalidateWorldMatrix() override;

	private:
		bool compileEmitterImage_();
        bool compilePolygon_();

    protected:
		bool updateParticleVertex_();
        void updateRelative_();

    protected:
        void updateVertexWM_();
		void updateBB_();

	protected:
		void updateMaterial_();
		void invalidateMaterial_();

	protected:
		inline const RenderMaterialInterfacePtr & getMaterial( uint32_t _index );

	protected:
		ResourceHolder<ResourceEmitterContainer> m_resourceEmitterContainer;

		ConstString m_emitterName;

		ConstString m_emitterImageName;

		ParticleEmitterInterfacePtr m_emitter;

        Polygon m_polygon;

        bool m_emitterRelative;

        mt::vec3f m_emitterPosition;

        bool m_randomMode;

		float m_startPosition;

		RenderMaterialInterfacePtr m_materials[ MENGINE_PARTICLE_MAX_ATLAS_TEXTURE * 2 ]; //intensive and non intensive
		bool m_invalidateMaterial;

		//TVectorVertex2D m_vertices;
		RenderVertex2D * m_vertices;
		uint32_t m_verticesCount;

		struct Batch
		{
			TVectorRenderVertex2D::size_type begin;
			TVectorRenderVertex2D::size_type size;
			RenderMaterialInterfacePtr material;

			mt::box2f bb;
		};

		typedef stdex::vector<Batch> TVectorBatchs;
		TVectorBatchs m_batchs;

		bool m_emitterTranslateWithParticle;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const RenderMaterialInterfacePtr & ParticleEmitter::getMaterial( uint32_t _index )
	{
		if( m_invalidateMaterial == true )
		{
			this->updateMaterial_();
		}

		const RenderMaterialInterfacePtr & material = m_materials[_index];

		return material;
	}
}
