#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/MovieKeyFrameInterface.h"

#	include "Kernel/Node.h"

#   include "Kernel/ResourceImage.h"

#	include "Core/ColourValue.h"
#	include "Core/ValueInterpolator.h"


#	include "math/mat3.h"
#	include "math/vec4.h"
#	include "math/mat4.h"

namespace Menge
{
	struct RenderMaterial;
	struct RenderMaterialGroup;
	
	class Mesh2D
		: public Node
	{
	public:
		Mesh2D();
		~Mesh2D();

    public:
        void setResourceImage( ResourceImage * _resourceImage );
        ResourceImage * getResourceImage() const;
        
    public:
		void setBlendAdd( bool _value );
		bool isBlendAdd() const;

	public:
		void setFrameShape( const MovieFrameShape * _shape );

	protected:
		bool _compile() override;
		void _release() override;

		void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera ) override;

		void _updateBoundingBox( mt::box2f & _boundingBox ) const override;
        void _invalidateColor() override;
		void _invalidateWorldMatrix() override;

    protected:
        void invalidateMaterial();
        void updateMaterial();
        inline const RenderMaterialInterfacePtr & getMaterial();

		        
	protected:
		bool compileResource_();

	protected:
		void invalidateVertices();
		void updateVertices() const;

	protected:
		void invalidateVerticesWM();
		void updateVerticesWM() const;

	protected:
		void invalidateVerticesColor();
		void updateVerticesColor() const;

	protected:
		inline const RenderVertex2D * getVerticesWM() const;

	protected:
		ResourceHolder<ResourceImage> m_resourceImage;

		RenderMaterialInterfacePtr m_material;

		const MovieFrameShape * m_shape;

		mutable RenderVertex2D m_verticesWM[MENGINE_MOVIE_SHAPE_MAX_VERTEX];
		
		uint32_t m_vertexCount;
		uint32_t m_indicesCount;

		bool m_blendAdd;
		bool m_solid;

		bool m_invalidateMaterial;
		mutable bool m_invalidateVerticesLocal;
		mutable bool m_invalidateVerticesWM;
		mutable bool m_invalidateVerticesColor;
    };
    //////////////////////////////////////////////////////////////////////////
    inline const RenderMaterialInterfacePtr & Mesh2D::getMaterial()
    {
        if( m_invalidateMaterial == true )
        {
            this->updateMaterial();
        }

        return m_material;
    }
	//////////////////////////////////////////////////////////////////////////
	inline const RenderVertex2D * Mesh2D::getVerticesWM() const
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
