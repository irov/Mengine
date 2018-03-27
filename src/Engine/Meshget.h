#pragma once

#include "Interface/MovieKeyFrameInterface.h"

#include "Kernel/Node.h"
#include "Kernel/Materialable.h"

#include "Kernel/ResourceImage.h"

#include "Core/ColourValue.h"
#include "Core/ValueInterpolator.h"
#include "Core/RenderVertex2D.h"

#include "math/mat3.h"
#include "math/vec4.h"
#include "math/mat4.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	enum MeshgetEventFlag
	{
		EVENT_MESHGET_UPDATE = 0
	};
    //////////////////////////////////////////////////////////////////////////
    class MeshgetEventReceiver
        : public EventReceiver
    {
    public:
        virtual void onMeshgetUpdate( float _current, float _timing ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<MeshgetEventReceiver> MeshgetEventReceiverPtr;
	//////////////////////////////////////////////////////////////////////////
	class Meshget
		: public Node
		, public Eventable
		, public Materialable
	{
        EVENT_RECEIVER( MeshgetEventReceiver );

	public:
		Meshget();
		~Meshget() override;

	public:
		void setResourceImage( const ResourceImagePtr & _resourceImage );
		const ResourceImagePtr & getResourceImage() const;

	public:
		bool setVertices( const pybind::list & _positions, const pybind::list & _uv, const pybind::list & _colors, const pybind::list & _indices );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void _update( float _current, float _timing ) override;
		void _render( RenderServiceInterface * _renderService, const RenderState * _state ) override;

	protected:
		void _updateBoundingBox( mt::box2f & _boundingBox ) const override;

	protected:
		void _invalidateColor() override;
		void _invalidateWorldMatrix() override;

	protected:
		void invalidateVerticesWM();
		void updateVerticesWM() const;

	protected:
		void invalidateVerticesColor();
		void updateVerticesColor() const;

	protected:
		inline const TVectorRenderVertex2D & getVerticesWM() const;

	protected:
		RenderMaterialInterfacePtr _updateMaterial() const override;

	protected:
		ResourceHolder<ResourceImage> m_resourceImage;
		
		typedef stdex::vector<mt::vec3f> TVectorPosition;
		typedef stdex::vector<mt::vec2f> TVectorUV;
		typedef stdex::vector<mt::vec4f> TVectorColor;
		TVectorPosition m_positions;
		TVectorUV m_uvs;
		TVectorColor m_colors;

		TVectorRenderIndices m_indices;

		mutable TVectorRenderVertex2D m_verticesWM;
		
		bool m_solid;
				
		mutable bool m_invalidateVerticesWM;
		mutable bool m_invalidateVerticesColor;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const TVectorRenderVertex2D & Meshget::getVerticesWM() const
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
