# pragma once

#	include "Config/Typedef.h"

#	include "Kernel/Node.h"

#	include "Core/ColourValue.h"
#	include "Core/ValueInterpolator.h"
#	include "Core/ConstString.h"

#	include "TextLine.h"

#	include "Math/vec4.h"

#	include <list>

namespace Menge
{
	enum ETextFieldHorizontAlign
	{
		ETFHA_NONE = 0,
		ETFHA_CENTER = 1,
		ETFHA_LEFT = 2,
		ETFHA_RIGHT = 3
	};

	enum ETextFieldVerticalAlign
	{
		ETFVA_NONE = 0,
		ETFVA_CENTER = 1,
	};

	typedef std::vector<TextLine> TVectorTextLine;

	class TextField
		: public Node
	{
	public:
		TextField();
		~TextField();

	public:
		void setMaxLen( float _len );
		float getMaxLen() const;

	public:
		void setText( const String & _text );
		const String & getText() const;
		
		void setTextID( const ConstString& _key );
		const ConstString & getTextID() const;

		void setTextByKeyFormat( const ConstString& _key, const String & _format, const String & _arg );

	protected:
		void clearTextParams_();

	public:
		void setFontName( const ConstString & _name );
		const ConstString & getFontName() const;

	public:
		float getFontHeight() const;

	public:
		void setFontColor( const ColourValue & _color );
		const ColourValue& getFontColor() const;
		
		void enableOutline( bool _value );
		bool isOutline() const;

		void setOutlineColor( const ColourValue & _color );
		const ColourValue& getOutlineColor() const;
				
		void setLineOffset( float _offset );
		float getLineOffset() const;

		void setCharOffset( float _offset );
		float getCharOffset() const;

	public:
		void setNoneAlign();
		bool isNoneAlign() const;

		void setCenterAlign();
		bool isCenterAlign() const;

		void setRightAlign();
		bool isRightAlign() const;

		void setLeftAlign();
		bool isLeftAlign() const;

		void setVerticalNoneAlign();
		bool isVerticalNoneAlign() const;

		void setVerticalCenterAlign();
		bool isVerticalCenterAlign() const;
		
		size_t getMaxCharCount() const;
		void setMaxCharCount( int _maxCharCount );

		void setPixelsnap( bool _pixelsnap );
		bool getPixelsnap() const;

	public:
		const mt::vec2f& getTextSize() const;

    public:
		size_t getCharCount() const;

	protected:
		void _render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera ) override;

	protected:
		void renderOutline_( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera );

	protected:	
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;

		void _invalidateWorldMatrix() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _invalidateColor() override;

	protected:
		float getHorizontAlignOffset_( const TextLine & _line );

	protected:
		void updateVertices_();
		void invalidateVertices_() const;

        void updateVerticesWM_();
        void updateVertexDataWM_( TVectorRenderVertex2D & _outVertex, const TVectorRenderVertex2D & _fromVertex );

        void invalidateVerticesWM_() const;

		inline TVectorRenderVertex2D & getOutlineVertices();
		inline TVectorRenderVertex2D & getTextVertices();

		void updateVertexData_( const ColourValue & _color, TVectorRenderVertex2D& _vertexData );

	protected:
		const TVectorTextLine & getTextLines() const;

		void invalidateTextLines() const;
		inline bool isInvalidateTextLines() const;
		
		void updateTextLines_() const;

	protected:
		inline TextFontInterface * getFont() const;
		inline void invalidateFont();
		void updateFont_() const;

	protected:
		mutable TextFontInterface * m_font;
		mutable bool m_invalidateFont;

	protected:
		inline const RenderMaterialInterfacePtr & getMaterialFont();
		inline const RenderMaterialInterfacePtr & getMaterialOutline();
				
	protected:
		mutable RenderMaterialInterfacePtr m_materialFont;
		mutable RenderMaterialInterfacePtr m_materialOutline;

	protected:
		const ConstString & calcFontName() const;
		float calcLineOffset() const;
		float calcCharOffset() const;

		const ColourValue & calcColorFont() const;
		const ColourValue & calcColorOutline() const;

	protected:
        ConstString m_key;

		const TextEntryInterface * m_textEntry;

		String m_text;

		String m_format;
		String m_formatArg;
				
		ETextFieldHorizontAlign m_horizontAlign;
		ETextFieldVerticalAlign m_verticalAlign;

		float m_maxWidth;

		ConstString m_fontName;

		float m_lineOffset;
		float m_charOffset;

		ColourValue m_colorFont;
		ColourValue m_colorOutline;

		size_t m_fontParams;
		
		size_t m_maxCharCount;

		mutable size_t m_charCount;
		mutable mt::vec2f m_textSize;

		bool m_outline;
		bool m_pixelsnap;
				
				
		mutable TVectorTextLine m_lines;

		TVectorRenderVertex2D m_vertexDataText;
		TVectorRenderVertex2D m_vertexDataOutline;

        TVectorRenderVertex2D m_vertexDataTextWM;
        TVectorRenderVertex2D m_vertexDataOutlineWM;

		mutable bool m_invalidateVertices;
        mutable bool m_invalidateVerticesWM;

		mutable bool m_invalidateTextLines;
	};
	//////////////////////////////////////////////////////////////////////////
	inline TVectorRenderVertex2D & TextField::getOutlineVertices()
	{
		if( m_invalidateVerticesWM == true )
		{
			this->updateVerticesWM_();
		}

		return m_vertexDataOutlineWM;
	}
	//////////////////////////////////////////////////////////////////////////
	inline TVectorRenderVertex2D & TextField::getTextVertices()
	{
		if( m_invalidateVerticesWM == true )
		{
			this->updateVerticesWM_();
		}

		return m_vertexDataTextWM;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool TextField::isInvalidateTextLines() const
	{
		return m_invalidateTextLines;
	}
	//////////////////////////////////////////////////////////////////////////
	inline TextFontInterface * TextField::getFont() const
	{
		if( m_invalidateFont == true )
		{
			this->updateFont_();
		}

		return m_font;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void TextField::invalidateFont()
	{
		m_invalidateFont = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const RenderMaterialInterfacePtr & TextField::getMaterialFont()
	{
		if( m_invalidateFont == true )
		{
			this->updateFont_();
		}

		return m_materialFont;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const RenderMaterialInterfacePtr & TextField::getMaterialOutline()
	{
		if( m_invalidateFont == true )
		{
			this->updateFont_();
		}

		return m_materialOutline;
	}
}
