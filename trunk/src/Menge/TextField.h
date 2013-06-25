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
	class ResourceFont;
	class ResourceImage;

	struct RenderMaterial;

	//! TextField - ����� ��� ��������� ������. 

	/*! xml - ���� ����� ��������� ���������:
	* <Node Name = "���_����" Type = "TextField">
	*  <Font Name = "���_������_������"/>
	*  <Text Value = "������������_�����"/>
	*  <Color Value = "����"/>
	*  <Height Value = "������ �����"/>
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
		void setMaxLen( float _len );
		float getMaxLen() const;

		void setText( const String& _text );
		const String& getText() const;

		const String & getDefaultText() const;


		void setTextByKey( const ConstString& _key );
		const ConstString & getTextKey() const;

		void setTextByKeyFormat( const ConstString& _key, const String & _format, size_t _number );

		void setFontHeight( float _height );
		float getFontHeight() const;

		void setOutlineColor( const ColourValue& _color );
		const ColourValue& getOutlineColor() const;

		void enableOutline( bool _value );
		bool isOutline() const;

		void setLineOffset( float _offset );
		float getLineOffset() const;

		void setResourceFont( const ConstString& _resName );
		const ConstString & getResourceFont() const;

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
        void updateVertexDataWM_( TVectorVertex2D & _outVertex, const TVectorVertex2D & _fromVertex );

        void invalidateVerticesWM_() const;

		inline TVectorVertex2D & getOutlineVertices();
		inline TVectorVertex2D & getTextVertices();

		void updateVertexData_( const ColourValue & _color, TVectorVertex2D& _vertexData );

	private:
		const TListTextLine & getTextLines() const;

		void invalidateTextLines() const;
		inline bool isInvalidateTextLines() const;
		
		void updateTextLines_() const;

	private:
		ResourceFont * m_resourceFont;
		ConstString m_resourceFontName;
		ConstString m_key;

		ColourValue m_outlineColor;

		String m_text;
		String m_format;
		size_t m_number;

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

		TVectorVertex2D m_vertexDataText;
		TVectorVertex2D m_vertexDataOutline;

        TVectorVertex2D m_vertexDataTextWM;
        TVectorVertex2D m_vertexDataOutlineWM;

		mutable bool m_invalidateVertices;
        mutable bool m_invalidateVerticesWM;

		mutable bool m_invalidateTextLines;
	};
	//////////////////////////////////////////////////////////////////////////
	inline TVectorVertex2D & TextField::getOutlineVertices()
	{
		if( m_invalidateVerticesWM == true )
		{
			this->updateVerticesWM_();
		}

		return m_vertexDataOutline;
	}
	//////////////////////////////////////////////////////////////////////////
	inline TVectorVertex2D & TextField::getTextVertices()
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
