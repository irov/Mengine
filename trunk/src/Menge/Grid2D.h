#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Shape.h"

#   include "Kernel/ResourceImage.h"

#	include "Core/ColourValue.h"
#	include "Core/ValueInterpolator.h"

#	include "Interface/RenderSystemInterface.h"

#	include "math/mat3.h"
#	include "math/vec4.h"
#	include "math/mat4.h"

namespace Menge
{
	class Grid2D
		: public Node
	{
	public:
		Grid2D();
		~Grid2D();

	public:
		void setResourceImage( ResourceImage * _resourceImage );
		ResourceImage * getResourceImage() const;

	public:
		void setWidth( float _width );
		float getWidth() const;

		void setHeight( float _height );
		float getHeight() const;

		void setCountX( uint32_t _count );
		uint32_t getCountX() const;

		void setCountY( uint32_t _count );
		uint32_t getCountY() const;

		bool setGridColor( uint32_t _i, uint32_t _j, const ColourValue & _value );
		bool getGridColor( uint32_t _i, uint32_t _j, ColourValue & _value ) const;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera ) override;

    protected:
		void _invalidateWorldMatrix() override;
		
	protected:
		void updateVerticesWM_();
		void updateMaterial_();

	protected:
		inline const RenderVertex2D * getVerticesWM();
		inline const RenderMaterialInterfacePtr & getMaterial();
		       
	protected:
		ResourceHolder<ResourceImage> m_resourceImage;

		RenderMaterialInterfacePtr m_material;

		bool m_blendAdd;

		float m_width;
		float m_height;

		uint32_t m_countX;
		uint32_t m_countY;
		
		TVectorRenderVertex2D m_vertices;
		TVectorRenderVertex2D m_verticesWM;

		TVectorRenderIndices2D m_indices;

		mutable bool m_invalidateMaterial;
		mutable bool m_invalidateVerticesWM;
    };
	//////////////////////////////////////////////////////////////////////////
	inline const RenderMaterialInterfacePtr & Grid2D::getMaterial()
	{
		if( m_invalidateMaterial == true )
		{
			this->updateMaterial_();
		}

		return m_material;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const RenderVertex2D * Grid2D::getVerticesWM()
	{
		if( m_invalidateVerticesWM == true )
		{
			this->updateVerticesWM_();
		}

		return &m_verticesWM[0];
	}
}
