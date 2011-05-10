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
		ResourceImageCell();

	public:
		std::size_t getCount() const override;

		const mt::vec2f & getMaxSize( std::size_t _frame ) const override;
		const mt::vec2f & getSize( std::size_t _frame ) const override;
		const mt::vec2f & getOffset( std::size_t _frame ) const override;
		const mt::vec4f & getUV( std::size_t _frame ) const override;
		const mt::vec4f & getUVImage( std::size_t _frame ) const override;

		std::size_t getNumX() const;
		std::size_t getNumY() const;

		bool isAlpha( std::size_t _frame ) const;

		Texture* getTexture( std::size_t _frame ) const override;

		const ConstString & getFilename( std::size_t _frame ) const override;
		const ConstString & getCodecType( std::size_t _frame ) const override;
		std::size_t getFilenameCount() const override;

	public:
		void loader( BinParser * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		std::size_t m_numX;
		std::size_t m_numY;

		typedef std::vector<mt::vec4f> TVectorUV;
		TVectorUV m_uvs;

		ImageFrame m_imageFrame;

	private:
		struct ImageDesc
		{
			ConstString fileName;
			ConstString codecType;
			mt::vec4f uv;
			mt::vec2f offset;
			mt::vec2f maxSize;
			mt::vec2f size;
			bool isAlpha;
		};

		ImageDesc m_imageDesc;
	};
}
