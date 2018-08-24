#pragma once

#include "Interface/MovieKeyFrameInterface.h"

#include "Kernel/Node.h"
#include "Kernel/Materialable.h"
#include "Kernel/Surface.h"
#include "Kernel/ColourValue.h"
#include "Kernel/ValueInterpolator.h"
#include "Kernel/RenderVertex2D.h"
#include "Kernel/BaseEventation.h"

#include "Config/Vector.h"

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
        virtual void onMeshgetUpdate( const UpdateContext * _context ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MeshgetEventReceiver> MeshgetEventReceiverPtr;
	//////////////////////////////////////////////////////////////////////////
	class Meshget
		: public Node
        , public BaseEventation
	{
        DECLARE_EVENTABLE( MeshgetEventReceiver );

	public:
		Meshget();
		~Meshget() override;

    public:
        void setSurface( const SurfacePtr & _surface );
        const SurfacePtr & getSurface() const;

	public:
		bool setVertices( const pybind::list & _positions, const pybind::list & _uv, const pybind::list & _colors, const pybind::list & _indices );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void _update( const UpdateContext * _context ) override;
		void _render( const RenderContext * _state ) override;

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
		inline const VectorRenderVertex2D & getVerticesWM() const;
        
	protected:
        SurfacePtr m_surface;
		
		typedef Vector<mt::vec3f> VectorPosition;
		typedef Vector<mt::vec2f> VectorUV;
		typedef Vector<mt::vec4f> VectorColor;
		VectorPosition m_positions;
		VectorUV m_uvs;
		VectorColor m_colors;

		VectorRenderIndices m_indices;

		mutable VectorRenderVertex2D m_verticesWM;
			
		mutable bool m_invalidateVerticesWM;
		mutable bool m_invalidateVerticesColor;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const VectorRenderVertex2D & Meshget::getVerticesWM() const
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
