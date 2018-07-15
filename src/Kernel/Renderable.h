#pragma once

#include "Interface/RenderViewportInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderScissorInterface.h"
#include "Interface/RenderTargetInterface.h"
#include "Interface/RenderMaterialInterface.h"

#include "Kernel/Viewport.h"
#include "Kernel/Mixin.h"
#include "Kernel/RenderVertex2D.h"
#include "Kernel/RenderIndex.h"
#include "Kernel/IntrusivePtr.h"

#include "math/box2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct RenderContext
    {
        RenderViewportInterfacePtr viewport;
        RenderCameraInterfacePtr camera;
        RenderScissorInterfacePtr scissor;
        RenderTargetInterfacePtr target;

        uint32_t debugMask;
    };
    //////////////////////////////////////////////////////////////////////////
	class Renderable
        : public Mixin
	{
	public:
		Renderable();
        ~Renderable();

	public:
		virtual void render( const RenderContext * _state ) = 0;

	public:
		virtual void _render( const RenderContext * _state );
		virtual void _debugRender( const RenderContext * _state );

	public:
		virtual void setHide( bool _hide );
		inline bool getHide() const;

	public:
		void setLocalHide( bool _localHide );
		inline bool getLocalHide() const;

    public:
        void setExternalRender( bool _externalRender );
        inline bool getExternalRender() const;

	protected:
        virtual void _setHide( bool _hide );
		virtual void _setLocalHide( bool _localHide );		
        virtual void _setExternalRender( bool _externalRender );

    protected:
        void addRenderObject( const RenderContext * _state, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const RenderIndex * _indices, uint32_t _indexCount
            , const mt::box2f * _bb, bool _debug ) const;

        void addRenderQuad( const RenderContext * _state, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const mt::box2f * _bb, bool _debug ) const;

        void addRenderLine( const RenderContext * _state, const RenderMaterialInterfacePtr & _material
            , const RenderVertex2D * _vertices, uint32_t _vertexCount
            , const mt::box2f * _bb, bool _debug ) const;

    protected:
        RenderVertex2D * getDebugRenderVertex2D( uint32_t _count ) const;
        const RenderMaterialInterfacePtr & getDebugMaterial() const;
		
	protected:
		bool m_hide;
		bool m_localHide;
        bool m_externalRender;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Renderable> RenderablePtr;
	//////////////////////////////////////////////////////////////////////////
	inline bool Renderable::getHide() const
	{
		return m_hide;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Renderable::getLocalHide() const
	{
		return m_localHide;
	}
    //////////////////////////////////////////////////////////////////////////
    inline bool Renderable::getExternalRender() const
    {
        return m_externalRender;
    }
}
