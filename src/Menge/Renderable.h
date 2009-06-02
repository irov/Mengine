#	pragma once

#	include "Viewport.h"

namespace Menge
{
	class Renderable
	{
	public:
		Renderable();

	public:
		virtual void render( unsigned int _debugMask ) = 0;
		virtual void _render( unsigned int _debugMask ) = 0;

	public:
		virtual void hide( bool _value );
		inline bool isHide() const;

	public:
		virtual bool _checkVisibility( const Viewport & _viewport ) = 0;

	public:
		bool checkVisibility();
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
