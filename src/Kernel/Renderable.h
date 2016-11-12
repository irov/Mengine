#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Core/Viewport.h"

namespace Menge
{
	class Renderable
	{
	public:
		Renderable();

	public:
		virtual void render( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state, unsigned int _debugMask ) = 0;

	public:
		virtual void _render( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state );
		virtual void _debugRender( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state, unsigned int _debugMask );

	public:
		virtual void hide( bool _value );
		inline bool isHide() const;

	public:
		void localHide( bool _value );
		inline bool isLocalHide() const;

	protected:
		virtual void _localHide( bool _value );
		virtual void _hide( bool _value );
		
	protected:
		bool m_hide;
		bool m_localHide;
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool Renderable::isHide() const
	{
		return m_hide;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Renderable::isLocalHide() const
	{
		return m_localHide;
	}
}
