#	pragma once

#	include "Core/ColourValue.h"

namespace Menge
{
	class Colorable
	{
	public:
		Colorable();

	public:
		void setPersonalColor( const ColourValue& _color );
		inline const ColourValue & getPersonalColor() const;

		void setPersonalAlpha( float _alpha );
		inline float getPersonalAlpha() const;

		void setLocalColor( const ColourValue& _color );
		inline const ColourValue & getLocalColor() const;

		void setLocalAlpha( float _alpha );
		inline float getLocalAlpha() const;

	public:
		inline bool isLocalTransparent() const;
		inline bool isPersonalTransparent() const;
        
	protected:
		inline const ColourValue & getRelationColor() const;

	public:
		void invalidateColor();
		inline bool isInvalidateColor() const;

	protected:
		const ColourValue& updateRelationColor( const ColourValue& _parentColor ) const;

	protected:
        virtual void _setPersonalColor( const ColourValue& _color );
        virtual void _setPersonalAlpha( float _alpha );

		virtual void _invalidateColor();

	protected:
		ColourValue m_colorPersonal;
		ColourValue m_colorLocal;		

		mutable ColourValue m_colorRelation;
		mutable bool m_invalidateColor;

		bool m_localTransparent;
		bool m_personalTransparent;
	};
	//////////////////////////////////////////////////////////////////////////
	inline bool Colorable::isLocalTransparent() const
	{
		return m_localTransparent;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Colorable::isPersonalTransparent() const
	{
		return m_personalTransparent;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & Colorable::getRelationColor() const
	{
		return m_colorRelation;
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
        float alpha = m_colorPersonal.getA();

		return alpha;
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
