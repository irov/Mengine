#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"
#	include "Core/ColourValue.h"

#	include <vector>
#	include <map>

namespace Menge
{
	class RenderTextureInterface;
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

		const mt::vec2f & getOffset( unsigned int _char ) const;
		float  getKerning( unsigned int _charPrev, unsigned int _charCur ) const;
		const mt::vec2f & getSize( unsigned int _char ) const;

		const RenderTextureInterface * getImage() const;
		const RenderTextureInterface * getOutlineImage() const;

		float getInitSize() const;

		const WString & getImagePath() const;
		const WString & getOutlineImagePath() const;

		const ColourValue & getColor() const;

	public:
		void loader( BinParser * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		void setImagePath_( const WString & _path );
		void setOutlineImagePath_( const WString & _path );

	private:
		ConstString m_glyphResource;
		ResourceGlyph * m_glyph;
		
		WString m_imageFile;
		ConstString m_imageCodec;

		//String m_fontdefPath;

		WString m_outlineImageFile;
		ConstString m_outlineImageCodec;

		float m_whsRatio;
		float m_initSize;
		float m_textureRatio;

		RenderTextureInterface* m_image;
		RenderTextureInterface* m_outline;
		mt::vec2f m_imageInvSize;

		ColourValue m_color;
	};
}
