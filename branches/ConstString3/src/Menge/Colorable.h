#	pragma once

#	include "Loadable.h"

#	include "Core/ColourValue.h"

namespace Menge
{
	class Colorable
		: virtual public Loadable
	{
	public:
		Colorable();

	public:
		void setFullBlend( bool _value );
		inline bool isFullBlend() const;

	public:
		void setLocalColor( const ColourValue& _color );
		inline const ColourValue & getLocalColor() const;

		void setLocalAlpha( float _alpha );
		inline float getLocalAlpha() const;

	public:
		void loader( BinParser * _parser ) override;

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
		ColourValue m_colorLocal;
		bool m_fullBlend;

		mutable ColourValue m_colorWorld;
		mutable bool m_invalidateColor;
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
