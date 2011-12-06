#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"
#	include "Core/ColourValue.h"

#	include <vector>
#	include <map>

namespace Menge
{
	class Texture;
	class DataStreamInterface;
	class ResourceGlyph;

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
		ResourceFont();

	public:
		//! Возвращает uv - координаты глифа.
		/*!
		\param _id индекс глифа
		\return uv - координаты глифа
		*/
		mt::vec4f getUV( unsigned int _id ) const;

		//! Возвращает отношение ширины к высоте глифа.
		/*!
		\param _id индекс глифа
		\return коэффициент пропорциональности глифа
		*/
		float getCharRatio( unsigned int _id ) const;

		const mt::vec2f& getOffset( unsigned int _char ) const;
		const mt::vec2f& getSize( unsigned int _char ) const;

		const Texture * getImage() const;
		const Texture * getOutlineImage() const;

		float getInitSize() const;

		const ConstString& getImagePath() const;
		const ConstString& getOutlineImagePath() const;

		const ColourValue & getColor() const;

	public:
		void loader( BinParser * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		void setImagePath_( const ConstString& _path );
		void setOutlineImagePath_( const ConstString& _path );

	private:
		ConstString m_glyphResource;
		ResourceGlyph * m_glyph;
		
		ConstString m_imageFile;
		ConstString m_imageCodec;

		String m_fontdefPath;

		ConstString m_outlineImageFile;
		ConstString m_outlineImageCodec;

		float m_whsRatio;
		float m_initSize;
		float m_textureRatio;

		Texture* m_image;
		Texture* m_outline;
		mt::vec2f m_imageInvSize;

		ColourValue m_color;
	};
}
