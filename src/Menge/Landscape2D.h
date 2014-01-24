#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Shape.h"

#   include "ResourceImage.h"

#	include "Core/ColourValue.h"
#	include "Core/ValueInterpolator.h"

#	include "../Interface/RenderSystemInterface.h"

#	include "math/mat3.h"
#	include "math/vec4.h"
#	include "math/mat4.h"

namespace Menge
{
	typedef std::vector<ResourceImage *> TVectorResourceImage;

	struct RenderMaterial;
	struct RenderMaterialGroup;    
	
	class Landscape2D 
		: public Node
	{
	public:
		Landscape2D();
		~Landscape2D();

	public:
		void setBackParts( const TVectorResourceImage & _images, size_t countX, size_t countY );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera ) override;

    protected:
		void _invalidateWorldMatrix() override;
		void invalidateVerticesWM();
		void updateVerticesWM_();

		inline const RenderVertex2D * getVerticesWM();
	
		       
	protected:
		bool compileResources_();

	protected:
		TVectorResourceImage m_images;

		size_t m_countX;
		size_t m_countY;		

		struct Element
		{
			ResourceImage * resource;
			RenderMaterialInterfacePtr material;
		};

		typedef std::vector<Element> TVectorElements;
		TVectorElements m_elements;
				
		TVectorRenderVertex2D m_verticesWM;
		bool m_invalidateVerticesWM;
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
}
