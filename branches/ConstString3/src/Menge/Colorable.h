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
		void setLocalColor( const ColourValue& _color );
		void setLocalAlpha( float _alpha );

		inline const ColourValue & getRelationColor() const;
		inline const ColourValue & getLocalColor() const;

	public:
		void loader( BinParser * _parser ) override;

	public:
		void invalidateColor();
		inline bool isInvalidateColor() const;

	protected:
		const ColourValue& updateRelationColor( const ColourValue& _parentColor ) const;

	protected:
		virtual void _invalidateColor();

	protected:
		ColourValue m_colorLocal;

		mutable ColourValue m_colorWorld;
		mutable bool m_invalidateColor;
	};
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
}
