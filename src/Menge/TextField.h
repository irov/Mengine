# pragma once

#	include "Config/Typedef.h"

#	include "Kernel/Node.h"

#	include "Core/ColourValue.h"
#	include "Core/ValueInterpolator.h"
#	include "Core/ConstString.h"

#   include "ResourceFont.h"

#	include "TextLine.h"

#	include "Math/vec4.h"

#	include <list>

namespace Menge
{
	struct RenderMaterial;

	//! TextField - класс для отрисовки шрифта. 

	/*! xml - файл имеет следующую структуру:
	* <Node Name = "имя_ноды" Type = "TextField">
	*  <Font Name = "имя_ресура_шрифта"/>
	*  <Text Value = "отображаемый_текст"/>
	*  <Color Value = "цвет"/>
	*  <Height Value = "высота глифа"/>
	*  <CenterAlign Value = "1/0"/>
	* </Node>
	*/

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

	typedef std::list<TextLine> TListTextLine;

	class TextField
		: public Node
	{
	public:
		TextField();
		~TextField();

    public:
        void setResourceFont( ResourceFont * _resourceFont );
        ResourceFont * getResourceFont() const;

	public:
		void setMaxLen( float _len );
		float getMaxLen() const;

		void setText( const String & _text );
		const String & getText() const;
		
		void setTextByKey( const ConstString& _key );
		const ConstString & getTextKey() const;

		void setTextByKeyFormat( const ConstString& _key, const String & _format, const String & _arg );

		void setFontHeight( float _height );
		float getFontHeight() const;

		void setOutlineColor( const ColourValue& _color );
		const ColourValue& getOutlineColor() const;

		void enableOutline( bool _value );
		bool isOutline() const;

		void setLineOffset( float _offset );
		float getLineOffset() const;

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
		
		int getMaxCharCount() const;
		void setMaxCharCount( int _maxCharCount );

		void setCharOffset( float _offset );
		float getCharOffset() const;

		void setPixelsnap( bool _pixelsnap );
		bool getPixelsnap() const;

	public:
		const mt::vec2f& getTextSize() const;

    public:
		int getCharCount() const;

	protected:
		void _render( RenderCameraInterface * _camera ) override;

	private:
		void renderOutline_( RenderCameraInterface * _camera );

	protected:	
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;

		void _invalidateWorldMatrix() override;
		void _updateBoundingBox( mt::box2f & _boundingBox ) override;
		void _invalidateColor() override;

	private:
		float getHorizontAlignOffset_( const TextLine & _line );

	private:
		void updateVertices_();
		void invalidateVertices_() const;

        void updateVerticesWM_();
        void updateVertexDataWM_( TVectorRenderVertex2D & _outVertex, const TVectorRenderVertex2D & _fromVertex );

        void invalidateVerticesWM_() const;

		inline TVectorRenderVertex2D & getOutlineVertices();
		inline TVectorRenderVertex2D & getTextVertices();

		void updateVertexData_( const ColourValue & _color, TVectorRenderVertex2D& _vertexData );

	private:
		const TListTextLine & getTextLines() const;

		void invalidateTextLines() const;
		inline bool isInvalidateTextLines() const;
		
		void updateTextLines_() const;

	private:
        ResourceHolder<ResourceFont> m_resourceFont;

        ConstString m_key;

		ColourValue m_outlineColor;

		String m_text;

		String m_format;
		String m_formatArg;

		float m_fontHeight;		
		
		ETextFieldHorizontAlign m_horizontAlign;
		ETextFieldVerticalAlign m_verticalAlign;

		float m_maxWidth;
		float m_charOffset;
		int m_maxCharCount;

		mutable int m_charCount;
		mutable mt::vec2f m_textSize;

		bool m_outline;
		bool m_pixelsnap;

		float m_lineOffset;
				
		mutable TListTextLine m_lines;

		const RenderMaterial * m_materialText;
		const RenderMaterial * m_materialOutline;

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
}
