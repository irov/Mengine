#	pragma once

#	include "Loadable.h"

#	include "Core/Viewport.h"

namespace Menge
{
	class Camera2D;

	class Renderable
		: virtual public Loadable
	{
	public:
		Renderable();

	public:
		virtual void render( Camera2D * _camera ) = 0;

	public:
		virtual void _render( Camera2D * _camera );
		virtual void _debugRender( Camera2D* _camera, unsigned int _debugMask );

	public:
		virtual void hide( bool _value );
		inline bool isHide() const;

	protected:
		virtual void _hide( bool _value );

	public:
		void loader( BinParser * _xml ) override;

	public:
		bool checkVisibility( const Viewport& _viewport );
		inline void invalidateVisibility();
		inline bool isInvalidateVisibility() const;
		inline bool getVisibility() const;

	protected:
		virtual bool _checkVisibility( const Viewport & _viewport ) = 0;

	protected:
		bool m_hide;
		bool m_visibility;
		bool m_invalidateVisibility;
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool Renderable::isHide() const
	{
		return m_hide;
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
