#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"
#	include <vector>
#	include <map>

namespace Menge
{
	class Texture;
	class DataStreamInterface;

	//! ResourceFont - ресурс-файл, который содержит изображение, список uv - координат и коэффициентов пропорциональности. 

    /*! xml - файл имеет следующую структуру:
	* <Resource Name = "имя_ресурса" Type = "ResourceFont" >
	*	<File Path = "имя_файла"/>
	* </Resource>
	*/

	class ResourceFont
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceFont )

	public:
		//! Конструктор.
		/*!
		\param _name имя ресурса.
		*/
		ResourceFont( const ResourceFactoryParam & _params );

	public:
		//! Возвращает uv - координаты глифа.
		/*!
		\param _id индекс глифа
		\return uv - координаты глифа
		*/
		const mt::vec4f & getUV( unsigned int _id ) const;

		//! Возвращает отношение ширины к высоте глифа.
		/*!
		\param _id индекс глифа
		\return коэффициент пропорциональности глифа
		*/
		float getCharRatio( unsigned int _id ) const;

		//! Возвращает изображение для шрифта.
		/*!
		\return изображение
		*/
		Texture* getImage();

		//! Возвращает аутлан - изображение для шрифта.
		/*!
		\return изображение
		*/
		Texture* getOutlineImage();
		float getInitSize() const;

		const String& getFontdefPath() const;
		const String& getImagePath() const;
		const String& getOutlineImagePath() const;

		const mt::vec2f& getOffset( unsigned int _char ) const;
		const mt::vec2f& getSize( unsigned int _char ) const;

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:

		void setFontdefPath_( const String& _path );
		void setImagePath_( const String& _path );
		void setOutlineImagePath_( const String& _path );

		void setGlyph( unsigned int _id, const mt::vec4f& _uv, const mt::vec2f& _offset, float _ratio, const mt::vec2f& _size );
		String getFontDir( const String& _fontName );

		bool parseFontdef( DataStreamInterface * _stream );
		bool parseAttribute( const String& name, const String& params );

		void loaderFontdef_( XmlElement* _xml );
		void addGlyph_( const String& _glyph, const String& _rect, const String& _offset, int _width );
	private:

		struct Glyph
		{
			Glyph( unsigned int _id, const mt::vec4f& _uv, const mt::vec2f& _offset, float _ratio, const mt::vec2f& _size )
				: id( _id )
				, uv( _uv )
				, offset( _offset )
				, ratio( _ratio )
				, size( _size )
			{}
			unsigned int id;
			mt::vec4f uv;
			mt::vec2f offset;
			float ratio;
			mt::vec2f size;
		};

		typedef std::map<unsigned int, Glyph> TMapGlyph;
		TMapGlyph	m_glyphs;

		String m_fontdefFile;
		String m_imageFile;
		String m_outlineImageFile;

		float m_whsRatio;
		float m_initSize;
		float m_textureRatio;

		Texture* m_image;
		Texture* m_outline;
		mt::vec2f m_imageInvSize;
	};
}
