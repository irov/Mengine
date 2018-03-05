#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Materialable.h"

#   include "Kernel/ResourceImage.h"

#	include "Core/ColourValue.h"
#	include "Core/ValueInterpolator.h"
#   include "Core/RenderVertex2D.h"
#   include "Core/RenderIndices.h"

#	include "Math/mat3.h"
#	include "Math/vec4.h"
#	include "Math/mat4.h"

namespace Menge
{
	class Grid2D
		: public Node
		, public Materialable
	{
	public:
		Grid2D();
		~Grid2D();

	public:
		void setResourceImage( const ResourceImagePtr & _resourceImage );
		const ResourceImagePtr & getResourceImage() const;

	public:
		void setAngle( const mt::vec2f & _offset, float _angle );

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
		void _update( float _current, float _timing ) override;
		void _render( RenderServiceInterface * _renderService, const RenderObjectState * _state ) override;

    protected:
		void _invalidateWorldMatrix() override;

		void _updateBoundingBox( mt::box2f & _boundingBox ) const override;
		
	protected:
		void updateVerticesWM_();

	protected:
		RenderMaterialInterfacePtr _updateMaterial() const override;

	protected:
		inline const RenderVertex2D * getVerticesWM();

	protected:
		ResourceHolder<ResourceImage> m_resourceImage;

		float m_width;
		float m_height;

		float m_angle;
		mt::vec2f m_offset;

		uint32_t m_countX;
		uint32_t m_countY;
		
		TVectorRenderVertex2D m_vertices;
		TVectorRenderVertex2D m_verticesWM;

		TVectorRenderIndices m_indices;
				
		mutable bool m_invalidateVerticesWM;
    };
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
