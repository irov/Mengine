#	pragma once

#	include "Core/ColourValue.h"

namespace Menge
{
	class Colorable
	{
	public:
		Colorable();

	public:
		void setFullBlend( bool _value );
		inline bool isFullBlend() const;

	public:
		void setPersonalColor( const ColourValue& _color );
		inline const ColourValue & getPersonalColor() const;

		void setPersonalAlpha( float _alpha );
		inline float getPersonalAlpha() const;

		void setLocalColor( const ColourValue& _color );
		inline const ColourValue & getLocalColor() const;

		void setLocalAlpha( float _alpha );
		inline float getLocalAlpha() const;

	protected:
		inline const ColourValue & getRelationColor() const;

	public:
		void invalidateColor();
		inline bool isInvalidateColor() const;

	protected:
		const ColourValue& updateRelationColor( const ColourValue& _parentColor ) const;

	protected:
		virtual void _invalidateColor();

	protected:
		ColourValue m_colorPersonal;

		ColourValue m_colorLocal;		

		mutable ColourValue m_colorWorld;
		mutable bool m_invalidateColor;

		bool m_fullBlend;
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool Colorable::isFullBlend() const
	{
		return m_fullBlend;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & Colorable::getRelationColor() const
	{
		return m_colorWorld;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Colorable::isInvalidateColor() const
	{
		return m_invalidateColor;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const ColourValue & Colorable::getPersonalColor() const
	{
		return m_colorPersonal;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Colorable::getPersonalAlpha() const
	{
		return m_colorPersonal.getA();
	}
	//////////////////////////////////////////////////////////////////////////
	inline const ColourValue& Colorable::getLocalColor() const
	{
		return m_colorLocal;
	}
	//////////////////////////////////////////////////////////////////////////
	inline float Colorable::getLocalAlpha() const
	{
		return m_colorLocal.getA();
	}
}
