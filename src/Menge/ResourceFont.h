#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"
#	include <vector>
#	include <map>

class RenderImageInterface;
class DataStreamInterface;

namespace Menge
{
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

		//! Возвращает коэффициент пропорциональности глифа, т.е. его отношение ширины к высоте.
		/*!
		\param _id индекс глифа
		\return коэффициент пропорциональности глифа
		*/
		float getCharRatio( char _id ) const;

		//! Возвращает изображение для шрифта.
		/*!
		\return изображение
		*/
		const RenderImageInterface * getImage() const;

		//! Возвращает аутлан - изображение для шрифта.
		/*!
		\return изображение
		*/
		const RenderImageInterface * getOutlineImage() const;

		void setFontPath( const std::string & _path );

		int getInitSize();

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:

		void setGlyph( unsigned int _id, float _u1, float _v1, float _u2, float _v2 );
		std::string getFontDir( const std::string & _fontName );

		bool parseFontdef( DataStreamInterface * _stream );
		bool parseAttribute( const std::string & name, const std::string & params );

	private:

		struct Glyph
		{
			Glyph( unsigned int _id, const mt::vec4f& _rect, float _ratio )
				: id( _id )
				, rect( _rect )
				, ratio( _ratio )
			{}
			unsigned int id;
			mt::vec4f rect;
			float ratio;
		};

		typedef std::map<unsigned int, Glyph> TMapGlyph;

		TMapGlyph	m_glyphs;

		std::string m_filename;
		std::string m_fontDir;
		std::string m_fullname;

		float m_whsRatio;
		int m_initSize;

		RenderImageInterface * m_image;

		RenderImageInterface * m_outline;
	};
}