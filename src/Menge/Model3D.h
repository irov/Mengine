#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/Model3DInterface.h"

#	include "Kernel/Node.h"
#	include "Kernel/Animatable.h"

#   include "ResourceImage.h"
#	include "ResourceModel3D.h"

#	include "Core/ColourValue.h"
#	include "Core/ValueInterpolator.h"

#	include "Camera3D.h"

#	include "math/mat3.h"
#	include "math/vec4.h"
#	include "math/mat4.h"

namespace Menge
{
	struct RenderMaterial;
	struct RenderMaterialGroup;

	class Model3D
		: public Node
		, public Animatable
	{
	public:
		Model3D();
		~Model3D();

	public:
		void setResourceModel3D( ResourceModel3D * _resourceImage );
		ResourceModel3D * getResourceModel3D() const;

	public:
		void setBlendAdd( bool _value );
		bool isBlendAdd() const;

	protected:
		void _setTiming( float _timming ) override;
		float _getTiming() const override;

		void _setFirstFrame() override;
		void _setLastFrame() override;

		void _setReverse( bool _value ) override;

	protected:
		bool _play( float _time ) override;
		bool _restart( float _time, size_t _enumerator ) override;
		void _stop( size_t _enumerator ) override;
		void _end( size_t _enumerator ) override;
		bool _interrupt( size_t _enumerator ) override;

	protected:
		size_t getFrame_( float _timing, float & _delthaTiming ) const;
		void updateCurrentFrame_();
		void setCurrentFrame_( size_t _frame );

	protected:
		void _update( float _current, float _timing ) override;
		void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera ) override;

		bool _activate() override;
		void _deactivate() override;

	protected:
		bool _compile() override;
		void _release() override;
				
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _invalidateColor() override;
		void _invalidateWorldMatrix() override;

	protected:
		void invalidateMaterial();
		void updateMaterial();
		inline const RenderMaterial * getMaterial();

	protected:
		void updateResource_();

	protected:
		bool compileResource_();

	protected:
		void invalidateVertices();
		void updateVertices();

	protected:
		void invalidateVerticesWM();
		void updateVerticesWM();

	protected:
		void invalidateVerticesColor();
		void updateVerticesColor();

	protected:
		inline const RenderVertex2D * getVerticesWM();

	protected:		
		ResourceHolder<ResourceModel3D> m_resourceModel;

		float m_frameTiming;
		size_t m_currentFrame;

		bool m_blendAdd;
		bool m_solid;

		const RenderMaterialGroup * m_materialGroup;
		const RenderMaterial * m_material;

		bool m_invalidateMaterial;

		size_t m_texturesNum;
		RenderTextureInterfacePtr m_textures[2];

		Camera3D * m_camera;

		const Model3DFrame * m_frame;
		bool m_invalidateVerticesLocal;

		RenderVertex2D m_verticesWM[MENGINE_MODEL_MAX_VERTEX];
		bool m_invalidateVerticesWM;

		size_t m_vertexCount;
		size_t m_indicesCount;

		bool m_invalidateVerticesColor;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const RenderMaterial * Model3D::getMaterial()
	{
		if( m_invalidateMaterial == true )
		{
			this->updateMaterial();
		}

		return m_material;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const RenderVertex2D * Model3D::getVerticesWM()
	{
		if( m_invalidateVerticesWM == true )
		{
			this->updateVerticesWM();
		}

		if( m_invalidateVerticesColor == true )
		{
			this->updateVerticesColor();
		}

		return m_verticesWM;
	}
}
