#	pragma once

#	include "ResourceImage.h"

#	include <vector>

namespace Menge
{
	//! ResourceImageCell - ресурс-файл, который хранит размеченное изображение. 

	/*! xml - файл имеет следующую структуру:
	* <Resource Name = "имя_ресурса" Type = "ResourceImageCell" >
	*	<File Path = "имя_файла"/>
	*   <Cell X = "количество картинок по горизонтали"/>
	*   <Cell Y = "количество картинок по вертикали"/>
	* </Resource>
	*/

	class ResourceImageCell
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageCell )

	public:
		ResourceImageCell( const std::string & _name );

	public:
		unsigned int getCount() const override;

		const mt::vec2f & getMaxSize( unsigned int _frame ) const override;
		const mt::vec2f & getSize( unsigned int _frame ) const override;
		const mt::vec2f & getOffset( unsigned int _frame ) const override;
		const mt::vec4f & getUV( unsigned int _frame ) const override;		

		const RenderImageInterface * getImage( unsigned int _frame ) const override;

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		std::string m_filename;

		unsigned int m_numX;
		unsigned int m_numY;

		mt::vec2f m_offset;
		
		typedef std::vector<mt::vec4f> TVectorUV;
		TVectorUV m_uvs;

		ImageFrame m_imageFrame;
	};
}