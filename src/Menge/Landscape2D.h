#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Materialable.h"

#   include "Kernel/ResourceImage.h"

#	include "Core/ColourValue.h"
#	include "Core/ValueInterpolator.h"

#	include "Interface/RenderSystemInterface.h"

#	include "Math/mat3.h"
#	include "Math/vec4.h"
#	include "Math/mat4.h"

namespace Menge
{
	typedef stdex::vector<ResourceImage *> TVectorResourceImage;

	struct RenderMaterial;
	struct RenderMaterialGroup;    

	struct Landscape2DElement
	{
		ResourceImage * image;
		RenderMaterialInterfacePtr material;
		uint32_t i;
		uint32_t j;
		mt::box2f bb;
		mt::box2f bb_wm;
	};

	typedef stdex::vector<Landscape2DElement> TVectorLandscape2DElements;
	
	class Landscape2D 
		: public Node
		, public Materialable
	{
	public:
		Landscape2D();
		~Landscape2D();

	public:
		void setBackParts( const TVectorResourceImage & _images, uint32_t _countX, uint32_t _countY, float _width, float _height );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void _render( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state ) override;

    protected:
		void _invalidateWorldMatrix() override;
		
	protected:
		void updateVerticesWM_();
		void updateElementWM_();

	protected:
		inline const RenderVertex2D * getVerticesWM( size_t _offset );
		inline TVectorLandscape2DElements & getElementWM();
		       
	protected:
		bool compileResources_();

	protected:
		RenderMaterialInterfacePtr _updateMaterial() const override;

	protected:
		TVectorResourceImage m_images;

		uint32_t m_elementCountX;
		uint32_t m_elementCountY;
		float m_elementWidth;
		float m_elementHeight;

		TVectorLandscape2DElements m_elements;
				
		TVectorRenderVertex2D m_verticesWM;
		
		mutable bool m_invalidateVerticesWM;
		mutable bool m_invalidateElementWM;
    };
	//////////////////////////////////////////////////////////////////////////
	inline const RenderVertex2D * Landscape2D::getVerticesWM( size_t _offset )
	{
		if( m_invalidateVerticesWM == true )
		{
			this->updateVerticesWM_();
		}

		return &m_verticesWM[_offset];
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
