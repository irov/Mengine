#pragma once

#include "Interface/MovieKeyFrameInterface.h"

#include "Kernel/Node.h"
#include "Kernel/Materialable.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/ResourceHolder.h"

#include "Kernel/ColourValue.h"
#include "Kernel/ValueInterpolator.h"
#include "Kernel/RenderVertex2D.h"

#include "math/mat3.h"
#include "math/vec4.h"
#include "math/mat4.h"

namespace Mengine
{
	class Mesh2D
		: public Node
		, public Materialable
	{
	public:
		Mesh2D();
		~Mesh2D() override;

    public:
		void setResourceImage( const ResourceImagePtr & _resourceImage );
		const ResourceImagePtr & getResourceImage() const;
        
	public:
		void setFrameShape( const MovieFrameShape * _shape );

	protected:
		bool _compile() override;
		void _release() override;

		void _render( const RenderContext * _state ) override;

		void _updateBoundingBox( mt::box2f & _boundingBox ) const override;
        void _invalidateColor() override;
		void _invalidateWorldMatrix() override;
		        
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
		RenderMaterialInterfacePtr _updateMaterial() const override;

	protected:
		ResourceHolder<ResourceImage> m_resourceImage;

		const MovieFrameShape * m_shape;

		mutable RenderVertex2D m_verticesWM[MENGINE_MOVIE_SHAPE_MAX_VERTEX];
		
		uint32_t m_vertexCount;
		uint32_t m_indicesCount;

		bool m_solid;

		mutable bool m_invalidateVerticesLocal;
		mutable bool m_invalidateVerticesWM;
		mutable bool m_invalidateVerticesColor;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Mesh2D> Mesh2DPtr;
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
