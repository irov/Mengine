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
	typedef stdex::vector<ResourceImage *> TVectorResourceImage;

	struct RenderMaterial;
	struct RenderMaterialGroup;    

	struct Landscape2DElement
	{
		ResourceImage * resource;
		RenderMaterialInterfacePtr material;
		size_t i;
		size_t j;
		mt::box2f bb;
		mt::box2f bb_wm;

		size_t refcount;
	};

	typedef stdex::vector<Landscape2DElement> TVectorLandscape2DElements;
	
	class Landscape2D 
		: public Node
	{
	public:
		Landscape2D();
		~Landscape2D();

	public:
		void setBackParts( const TVectorResourceImage & _images, size_t _countX, size_t _countY, float _width, float _height );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera ) override;

    protected:
		void _invalidateWorldMatrix() override;
		
	protected:
		void updateElementVertex_( const Landscape2DElement & _el, const mt::mat4f & _wm, RenderVertex2D * _vertices );

	protected:
		void updateVerticesWM_();
		void updateElementWM_();

	protected:
		inline const RenderVertex2D * getVerticesWM();
		inline TVectorLandscape2DElements & getElementWM();
		       
	protected:
		bool compileResources_();

	protected:
		TVectorResourceImage m_images;

		size_t m_elementCountX;
		size_t m_elementCountY;
		float m_elementWidth;
		float m_elementHeight;


		TVectorLandscape2DElements m_elements;
				
		TVectorRenderVertex2D m_verticesWM;
		
		mutable bool m_invalidateVerticesWM;
		mutable bool m_invalidateElementWM;
    };
	//////////////////////////////////////////////////////////////////////////
	inline const RenderVertex2D * Landscape2D::getVerticesWM()
	{
		if( m_invalidateVerticesWM == true )
		{
			this->updateVerticesWM_();
		}

		return &m_verticesWM[0];
	}
	//////////////////////////////////////////////////////////////////////////
	inline TVectorLandscape2DElements & Landscape2D::getElementWM()
	{
		if( m_invalidateElementWM == true )
		{
			this->updateElementWM_();
		}

		return m_elements;
	}
}
