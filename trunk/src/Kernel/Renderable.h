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
		virtual void render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask ) = 0;

	public:
		virtual void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera );
		virtual void _debugRender( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask );

	public:
		virtual void hide( bool _value );
		inline bool isHide() const;

	public:
		void localHide( bool _value );
		inline bool isLocalHide() const;

	protected:
		virtual void _localHide( bool _value );
		virtual void _hide( bool _value );

	public:
		bool checkVisibility( const Viewport& _viewport );
		inline void invalidateVisibility();
		inline bool isInvalidateVisibility() const;
		inline bool getVisibility() const;

	protected:
		virtual bool _checkVisibility( const Viewport & _viewport ) = 0;
		
	protected:
		bool m_hide;
		bool m_localHide;

		bool m_visibility;
		bool m_invalidateVisibility;
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
	//////////////////////////////////////////////////////////////////////////
	inline void Renderable::invalidateVisibility()
	{
		m_invalidateVisibility = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Renderable::isInvalidateVisibility() const
	{
		return m_invalidateVisibility;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Renderable::getVisibility() const
	{
		return m_visibility;
	}
}
