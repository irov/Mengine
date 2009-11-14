#	pragma once

#	include "Core/Viewport.h"

namespace Menge
{
	class Camera2D;

	class Renderable
	{
	public:
		Renderable();

	public:
		virtual void render( Camera2D * _camera ) = 0;
		virtual void _render( Camera2D * _camera ) = 0;

	public:
		virtual void hide( bool _value );
		inline bool isHide() const;

	public:
		virtual bool _checkVisibility( const Viewport & _viewport ) = 0;

	public:
		bool checkVisibility( const Viewport& _viewport );
		void changeVisibility();

	protected:
		bool m_hide;
		bool m_visibility;
		bool m_changeVisibility;
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool Renderable::isHide() const
	{
		return m_hide;
	}
}
