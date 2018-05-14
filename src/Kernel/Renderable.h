#pragma once

#include "Core/Viewport.h"
#include "Core/Mixin.h"
#include "Core/IntrusivePtr.h"

namespace Mengine
{
    class RenderServiceInterface;
    struct RenderState;

	class Renderable
        : public Mixin
	{
	public:
		Renderable();
        ~Renderable();

	public:
		virtual void render( RenderServiceInterface * _renderService, const RenderState * _state, uint32_t _debugMask ) = 0;

	public:
		virtual void _render( RenderServiceInterface * _renderService, const RenderState * _state );
		virtual void _debugRender( RenderServiceInterface * _renderService, const RenderState * _state, uint32_t _debugMask );

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
