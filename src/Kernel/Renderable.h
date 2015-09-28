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
		virtual void render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderClipplaneInterface * _clipplane, unsigned int _debugMask ) = 0;

	public:
		virtual void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderClipplaneInterface * _clipplane );
		virtual void _debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderClipplaneInterface * _clipplane, unsigned int _debugMask );

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
