#	pragma once

#	include "TTFServiceInterface.h"

#	include "Kernel/Node.h"

namespace Menge
{
	class TTFText
		: public Node
	{
	public:
		TTFText();
		~TTFText();

	public:
		void setTextID( const ConstString & _key );
		const ConstString & getTextID() const;

	public:
		void setFontName( const ConstString & _name );
		const ConstString & getFontName() const;

	protected:
		void _render( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state ) override;

	protected:
		void _invalidateWorldMatrix() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) const override;
		void _invalidateColor() override;

	protected:
		inline const TTFFontInterfacePtr & getFont() const;

	protected:
		void updateFont_() const;

	protected:
		inline const U32String & getText();

	protected:		
		inline const TVectorRenderVertex2D & getVertices();
		inline const TVectorRenderVertex2D & getVerticesWM();

	protected:
		void updateText_();
		void updateVertices_();
		void updateVerticesWM_();

	protected:
		inline void invalidateText() const;
		inline void invalidateFont() const;
		inline void invalidateVertices() const;
		inline void invalidateVerticesWM() const;

	protected:
		ConstString m_fontName;
		mutable TTFFontInterfacePtr m_font;
		
		ConstString m_key;
		U32String m_text;

		TVectorRenderVertex2D m_vertexText;
		TVectorRenderVertex2D m_vertexTextWM;

		mutable bool m_invalidateText;
		mutable bool m_invalidateFont;
		mutable bool m_invalidateVertices;
		mutable bool m_invalidateVerticesWM;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const TTFFontInterfacePtr & TTFText::getFont() const
	{
		if( m_invalidateFont == true )
		{
			this->updateFont_();
		}

		return m_font;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void TTFText::invalidateText() const
	{
		m_invalidateText = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void TTFText::invalidateFont() const
	{
		m_invalidateFont = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void TTFText::invalidateVertices() const
	{
		m_invalidateVertices = true;

		this->invalidateVerticesWM();
	}
	//////////////////////////////////////////////////////////////////////////
	inline void TTFText::invalidateVerticesWM() const
	{
		m_invalidateVerticesWM = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const U32String & TTFText::getText()
	{
		if( m_invalidateText == true )
		{
			this->updateText_();
		}

		return m_text;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const TVectorRenderVertex2D & TTFText::getVerticesWM()
	{
		if( m_invalidateVerticesWM == true )
		{
			this->updateVerticesWM_();
		}

		return m_vertexTextWM;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const TVectorRenderVertex2D & TTFText::getVertices()
	{
		if( m_invalidateVertices == true )
		{
			this->updateVertices_();
		}

		return m_vertexText;
	}
}